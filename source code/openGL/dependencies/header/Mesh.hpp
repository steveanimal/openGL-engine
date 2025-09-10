#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <glm.hpp>            
#include <gtc/matrix_transform.hpp> 
#include <gtc/type_ptr.hpp> 

#include <stb_image.h>

#include <Window.hpp>
#include <Utils.hpp>

#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <unordered_map>

#ifndef MODEL_PATH

    #define MODEL_PATH "resource/model"

#endif // MODEL_PATH


namespace gl {

    struct vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

#define VERT_SIZE sizeof(vertex)

    class object {
    private:
        std::vector<vertex> vertices;
        std::vector<unsigned int> indices;

        GLuint VAO, VBO, EBO;

        enum class TextureType {
            BaseColor,
            Normal,
            MetallicRoughness,
            Occlusion,
            Emissive,
            ExtraDiffuse
        };

        struct TexEntry {
            std::string name;          // logical name (e.g. "baseColor", "normal")
            gl::texture2D* text;    // pointer to texture
        };

        std::vector<TexEntry> textures;

        Assimp::Importer* importer{ nullptr }; // keep alive for embedded textures

    public:
        object(const std::string& modelPath, const std::string& externalTexPath = "") {
            importer = new Assimp::Importer();

            const aiScene* scene = importer->ReadFile(
                modelPath,
                aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
            );

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
                throw std::runtime_error("ASSIMP ERROR: " + std::string(importer->GetErrorString()));

            // process scene
            processNode(scene->mRootNode, scene);
            setupMesh();

            // If external texture path provided, push it as baseColor (diffuse)
            if (!externalTexPath.empty()) {
                textures.push_back({ "baseColor", new gl::texture2D(externalTexPath) });
            }

            // If GLB, attempt to read material textures (we collect multiple)
            if (modelPath.size() >= 4 && modelPath.substr(modelPath.size() - 4) == ".glb") {
                for (unsigned m = 0; m < scene->mNumMaterials; ++m) {
                    aiMaterial* mat = scene->mMaterials[m];

                    // helper lambda to load a texture by aiTextureType
                    auto loadFromMaterial = [&](aiTextureType type, const std::string& logicalName) {
                        if (mat->GetTextureCount(type) == 0) return;
                        aiString str;
                        mat->GetTexture(type, 0, &str);
                        if (str.C_Str()[0] == '*') {
                            int texIndex = atoi(str.C_Str() + 1);
                            aiTexture* tex = scene->mTextures[texIndex];

                            unsigned char* data = nullptr;
                            int width = 0, height = 0, channels = 0;

                            if (tex->mHeight == 0) {
                                // compressed (png/jpg) in memory
                                data = stbi_load_from_memory(
                                    reinterpret_cast<unsigned char*>(tex->pcData),
                                    tex->mWidth, // bytes
                                    &width, &height, &channels, 0
                                );
                                if (!data) {
                                    std::cerr << "Failed to decode embedded compressed texture\n";
                                    return;
                                }
                                textures.push_back({ logicalName, new gl::texture2D(data, width, height, channels) });
                                stbi_image_free(data);
                            }
                            else {
                                // raw RGBA RGBA ... size = width*height*4
                                width = tex->mWidth;
                                height = tex->mHeight;
                                channels = 4;
                                unsigned char* raw = new unsigned char[width * height * 4];
                                memcpy(raw, tex->pcData, width * height * 4);
                                textures.push_back({ logicalName, new gl::texture2D(raw, width, height, channels) });
                                delete[] raw;
                            }
                        }
                        else {
                            // external reference in the material (file path)
                            std::string p = str.C_Str();
                            // construct absolute path relative to model directory
                            std::filesystem::path modelDir = std::filesystem::path(modelPath).parent_path();
                            std::filesystem::path texPath = modelDir / p;
                            if (std::filesystem::exists(texPath)) {
                                textures.push_back({ logicalName, new gl::texture2D(texPath.string()) });
                            }
                            else {
                                std::cerr << "Material texture file not found: " << texPath.string() << "\n";
                            }
                        }
                        };

                    loadFromMaterial(aiTextureType_DIFFUSE, "baseColor");
                    loadFromMaterial(aiTextureType_NORMALS, "normal");
                    loadFromMaterial(aiTextureType_METALNESS, "metallicRoughness");
                    loadFromMaterial(aiTextureType_AMBIENT_OCCLUSION, "occlusion");
                    loadFromMaterial(aiTextureType_EMISSIVE, "emissive");
                    // note: glTF uses combined metallicRoughness; Assimp mapping may vary across exporters
                }
            }
        }

        ~object() {
            for (auto& t : textures) {
                delete t.text;
            }
            textures.clear();
            if (importer) delete importer;
            if (VAO) glDeleteVertexArrays(1, &VAO);
            if (VBO) glDeleteBuffers(1, &VBO);
            if (EBO) glDeleteBuffers(1, &EBO);
        }

        void setTexture2D(gl::texture2D* tex, unsigned index) {
            textures[index].text = tex;
        }

        void draw(GLuint shaderProgram, const glm::vec3& pos, const glm::vec2& rot, const glm::vec3& scl) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
            model = glm::scale(model, scl);

            drawWithModel(shaderProgram, model);
        }

    private:
        inline std::filesystem::path getPath(const std::string& relativePath) {
            std::filesystem::path exePath = std::filesystem::current_path();
            return exePath / relativePath;
        }

        void drawWithModel(GLuint shaderProgram, const glm::mat4& model) {
            GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Bind textures sequentially. Uniform naming rule: <logical><index>
            // e.g. baseColor0, normal0, occlusion0 etc. You must match these in your shader.
            int unit = 0;
            for (size_t i = 0; i < textures.size(); ++i) {
                const TexEntry& te = textures[i];
                glActiveTexture(GL_TEXTURE0 + unit);
                te.text->bind(GL_TEXTURE_2D);

                std::string uniformName = te.name + std::to_string(0); // e.g. "baseColor0"
                GLint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
                if (loc >= 0) glUniform1i(loc, unit);
                unit++;
            }

            // draw
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // reset unit
            glActiveTexture(GL_TEXTURE0);
        }

        void loadModel(const std::string& path) {
            if (!importer) importer = new Assimp::Importer();

            const aiScene* scene = importer->ReadFile(
                getPath(path).string(),
                aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
            );

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
                throw std::runtime_error("ASSIMP ERROR: " + std::string(importer->GetErrorString()));

            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode* node, const aiScene* scene) {
            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                processMesh(mesh);
            }
            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], scene);
            }
        }

        void processMesh(aiMesh* mesh) {
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                vertex v;
                v.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                if (mesh->HasNormals()) v.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                else v.Normal = glm::vec3(0.0f);
                if (mesh->mTextureCoords[0]) v.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                else v.TexCoords = glm::vec2(0.0f, 0.0f);
                vertices.push_back(v);
            }

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }
        }

        void setupMesh() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            // Position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

            // Normal
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, Normal));

            // TexCoords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, TexCoords));

            glBindVertexArray(0);
        }
    };

}
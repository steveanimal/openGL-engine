#include <window.hpp>
#include <Utils.hpp>
#include <Game.hpp>
#include <Mesh.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>

#include <iostream>
#include <windows.h>

void renderFps(float fps, float avgFps, float maxFps) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(350, 200));
    ImGui::Begin("Debug");
    ImGui::Text("    fps: %.1f", fps);
    ImGui::Text("avg fps: %.1f", avgFps);
    ImGui::Text("max fps: %.1f", maxFps);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
    if (!glfwInit()) return -1;

    gl::window window(1920, 1080, "window");

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);

    if (glewInit() != GLEW_OK) return -1;

    glfwSwapInterval(0);

    gl::shader shader("resource/shader/vert.glsl", "resource/shader/frag.glsl");

    shader.useProgram();

    //"resource/texture/grassblock_top.png",
    //"resource/texture/dirtblock.png",
    //"resource/texture/grassblock.png",

    gl::object house_model("resource/model/house.obj", "resource/texture/house_texture.png");
    gl::object player_model("resource/model/player.glb");

    gl::player player(gl::camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)), shader);

    float maxFps = 0.0f;
    float avgFps = 0.0f;
    float fps = 0.0f;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.FontGlobalScale = 3.0f;

    // 2. Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // 3. Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (window.run()) {
        // Clear screen
        window.clearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Use shader and bind texture
        // Process keyboard input
        // Process mouse movement
        // View & Projection from camera
        // Send matrices
        player.update(window, shader);

        // Draw cube
        house_model.draw(shader.getProgram(), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f), glm::vec3(0.1f));
        player_model.draw(shader.getProgram(), player.getPos() - glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

        fps = window.getFps();
        avgFps = (avgFps + fps) / 2;
        if (fps > maxFps) maxFps = fps;

        renderFps(fps, avgFps, maxFps);

        window.swapBuffers();
    }

    glfwTerminate();

    std::cout << "avg fps: " << avgFps <<
               "\nmax fps: " << maxFps << std::endl;

    system("pause");
}

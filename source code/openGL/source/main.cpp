#include <Game.hpp>
#include <window.hpp>
#include <Utils.hpp>
#include <Mesh.hpp>

#include <iostream>
//#include <windows.h>

int main() {
    if (!glfwInit()) return -1;

    gl::window window(1920, 1080, "window");
    if (!window) throw std::runtime_error("error init window");
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    window.vsync(ENABLE_ADAPTIVE_VSYNC);    

    if (glewInit() != GLEW_OK) return -1;

    gl::shader shader("resource/shader/vert.glsl", "resource/shader/frag.glsl");

    shader.useProgram();

    gl::object awp("resource/model/awp.glb");
    gl::object model("resource/model/player.glb");

    gl::player player(gl::camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)), shader);

    float maxFps = 0.0f;
    float avgFps = 0.0f;
    float fps = 0.0f;

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

        model.draw(shader.getProgram(), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f));

        glm::vec3 pos = getWeaponOffset(player.getCam());
        glm::quat rot = getWeaponRotation(player.getCam());
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot);
        model = glm::scale(model, glm::vec3(1.0f));

        awp.draw(shader.getProgram(), model);

        std::cout << player.getPos().x << std::endl;

        fps = window.getFps();
        avgFps = (avgFps + fps) / 2;
        if (fps > maxFps) maxFps = fps;

        window.swapBuffers();
    }

    glfwTerminate();

    std::cout << "avg fps: " << avgFps <<
               "\nmax fps: " << maxFps << std::endl;

    //system("pause");
}

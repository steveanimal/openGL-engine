#include <window.hpp>
#include <Utils.hpp>
#include <Game.hpp>
#include <Mesh.hpp>

#include <iostream>
//#include <windows.h>

int main() {
    if (!glfwInit()) return -1;

    gl::window window(1920, 1080, "window");
    window.vsync(ENABLE_ADAPTIVE_VSYNC);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);

    if (glewInit() != GLEW_OK) return -1;

    gl::imgui ui(window.getWindow(), "debug");

    ui.setFont(3.0f);
    ui.setWindowSize(250, 300);

    gl::shader shader("resource/shader/vert.glsl", "resource/shader/frag.glsl");

    shader.useProgram();

    //"resource/texture/grassblock_top.png",
    //"resource/texture/dirtblock.png",
    //"resource/texture/grassblock.png",

    gl::object house_model("resource/model/house.obj", "resource/texture/house_texture.png");

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
        house_model.draw(shader.getProgram(), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f), glm::vec3(0.1f));

        fps = window.getFps();
        avgFps = (avgFps + fps) / 2;
        if (fps > maxFps) maxFps = fps;

        ui.clearText();
        ui.addText(std::string("    fps: ") + std::to_string(fps));
        ui.addText(std::string("avg fps: ") + std::to_string(avgFps));
        ui.addText(std::string("max fps: ") + std::to_string(maxFps));

        ui.render();

        window.swapBuffers();
    }

    glfwTerminate();

    std::cout << "avg fps: " << avgFps <<
               "\nmax fps: " << maxFps << std::endl;

    system("pause");
}

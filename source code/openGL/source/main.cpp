#include <Game.hpp>
#include <window.hpp>
#include <Utils.hpp>
#include <Mesh.hpp>

#include <iostream>
//#include <windows.h>

#define WEAPON_OFFSET glm::vec3(-0.3f, -0.35f, 0.2f)

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

    gl::player player(gl::camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), shader.getProgram()));

    unsigned char zoom = 0;
    bool ifpress = false;
    float fov = 60.0f;

    while (window.run()) {
        // Clear screen
        window.clearColor(1.0f, 1.0f, 1.0f, 1.0f);

        if (glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !ifpress) {
            ifpress = true;

            zoom = (zoom + 1) % 3;

            if (zoom == 0) fov = 60.0f;
            else if (zoom == 1) fov = 35.0f;
            else if (zoom == 2) fov = 20.0f;
        }

        if (ifpress && glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) ifpress = false;

        // update the projection matrix
        player.setFov(glm::mix(player.getFov(), fov, 15.0f * window.getDeltaTime()), (float)window.getWidth() / (float)window.getHeight());

        player.update(window, shader);

        // Draw cube
        model.draw(shader.getProgram(), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));

        awp.draw(shader.getProgram(), gl::getItemModel(player.getCam(), WEAPON_OFFSET, glm::vec3(1.0f)));

        awp.draw(shader.getProgram(), glm::vec3(10.0f), glm::vec3(1.0f), glm::vec3(0.0f));

        window.swapBuffers();
    }

    glfwTerminate();
}

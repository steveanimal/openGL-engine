#pragma once

#include <Window.hpp>
#include <Utils.hpp>

namespace gl {

	class player {
	private:
		gl::camera m_Camera;
		glm::vec3 m_Velocity;
		gl::uniform m_Model;
		gl::uniform m_View;
		gl::uniform m_Proj;
	public:
		player(gl::camera cam, GLuint shader) 
			: m_Camera(cam), m_Velocity(glm::vec3(0.0f))
		{
			m_Model = gl::uniform(glm::mat4(1.0f), glGetUniformLocation(shader, "model"));
			m_View = gl::uniform(glm::mat4(1.0f), glGetUniformLocation(shader, "view"));
			m_Proj = gl::uniform(glm::mat4(1.0f), glGetUniformLocation(shader, "projection"));
		}

		player(gl::camera cam, gl::shader shader)
			: m_Camera(cam), m_Velocity(glm::vec3(0.0f))
		{
			m_Model = gl::uniform(glm::mat4(1.0f), shader.getUniformLocation("model"));
			m_View = gl::uniform(glm::mat4(1.0f), shader.getUniformLocation("view"));
			m_Proj = gl::uniform(glm::mat4(1.0f), shader.getUniformLocation("projection"));
		}

		player(gl::camera cam, GLuint modelLoc, GLuint viewLoc, GLuint projLoc)
			: m_Camera(cam), m_Velocity(glm::vec3(0.0f))
		{
			m_Model = gl::uniform(glm::mat4(1.0f), modelLoc);
			m_View = gl::uniform(glm::mat4(1.0f), viewLoc);
			m_Proj = gl::uniform(glm::mat4(1.0f), projLoc);
		}

		void update(gl::window& window, gl::shader shader) {
			shader.useProgram();
			m_Camera.processKeyboard(window);
			m_Camera.processCursor(window.getCursorX(), window.getCursorY());

			m_View = m_Camera.getViewMatrix();
			m_Proj = glm::perspective(glm::radians(60.0f), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 10000.0f);

			m_Model.uniformMatrix4fv();
			m_View.uniformMatrix4fv();
			m_Proj.uniformMatrix4fv();
		}

		void setModel(glm::mat4& other) { m_Model = other; }

		void setView(glm::mat4& other) { m_View = other; }

		void setProj(glm::mat4& other) { m_Proj = other; }

		glm::mat4 getProj() const { return m_Proj.getValue(); }

		gl::camera getCam() { return m_Camera; }

		glm::vec3 getPos() const { return m_Camera.getPos(); }
	};

}
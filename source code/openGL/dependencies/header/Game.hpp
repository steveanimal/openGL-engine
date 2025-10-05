#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>

#include <cmath>

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
		player(gl::camera cam) 
			: m_Camera(cam), m_Velocity(glm::vec3(0.0f))
		{
			m_Model = gl::uniform(glm::mat4(1.0f), glGetUniformLocation(m_Camera.getShader(), "model"));
			m_View = gl::uniform(glm::mat4(1.0f), glGetUniformLocation(m_Camera.getShader(), "view"));
			m_Proj = gl::uniform(glm::mat4(1.0f), glGetUniformLocation(m_Camera.getShader(), "projection"));
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

			m_Camera.processInput(window);

			m_View = m_Camera.getViewMatrix();
			m_Proj = glm::perspective(glm::radians(m_Camera.getFov()), (float)window.getWidth() / (float)window.getHeight(), 0.001f, 10000.0f);

			m_Model.uniformMatrix4fv();
			m_View.uniformMatrix4fv();
			m_Proj.uniformMatrix4fv();
		}

		void setFov(const float& fov, const float& aspect) { m_Camera.setFov(fov); }

		const float getFov() const { return m_Camera.getFov(); }

		void setSens(const float& other) { m_Camera.setSens(other); }

		void setModel(glm::mat4& other) { m_Model = other; }

		void setView(glm::mat4& other) { m_View = other; }

		void setProj(glm::mat4& other) { m_Proj = other; }

		glm::mat4 getProj() const { return m_Proj.getValue(); }

		gl::camera getCam() { return m_Camera; }

		glm::vec3 getPos() const { return m_Camera.getPos(); }

		glm::vec3 getDirRadians() const { return m_Camera.getFront(); }
	};

	glm::mat4 getItemModel(const camera& cam, const glm::vec3& offset, const glm::vec3& scale) {
		return glm::scale(
			glm::translate(
				glm::translate(glm::mat4(1.0f), cam.getPos()) *
				glm::inverse(glm::lookAt(glm::vec3(0.0f), -cam.getFront(), cam.getUpVector())),
				offset
			),
			scale
		);
	}
}
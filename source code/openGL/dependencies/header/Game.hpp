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
			m_Camera.processInput(window);

			m_View = m_Camera.getViewMatrix();
			m_Proj = glm::perspective(glm::radians(60.0f), (float)window.getWidth() / (float)window.getHeight(), 0.001f, 10000.0f);

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

		glm::vec3 getPosRadians() const { return m_Camera.getDirection(); }
	};

	static const glm::vec3 kHoldOffset(0.35f, -0.25f, 0.6f);   // right, down, forward
	static const glm::vec3 kHoldRotation(-10.0f, 10.0f, 0.0f); // pitch, yaw, roll

	// ----------------------------
	// Returns weapon world position
	glm::vec3 getWeaponOffset(const gl::camera& cam)
	{
		// Base offsets in camera space
		glm::vec3 rightOffset(0.3f, -0.25f, -0.5f); // right, down, forward
		// right: positive x, down: negative y, forward: negative z

		// Camera axes
		glm::vec3 camRight = glm::normalize(glm::cross(cam.getDirection(), cam.getUpVector()));
		glm::vec3 camUp = glm::normalize(cam.getUpVector());
		glm::vec3 camForward = glm::normalize(cam.getDirection());

		// Transform offset to world space
		glm::vec3 worldOffset =
			camRight * rightOffset.x +
			camUp * rightOffset.y +
			camForward * rightOffset.z;

		return cam.getPos() + worldOffset;
	}

	glm::quat getWeaponRotation(const gl::camera& cam)
	{
		glm::vec3 handTilt(10.0f, 0.0f, 0.0f); // pitch, yaw, roll in degrees

		glm::vec3 camDir = cam.getDirection();
		glm::vec3 camUp = cam.getUpVector();

		// Prevent degenerate lookAt when looking straight up/down
		if (glm::abs(glm::dot(glm::normalize(camDir), glm::normalize(camUp))) > 0.999f)
		{
			// almost parallel, choose a fixed up (world up)
			camUp = glm::vec3(0, 1, 0);
			if (glm::dot(glm::normalize(camDir), camUp) > 0.999f)
				camUp = glm::vec3(0, 0, -1); // fallback if looking straight up
		}

		glm::quat camRot = glm::quatLookAt(glm::normalize(camDir), glm::normalize(camUp));
		glm::quat tilt = glm::quat(glm::radians(handTilt));

		return camRot * tilt;
	}


}
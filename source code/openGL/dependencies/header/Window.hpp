#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>            
#include <gtc/matrix_transform.hpp>  
#include <gtc/type_ptr.hpp>  

#include <string>
#include <stdexcept>
#include <iostream>

namespace gl {

	class window {
	private:
		struct RGBA {
			GLfloat r, g, b, a;
		};

		struct shader {
			GLuint shader;

			void bind(GLuint shaderProgram) {
				shader = shaderProgram;
			}
		};

		void onMouseMove(double xpos, double ypos) {
			m_CursorX = xpos;
			m_CursorY = ypos;
		}

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
			if (width <= 0 || height <= 0) return;

			gl::window* self = static_cast<gl::window*>(glfwGetWindowUserPointer(window));

			self->m_Width = width;
			self->m_Height = height;

			glViewport(0, 0, width, height);
		}

		static void mouse_callback(GLFWwindow* glfwWindow, double xpos, double ypos) {
			// Get the instance of your window class
			gl::window* self = static_cast<gl::window*>(glfwGetWindowUserPointer(glfwWindow));

			// Forward the data to your instance method
			self->onMouseMove(xpos, ypos);
		}

		static void window_focus_callback(GLFWwindow* window, int focused) {
			gl::window* self = static_cast<gl::window*>(glfwGetWindowUserPointer(window));

			self->hideCursor = focused;
		}

	private:
		GLFWwindow* m_Window;
		int m_Width;
		int m_Height;
		bool ifinit;
		GLuint m_Shader;
		double m_TargetAspect;
		double m_CursorX, m_CursorY;
		bool hideCursor;
		float deltaTime;
		float currentFrame;
		float lastFrame;
	public:
		window() = delete;

		window(int width, int height, const char* name, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr) 
			: m_Width(width),m_Height(height), ifinit(true), lastFrame((float)glfwGetTime())
		{
			glEnable(GL_DEPTH_TEST);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			m_Window = glfwCreateWindow(width, height, name, monitor, share);

			if (!m_Window) { 
				ifinit = false;
				throw std::runtime_error("Failed to create GLFW window");
			}

			glfwSetWindowUserPointer(m_Window, this);

			glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
			glfwSetWindowFocusCallback(m_Window, window_focus_callback);

			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			
			glfwMakeContextCurrent(m_Window);
		}

		~window() {
			glfwDestroyWindow(m_Window);
		}

		inline const int getWidth() const { return m_Width; }

		inline const int getHeight() const { return m_Height; }

		inline GLFWwindow* getWindow() const { return m_Window; }

		inline const GLuint getShader() const { return m_Shader; }

		inline const float getTargetAspect() const { return m_TargetAspect; }

		inline void setTargetAspect(float aspect) { m_TargetAspect = aspect; }

		inline const double getCursorX() const { return m_CursorX; }

		inline const double getCursorY() const { return m_CursorY; }

		inline const bool ifHideCursor() const { return hideCursor; }

		inline const float getDeltaTime() const { return deltaTime; }

		inline const float getFps() const { return 1 / deltaTime; }

		bool run() {
			glfwMakeContextCurrent(m_Window);
			glfwPollEvents();

			currentFrame = (float)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			
			if (hideCursor && glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetCursorPosCallback(m_Window, nullptr);
				hideCursor = false;
			}

			if (!hideCursor && glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				double x = m_CursorX;
				double y = m_CursorY;
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPosCallback(m_Window, mouse_callback);
				glfwSetCursorPos(m_Window, x, y);
				hideCursor = true;
			}
			
			return !(glfwGetKey(m_Window, GLFW_KEY_HOME) == GLFW_PRESS || glfwWindowShouldClose(m_Window));
		}

		void clearColor(RGBA color) {
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void clearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			glClearColor(r, g, b, a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void swapBuffers() {
			glfwSwapBuffers(m_Window);
		}

		void linkShader(GLuint shaderProgram) {
			m_Shader = shaderProgram;
			glfwSetWindowUserPointer(m_Window, this);
		}

		void drawArray(GLuint shaderProgram, GLuint VAO, GLenum mode, GLint first, GLsizei count) {
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glDrawArrays(mode, first, count);
		}

		void drawElements(GLuint shaderProgram, GLuint VAO, GLsizei count, const GLvoid* indices) {
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
		}

		void drawElements(GLuint VAO, GLsizei count, const GLvoid* indices) {
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
		}

		void drawElementsWithTexture3D(GLuint VAO, GLsizei count, const GLvoid* indices, GLuint texture) {
			glBindTexture(GL_TEXTURE_3D, texture);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
		}

		void renderQuad(GLuint shaderProgram, GLuint VAO, GLenum mode, GLint first, GLsizei count) {
			drawArray(shaderProgram, VAO, mode, first, count);
		}

		int getKeyPress(int key) { return glfwGetKey(m_Window, key) == GLFW_PRESS; }

		int getKeyHold(int key) { return glfwGetKey(m_Window, key) == GLFW_REPEAT; }

		int getKeyRelease(int key) { return glfwGetKey(m_Window, key) == GLFW_RELEASE; }

		bool operator!() { return !ifinit; }
	};

}
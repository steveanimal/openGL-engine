#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>            
#include <gtc/matrix_transform.hpp>  
#include <gtc/type_ptr.hpp>  

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>

#define ENABLE_NORMAL_VSYNC 1
#define ENABLE_ADAPTIVE_VSYNC -1
#define DISABLE_VSYNC 0

namespace gl {

	class camera;

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

		static void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
			// Get the instance of your window class
			gl::window* self = static_cast<gl::window*>(glfwGetWindowUserPointer(window));

			// Forward the data to your instance method
			self->onMouseMove(xpos, ypos);
		}

		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
			// Get the instance of your window class
			gl::window* self = static_cast<gl::window*>(glfwGetWindowUserPointer(window));

			// Forward the data to your instance method
		}

		static void window_focus_callback(GLFWwindow* window, int focused) {
			gl::window* self = static_cast<gl::window*>(glfwGetWindowUserPointer(window));

			self->hideCursor = focused;
		}

		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
			// xoffset → horizontal scroll (usually 0)
			// yoffset → vertical scroll (positive = scroll up, negative = scroll down)
			gl::window* self = static_cast<gl::window*>(glfwGetWindowUserPointer(window));
			self->scrollX = xoffset;
			self->scrollY = yoffset;
		}


	private:
		GLFWwindow* m_Window;
		int m_Width;
		int m_Height;
		bool ifinit;
		GLuint m_Shader;
		double m_TargetAspect;
		double m_CursorX, m_CursorY;
		double m_LastCursorX, m_LastCursorY;
		bool hideCursor;
		float deltaTime;
		float currentFrame;
		float lastFrame;
		int m_Vsync;
		double scrollX;
		double scrollY;
		int mouseButton;
		int mouseAction;
		int mode;
	public:
		window() = delete;

		window(int width, int height, const char* name, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr) 
			: m_Width(width),m_Height(height), ifinit(true), lastFrame((float)glfwGetTime())
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
			glfwSetScrollCallback(m_Window, scroll_callback);
			glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
			glfwSetCursorPosCallback(m_Window, nullptr);

			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			
			glfwMakeContextCurrent(m_Window);
		}

		~window() {
			glfwDestroyWindow(m_Window);
		}

		inline const int getCursorButton() const { return mouseButton; }

		inline const int getCursorAction() const { return mouseAction; }

		inline const int getCursorMode() const { return mode; }

		inline const int getWidth() const { return m_Width; }

		inline const int getHeight() const { return m_Height; }

		inline GLFWwindow* getWindow() const { return m_Window; }

		inline const GLuint getShader() const { return m_Shader; }

		inline const float getTargetAspect() const { return m_TargetAspect; }

		inline void setTargetAspect(float aspect) { m_TargetAspect = aspect; }

		inline const double getCursorX() const { return m_CursorX; }

		inline const double getCursorY() const { return m_CursorY; }

		inline const double getLastCursorX() const { return m_LastCursorX; }

		inline const double getLastCursorY() const { return m_LastCursorY; }

		inline void setLastCursorX(double other) { m_LastCursorX = other; }

		inline void setLastCursorY(double other) { m_LastCursorY = other; }

		inline const double getScrollX() const { return scrollX; }

		inline const double getScrollY() const { return scrollY; }

		inline void resetScrollX() { scrollX = 0.0f; }

		inline void resetScrollY() { scrollY = 0.0f; }

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
				glfwSetCursorPos(m_Window, (float)m_Width / (float)2, (float)m_Height / (float)2);
				hideCursor = false;
			}

			if (!hideCursor && glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				glfwSetCursorPos(m_Window, m_CursorX, m_CursorY);
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPosCallback(m_Window, mouse_pos_callback);
				hideCursor = true;
			}
			
			return !(glfwGetKey(m_Window, GLFW_KEY_HOME) == GLFW_PRESS || glfwWindowShouldClose(m_Window));
		}

		void clearColor(RGBA color) {
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void clearColor(glm::vec4 color) {
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

		void vsync(int type) { 
			glfwSwapInterval(type); 
			m_Vsync = type;
		}

		const int vsync() const { return m_Vsync; }

		int getKeyPress(int key) { return glfwGetKey(m_Window, key) == GLFW_PRESS; }

		int getKeyHold(int key) { return glfwGetKey(m_Window, key) == GLFW_REPEAT; }

		int getKeyRelease(int key) { return glfwGetKey(m_Window, key) == GLFW_RELEASE; }

		bool operator!() { return !ifinit; }
	};

	struct UIElement {
		int type;
		std::string label;
		void* data;
		float minVal, maxVal;
	};

	class imgui {
	private:
		GLFWwindow* m_Window;
		const char* m_Title;
		ImGuiIO* m_IO;

		enum ElementType {
			TEXT = 0,
			CHECKBOX,
			SLIDER_FLOAT,
			SLIDER_INT,
			BUTTON,
			INPUT_TEXT
		};

		std::vector<UIElement> m_Elements;
	public:
		imgui(GLFWwindow* window, const char* title)
			: m_Window(window), m_Title(title)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			m_IO = &ImGui::GetIO();
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
			ImGui_ImplOpenGL3_Init("#version 330");
		}

		void addText(const std::string& text) {
			m_Elements.push_back({ TEXT, text, nullptr, 0.0f, 0.0f });
		}

		void clearElements() {
			m_Elements.clear();
		}

		void addCheckbox(const std::string& label, bool* value) {
			m_Elements.push_back({ CHECKBOX, label, value, 0.0f, 0.0f });
		}

		void addSliderFloat(const std::string& label, float* value, float min, float max) {
			m_Elements.push_back({ SLIDER_FLOAT, label, value, min, max });
		}

		void addSliderInt(const std::string& label, int* value, int min, int max) {
			m_Elements.push_back({ SLIDER_INT, label, value, (float)min, (float)max });
		}

		void setFont(float size) { m_IO->FontGlobalScale = size; }

		void setWindowSize(unsigned x, unsigned y) { ImGui::SetNextWindowSize(ImVec2(x, y)); }

		void render() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin(m_Title);

			for (UIElement& e : m_Elements) {
				switch (e.type) {
				case TEXT:
					ImGui::Text("%s", e.label.c_str());
					break;
				case CHECKBOX:
					ImGui::Checkbox(e.label.c_str(), (bool*)e.data);
					break;
				case SLIDER_FLOAT:
					ImGui::SliderFloat(e.label.c_str(), (float*)e.data, e.minVal, e.maxVal);
					break;
				case SLIDER_INT:
					ImGui::SliderInt(e.label.c_str(), (int*)e.data, (int)e.minVal, (int)e.maxVal);
					break;
				case BUTTON:
					if (ImGui::Button(e.label.c_str())) {
						// TODO: store callback
					}
					break;
				}
			}

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		~imgui() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	};

}
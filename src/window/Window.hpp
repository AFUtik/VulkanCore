#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

struct GLFWwindow;

class Window {
public:
	static Window& instance() {
		static Window window; 
		return window;
	}

	int init(int width, int height, const char* title);

	Window() {};
	~Window();

	bool isShouldClose();
	void setShouldClose(bool flag);
	void swapBuffers();
	void setCursorMode(int mode);
	bool wasWindowResized() { return frameBufferResized; }
	void resetWindowResizedFlag() { frameBufferResized = false; }

	static int getWidth()  {return width;}
	static int getHeight() {return height;}
	static GLFWwindow* getGlfwWindow() {return window;}
private:
	static void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
	
	inline static int width  = 800;
	inline static int height = 600;
	inline static GLFWwindow* window = nullptr;
	inline static bool frameBufferResized = false;
};
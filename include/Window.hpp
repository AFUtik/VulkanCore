#pragma once

struct GLFWwindow;

class Window {
public:
	int init(int width, int height, const char* title);

	Window() {};
	~Window();

	bool isShouldClose();
	void setShouldClose(bool flag);
	void swapBuffers();
	void setCursorMode(int mode);
	bool wasWindowResized() { return frameBufferResized; }
	void resetWindowResizedFlag() { frameBufferResized = false; }

	int getWidth()  {return width;}
	int getHeight() {return height;}
	GLFWwindow* getGlfwWindow() {return window;}
private:
	static void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
	
	int width  = 800;
	int height = 600;
	GLFWwindow* window = nullptr;
	bool frameBufferResized = false;

    friend struct Events;
};


#pragma once

#include "Camera.hpp"

class Engine {
public:
	static constexpr int WIDTH = 1920;
	static constexpr int HEIGHT = 1080;

	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void run();
private:
	CameraOrtho camera;
};

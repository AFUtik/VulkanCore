#pragma once

#include "window/Events.hpp"
#include "window/Window.hpp"

#include "vk/Renderer.hpp"
#include "vk/FrameInfo.hpp"
#include "vk/Descriptors.hpp"
#include "vk/Buffer.hpp"

#include "Camera.hpp"

#include <memory>
#include <vector>

#include "Fonts.hpp"


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

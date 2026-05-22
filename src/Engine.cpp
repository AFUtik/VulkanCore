#include "Engine.hpp"
#include "GLFW/glfw3.h"
#include "Global.hpp"

#include "game/GameContext.hpp"
#include "rendering/BaseMesh.hpp"
#include "rendering/Renderer.hpp"
#include "vk/Device.hpp"

#include "vk/Mesh.hpp"
#include "vk/RenderTarget.hpp"
#include "vk/VkTexture.hpp"
#include "window/Events.hpp"
#include "window/Window.hpp"

#include <memory>
#include <thread>
#include <iostream>
#include <csignal>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "game/PCManager.hpp"

#define WIDTH 1720
#define HEIGHT 880

Engine::Engine() : camera(RENDER_WIDTH, RENDER_HEIGHT) 
{
	// Component Managers //
	registerPlanetComponents();

	Window::instance().init(WIDTH, HEIGHT, "Vulkan Engine");

	Events::init();
}

Engine::~Engine() {}

void Engine::run() {
	Window& window = Window::instance();
	global.renderer = std::make_unique<Renderer>();
	//global.assets.load();

	Events::toggle_cursor();
	double lastTime = glfwGetTime();
	double timeAccu = 0.0;
	double physAccu = 0.0;
	double alpha = 0.0;
	const double target_fps = 60.0;
	const float H = 1.0f / target_fps;
	const float speed = 320.0;
	//float camX = 0.0f;
	//float camY = 0.0f;

	while (!window.isShouldClose()) {
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;
		lastTime = currentTime;

		timeAccu += frameTime;
		physAccu += frameTime;
		while(physAccu >= GameContext::tickPhisicsDelta)
		{
			global.gameCtx.tick();
			physAccu -= GameContext::tickPhisicsDelta;
		}
		global.gameCtx.alpha = physAccu / GameContext::tickPhisicsDelta;
		if (timeAccu >= H) {
			//guiEventListener.listen();
			//guiRenderer->syncAll();

			if (Events::pressed(GLFW_KEY_W)) {
				camera.translate(Vec3(0.0, 1.0, 0.0) * H * speed);
			}
			if (Events::pressed(GLFW_KEY_S)) {
				camera.translate(Vec3(0.0, -1.0, 0.0) * H * speed);
			}
			if (Events::pressed(GLFW_KEY_D)) {
				camera.translate(Vec3(1.0, 0.0, 0.0) * H * speed);
			}
			if (Events::pressed(GLFW_KEY_A)) {
				camera.translate(Vec3(-1.0, 0.0, 0.0) * H * speed);
			}
			if (Events::pressed(GLFW_KEY_G)) {
				camera.addZoom(0.005f);
				
			}
			if (Events::pressed(GLFW_KEY_H)) {
				camera.addZoom(-0.005f);
			}

			if (Events::jpressed(GLFW_KEY_TAB)) {
				Events::toggle_cursor();
			}
			if (Events::jpressed(GLFW_KEY_ESCAPE)) {
				window.setShouldClose(true);
			}

			/* FOR 3D
			if (Events::_cursor_locked) {
				camY += -Events::deltaY / Window::getHeight() * 2;
				camX += -Events::deltaX / Window::getHeight() * 2;

				if (camY < -glm::radians(89.0f)) {
					camY = -glm::radians(89.0f);
				}
				if (camY > glm::radians(89.0f)) {
					camY = glm::radians(89.0f);
				}
				camera.rotate(-camY, camX, 0);
			}
			*/

			camera.updateView();

			global.renderer->render(camera);
			
			timeAccu -= H;
		}
		else {
			double sleepTime = H - timeAccu;
			std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
		}
		Events::pullEvents();
	}

	vkDeviceWaitIdle(myvk::Device::instance().device());
	global.renderer.reset();
}
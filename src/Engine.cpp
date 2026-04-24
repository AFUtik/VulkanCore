#include "Engine.hpp"
#include "Global.hpp"


//#include "gui/GUIContext.hpp"
//#include "gui/GUIRendering.hpp"
//#include "gui/GuiContext.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/GlobalRenderSystem.hpp"

#include "rendering/Renderer.hpp"
#include "texture/Texture.hpp"
#include "texture/TextureAtlas.hpp"
#include "vk/Device.hpp"
#include "vk/Material.hpp"
#include "vk/Mesh.hpp"
#include "vk/RenderTarget.hpp"
#include "vk/VkTexture.hpp"
#include "window/Events.hpp"
#include "window/Window.hpp"

#include "model/Mesh.hpp"

#include <memory>
#include <string>
#include <thread>
#include <numbers>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <iostream>

#define WIDTH 1920
#define HEIGHT 1080

#define RENDER_WIDTH 1920
#define RENDER_HEIGHT 1080

Engine::Engine() : camera(RENDER_WIDTH, RENDER_HEIGHT) 
{
	Window::instance().init(WIDTH, HEIGHT, "Vulkan Engine");

	Events::init();
}

Engine::~Engine() {}

void Engine::run() {
	Window& window = Window::instance();
	global.renderer = std::make_unique<Renderer>();

	Tileset tileset(RESOURCE_PATH+"img/tileset.png", 16, 16, 1);
	TextureUtils::save(&tileset.texture, RESOURCE_PATH+"img/paddedTileset.png");

	AtlasBuilder& atlasBuilder = global.atlasBuilder;
	atlasBuilder.reserve(64);
	atlasBuilder.setPadding(1);

	Texture stone(RESOURCE_PATH+"img/stone.png");
	Texture tuff (RESOURCE_PATH+"img/tuff.png");
	Texture sand (RESOURCE_PATH+"img/sand.png");

	TextureAtlas atlas;
	atlas.texture = Texture(64, 64, TextureChannels::RGBA);

	atlasBuilder.pack(&atlas, &stone, "stone");
	atlasBuilder.pack(&atlas, &tuff,  "tuff");
	atlasBuilder.pack(&atlas, &sand,  "sand");
	atlasBuilder.build(&atlas);
	TextureUtils::save(&atlas.texture, RESOURCE_PATH+"img/atlas.png");

	Mesh mesh;

	int segments = 32;
	float radius = 50.0f;

	mesh.vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f});
	for (int i = 0; i <= segments; i++) {
		float angle = i * 2.0f * std::numbers::pi / segments;
		float x = cos(angle) * radius;
		float y = sin(angle) * radius;

		mesh.vertices.push_back({x, y, 0.0f, 0.0f, 0.0f, 0.0, 0.0f, 1.0f, 1.0f});
	}
	for (int i = 1; i <= segments; i++) {
		mesh.indices.push_back(0);
		mesh.indices.push_back(i);
		mesh.indices.push_back(i + 1);
	}
	
	myvk::Mesh vkMesh = myvk::Mesh();
	vkMesh.updateBuffers(mesh.vertices, mesh.indices);
	
	Mesh screenMesh;
	screenMesh.vertices.push_back({1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	screenMesh.vertices.push_back({1.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	screenMesh.vertices.push_back({-1.0f,  -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	screenMesh.vertices.push_back({-1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});

	screenMesh.indices.push_back(0);
	screenMesh.indices.push_back(1);
	screenMesh.indices.push_back(2);
	screenMesh.indices.push_back(2);
	screenMesh.indices.push_back(3);
	screenMesh.indices.push_back(0);

	//myvk::Mesh vkMeshScreen = myvk::Mesh();
	//vkMeshScreen.updateBuffers(screenMesh.vertices, screenMesh.indices);

	

	Events::toggle_cursor();
	double lastTime = glfwGetTime();
	double timeAccu = 0.0f;
	const double target_fps = 60.0;
	const scalar H = 1.0f / target_fps;
	const scalar speed = 40.0;
	float camX = 0.0f;
	float camY = 0.0f;

	float angle = 0;
	float scale = 1;

	glm::mat4 model = glm::mat4(1.0f);

	while (!window.isShouldClose()) {
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;
		lastTime = currentTime;

		timeAccu += frameTime;
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
				camera.addZoom(0.01f);
				
			}
			if (Events::pressed(GLFW_KEY_H)) {
				camera.addZoom(-0.01f);
			}

			if (Events::jpressed(GLFW_KEY_TAB)) {
				Events::toggle_cursor();
			}

			/*
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

			global.renderer->vkRenderer.beginFrame();

			/*
			// Render Target //
			renderTarget.beginRenderPass(renderer.frameInfo());

			mainRenderSystem.setProjview(camera.getProjview());
			mainRenderSystem.render(
				&vkMesh,
				&vkDefaultMat, 
				model);

			renderTarget.endRenderPass(renderer.frameInfo());
			*/
			
			// SwapChain Renderer //

			global.renderer->vkRenderer.beginSwapChainRenderPass();

			global.renderer->vkRenderSystem.setProjview(camera.getProjview());
			global.renderer->vkRenderSystem.render(
				&vkMesh,
				&global.renderer->vkDefaultMat, 
				model);

			global.renderer->vkRenderer.endSwapChainRenderPass();
	
			global.renderer->vkRenderer.endFrame();
			
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

/*

void Engine::loadModels() {
	model = std::make_shared<Model>();

	MeshInstance meshInstance;
	meshInstance.vertices.push_back({1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	meshInstance.vertices.push_back({1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	meshInstance.vertices.push_back({-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	meshInstance.vertices.push_back({-1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});

	meshInstance.indices.push_back(0);
	meshInstance.indices.push_back(1);
	meshInstance.indices.push_back(2);
	meshInstance.indices.push_back(2);
	meshInstance.indices.push_back(3);
	meshInstance.indices.push_back(0);

	Texture2D texture("C:/cplusplus/VulkanRender/VulkanRender/resources/img/green.png");

	std::shared_ptr<GPUTexture> gpuTexture = std::make_shared<GPUTexture>(device, &texture);
	model->material = std::make_shared<GPUMaterial>(*globalPool, *materialSetLayout, gpuTexture);
	model->mesh = std::make_shared<GPUMesh>(device, meshInstance);
}

*/
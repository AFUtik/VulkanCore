#include "Engine.hpp"

#include "Fonts.hpp"



//#include "gui/GUIContext.hpp"
//#include "gui/GUIRendering.hpp"
//#include "gui/GuiContext.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "model/Texture.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/GlobalRenderSystem.hpp"

#include "vk/Device.hpp"
#include "vk/Material.hpp"
#include "vk/Mesh.hpp"
#include "vk/RenderTarget.hpp"
#include "vk/Texture.hpp"
#include "window/Events.hpp"
#include "window/Window.hpp"

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

	myvk::Renderer renderer;
	myvk::RenderTarget renderTarget(renderer.getSwapChain(), VkExtent2D{RENDER_WIDTH, RENDER_HEIGHT});

	//VkDescriptorImageInfo imageInfo;
	//if(model->texture) {
	//	imageInfo.sampler = model->texture->getSampler();
	//	imageInfo.imageView = model->texture->getView();
	//	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//}

	FontHandler fontHandler;
	fontHandler.createSample("Minecraft", absolutePath+"resources/fonts/minecraftRegular.otf");

	//GUIContext guiContext(Rect{window.width, window.height});
	//auto guiWindow = std::make_shared<GUIWindow>();
	//guiWindow->window.width = 700;
	//guiWindow->window.height = 500;
	//guiWindow->pos.x = 200;
	//guiWindow->pos.y = 200;
	//guiWindow->footer.height = 30;
	//guiContext.createWindow(guiWindow);
	//
	//GUIEventListener guiEventListener(&guiContext);

	//UIRenderSystem uiRenderSystem(device, renderer.getSwapChainRenderPass(), renderer.getDescriptorPool(), frameInfo);
	//std::shared_ptr<GUIRenderer> guiRenderer = std::make_shared<GUIRenderer>(&guiContext, &fontHandler);
	//uiRenderSystem.registerRenderer(guiRenderer);
	
	//guiRenderer->fetchContext();

	
	//myvk::GlobalRenderSystem screenRenderSystem(renderer, renderer.getSwapChainRenderPass());
	myvk::GlobalRenderSystem mainRenderSystem  (renderer, renderer.getSwapChainRenderPass());

	renderTarget.createFramebufferTexture(
		renderer.getDescriptorPool(), 
		mainRenderSystem.getMaterialSetLayout()
	);

	std::unique_ptr<uint8_t[]> whitePixel = std::make_unique<uint8_t[]>(4);
	whitePixel[0] = 255;
	whitePixel[1] = 255;
	whitePixel[2] = 255;
	whitePixel[3] = 255;

    Texture2D defaultTex(std::move(whitePixel), 1, 1, TextureChannels::RGBA);
	myvk::Texture  vkDefaultTex(&defaultTex, TextureFilter::Nearest);
	myvk::Material vkDefaultMat(
		*renderer.getDescriptorPool(),
		*mainRenderSystem.getMaterialSetLayout()
	);
	vkDefaultMat.create(&vkDefaultTex);

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

	std::vector<myvk::InstanceData> instances(16);
	for(int i = 0; i < 16; i++) {
		instances[i].model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3(100.0f * i, 0.0f, 0.0f)
		);
	}

	myvk::Mesh vkMesh = myvk::Mesh();
	vkMesh.updateBuffers(mesh.vertices, mesh.indices);
	vkMesh.updateInstanceBuffer(instances);
	
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

	Texture2D texture(absolutePath+"resources/img/tuff.png");
	myvk::Texture vkTex(&texture, TextureFilter::Nearest);
	
	myvk::Material vkMat = myvk::Material(
		*renderer.getDescriptorPool(),
		*mainRenderSystem.getMaterialSetLayout());
	vkMat.create(&vkTex);

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

			renderer.beginFrame();

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

			renderer.beginSwapChainRenderPass();

			mainRenderSystem.setProjview(camera.getProjview());
			mainRenderSystem.render(
				&vkMesh,
				&vkDefaultMat, 
				model);

			renderer.endSwapChainRenderPass();
	
			renderer.endFrame();
			
			timeAccu -= H;
		}
		else {
			double sleepTime = H - timeAccu;
			std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
		}
		Events::pullEvents();
	}

	vkDeviceWaitIdle(myvk::Device::instance().device());
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
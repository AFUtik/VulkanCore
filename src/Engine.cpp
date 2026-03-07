#include "Engine.hpp"
#include "Buffer.hpp"
#include "Descriptors.hpp"
#include "RenderSystem.hpp"
#include "Swapchain.hpp"
#include "UIRenderSystem.hpp"
#include "gui/GuiContext.hpp"
#include "model/GPUMesh.hpp"
#include "model/GPUTexture.hpp"
#include "model/Mesh.hpp"
#include "model/Texture.hpp"
#include "vulkan/vulkan_core.h"
#include "window/Events.hpp"

#include <memory>
#include <string>
#include <thread>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

using namespace myvk;

struct GlobalUbo {
	glm::mat4 projview{1.f};
};

Engine::Engine() : camera(window.width, window.height, glm::dvec3(0, 0, 5), glm::radians(90.0f)) {
	Events::init(window.window);

	globalPool = DescriptorPoolManager::Builder(device)
		.setMaxSets(72)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64)
		.build();

	createGlobalLayouts();
	loadModels();
}

Engine::~Engine() {
	
}

void Engine::createGlobalLayouts() {
	globalUniform.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < globalUniform.size(); i++) {
		globalUniform[i] = std::make_unique<Buffer>(
			device, 
			sizeof(GlobalUbo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
			VMA_MEMORY_USAGE_CPU_TO_GPU);
		globalUniform[i]->map();
	}

	globalSetLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build();
	globalLayouts.push_back(globalSetLayout->getDescriptorSetLayout());

	materialSetLayout = DescriptorSetLayout::Builder(device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	globalLayouts.push_back(materialSetLayout->getDescriptorSetLayout());

	globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for(int i = 0; i < globalDescriptorSets.size(); i++) {
		auto bufferInfo = globalUniform[i]->descriptorInfo();
		DescriptorWriter(*globalSetLayout, *globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(globalDescriptorSets[i]);
	}
}

void Engine::run() {
	//VkDescriptorImageInfo imageInfo;
	//if(model->texture) {
	//	imageInfo.sampler = model->texture->getSampler();
	//	imageInfo.imageView = model->texture->getView();
	//	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//}


	
	GUIContext guiContext(window.width, window.height);
	auto guiWindow = std::make_shared<GUIWindow>();
	guiWindow->windowWidth = 700;
	guiWindow->windowHeight = 500;
	guiWindow->posX = 200;
	guiWindow->posY = 200;
	
	guiContext.createWindow(guiWindow);

	UIRenderSystem uiRenderSystem(device, renderer.getSwapChainRenderPass(), globalPool.get(), &guiContext, &camera);

	RenderSystem renderSystem(device, renderer.getSwapChainRenderPass(), globalLayouts);
	renderSystem.addModel(model);

	Events::toggle_cursor(&window);
	double lastTime = glfwGetTime();
	double timeAccu = 0.0f;
	const double target_fps = 60.0;
	const double H = 1.0f / target_fps;
	const double speed = 2.0;
	float camX = 0.0f;
	float camY = 0.0f;
	bool writingActive = false;
	while (!window.isShouldClose()) {
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;
		lastTime = currentTime;

		timeAccu += frameTime;
		if (timeAccu >= H) {
			if (Events::pressed(GLFW_KEY_W)) {
				camera.translate(camera.zdir() * H * speed);
			}
			if (Events::pressed(GLFW_KEY_S)) {
				camera.translate(-camera.zdir() * H * speed);
			}
			if (Events::pressed(GLFW_KEY_D)) {
				camera.translate(camera.xdir() * H * speed);
			}
			if (Events::pressed(GLFW_KEY_A)) {
				camera.translate(-camera.xdir() * H * speed);
			}
			if (Events::pressed(GLFW_KEY_TAB)) {
				writingActive=!writingActive;
			}

			if (Events::_cursor_locked) {
				camY += -Events::deltaY / window.height * 2;
				camX += -Events::deltaX / window.height * 2;

				if (camY < -radians(89.0f)) {
					camY = -radians(89.0f);
				}
				if (camY > radians(89.0f)) {
					camY = radians(89.0f);
				}

				camera.setRotation(glm::mat4(1.0f));
				camera.rotate(-camY, camX, 0);
			}

			if (auto commandBuffer = renderer.beginFrame()) {
				renderer.beginSwapChainRenderPass(commandBuffer);

				int frameIndex = renderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					(float)frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex].set};

				camera.update();

				GlobalUbo ubo{};
				ubo.projview = camera.getProjviewProspective();
				globalUniform[frameIndex]->writeToBuffer(&ubo);
				globalUniform[frameIndex]->flush();

				renderSystem.render(frameInfo);
				uiRenderSystem.render(frameInfo);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
			timeAccu -= H;
		}
		else {
			double sleepTime = H - timeAccu;
			std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
		}
		Events::pullEvents();
	}
	vkDeviceWaitIdle(device.device());
}

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
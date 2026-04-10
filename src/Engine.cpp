#include "Engine.hpp"

#include "Fonts.hpp"

#include "rendering/GlobalRenderSystem.hpp"

//#include "gui/GUIContext.hpp"
//#include "gui/GUIRendering.hpp"
//#include "gui/GuiContext.hpp"
#include "rendering/RenderSystem.hpp"
#include "vk/Mesh.hpp"
#include "vk/Texture.hpp"
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
#include <iostream>

Engine::Engine() : camera(window.width, window.height, glm::dvec3(0, 0, 5), glm::radians(90.0f)), frameInfo(0, 0.0f, VK_NULL_HANDLE) {
	Events::init(window.window);
}

Engine::~Engine() {
	
}

void Engine::run() {
	myvk::Renderer renderer(window, device);

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

	myvk::GlobalRenderSystem renderSystem(device, renderer.getSwapChainRenderPass(), renderer.getDescriptorPool(), frameInfo);

	Mesh mesh;
	mesh.vertices.push_back({1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	mesh.vertices.push_back({1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	mesh.vertices.push_back({-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	mesh.vertices.push_back({-1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f});

	mesh.indices.push_back(0);
	mesh.indices.push_back(1);
	mesh.indices.push_back(2);
	mesh.indices.push_back(2);
	mesh.indices.push_back(3);
	mesh.indices.push_back(0);

	myvk::Mesh vkMesh = myvk::Mesh(device);
	vkMesh.update(mesh.vertices, mesh.indices);
	
	Texture2D texture(absolutePath+"resources/img/tuff.png");
	myvk::Material vkMat = myvk::Material(
		*renderer.getDescriptorPool(), 
		*renderSystem.getMaterialSetLayout(), 
		std::make_shared<myvk::Texture>(device, &texture)
	);

	Events::toggle_cursor(&window);
	double lastTime = glfwGetTime();
	double timeAccu = 0.0f;
	const double target_fps = 60.0;
	const double H = 1.0f / target_fps;
	const double speed = 2.0;
	float camX = 0.0f;
	float camY = 0.0f;
	while (!window.isShouldClose()) {
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;
		lastTime = currentTime;

		timeAccu += frameTime;
		if (timeAccu >= H) {
			//guiEventListener.listen();
			//guiRenderer->syncAll();

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
			if (Events::jpressed(GLFW_KEY_TAB)) {
				Events::toggle_cursor(&window);
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
				frameInfo.frameIndex = renderer.getFrameIndex();
				frameInfo.commandBuffer = commandBuffer;

				camera.update();

				renderSystem.render(
					&vkMesh, 
					&vkMat, 
					glm::mat4(1.0f), 
					&camera);

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
	std::cout << "End Function" << std::endl;
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
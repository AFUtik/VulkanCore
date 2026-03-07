#pragma once

#include "glm/gtc/type_precision.hpp"
#include "vulkan/vulkan_core.h"
#include "window/Events.hpp"
#include "window/Window.hpp"
#include "Renderer.hpp"

#include "Camera.hpp"
#include "Descriptors.hpp"
#include "Buffer.hpp"
#include "FrameInfo.hpp"

#include "model/Model.hpp"

#include <memory>
#include <vector>

#include "Fonts.hpp"

namespace myvk {
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
		void createGlobalLayouts();
		void loadModels();

		Window window{ WIDTH, HEIGHT, "Vulkan Engine" };
		Device device{ window };
		Renderer renderer{ window, device };

		Camera camera;

		std::unique_ptr<DescriptorPoolManager> globalPool{};

		std::unique_ptr<DescriptorSetLayout> globalSetLayout;
		std::unique_ptr<DescriptorSetLayout> materialSetLayout;
		std::vector<DescriptorSetData> globalDescriptorSets;
		std::vector<VkDescriptorSetLayout> globalLayouts;
		std::vector<std::unique_ptr<Buffer>> globalUniform;
		std::shared_ptr<Model> model;
	};
}
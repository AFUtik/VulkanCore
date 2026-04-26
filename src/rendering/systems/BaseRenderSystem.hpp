#pragma once

#include "../../vk/RenderSystem.hpp"

struct RenderState;

namespace myvk
{

class Renderer;
class RenderTarget;

struct Ubo {glm::mat4 projview{1.f};};

class BaseRenderSystem : public RenderSystem
{
public: 
    BaseRenderSystem(Renderer& renderer);
    BaseRenderSystem(Renderer& renderer, RenderTarget& target);

	Material* getDefaultMaterial();

    inline DescriptorSetLayout* getMaterialSetLayout() override {return materialSetLayout.get();}
    void render(RenderState& state, Mesh* mesh, Material* mat);
private:
    void createLayouts();
    void createDefaultMaterial();

    Ubo ubo;

    std::unique_ptr<DescriptorSetLayout> setLayout;
	std::unique_ptr<DescriptorSetLayout> materialSetLayout;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSetLayout> layouts;

	std::vector<std::unique_ptr<Buffer>> uniforms;

	std::unique_ptr<Material> defaultMaterial;
};

}
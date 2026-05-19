#pragma once


#include "vk/Descriptors.hpp"
#include "vk/RenderSystem.hpp"
#include "vk/Mesh.hpp"

#include <glm/glm.hpp>

struct RenderState;
struct RenderBatch;

struct VkDescriptorSet_T; 
using VkDescriptorSet = VkDescriptorSet_T*;

namespace myvk
{

class Buffer;

class Renderer;
class RenderTarget;

class Material;
class Mesh;

struct Shader;

struct PipelineConfigInfo;

struct GlobalUniformBuffer {glm::mat4 projview{1.f};};

class BaseRenderSystem : public RenderSystem
{
public: 
    BaseRenderSystem(
        Renderer& renderer, 
        PipelineConfigInfo& config);

    BaseRenderSystem(
        Renderer& renderer, 
        RenderTarget& target, 
        PipelineConfigInfo& config);

    ~BaseRenderSystem();

	Material* getDefaultMaterial();

    inline DescriptorSetLayout* getMaterialSetLayout() override {return materialSetLayout.get();}

    void render(RenderState& state, RenderBatch& batch);
    void clearInstances();
    
    static std::vector<VkVertexInputBindingDescription>   getBindingDescriptions();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
protected:
    void createLayouts();
    void createDefaultMaterial();
    
    GlobalUniformBuffer ubo;
    
    std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSetLayout> layouts;

    std::unique_ptr<DescriptorSetLayout> setLayout;
	std::unique_ptr<DescriptorSetLayout> materialSetLayout;

	std::vector<std::unique_ptr<Buffer>> globalUniforms;

    std::vector<std::unique_ptr<Buffer>> localInstanceSsbo;
    std::vector<std::unique_ptr<Buffer>> stagingInstanceSsbo;

	std::unique_ptr<Material> defaultMaterial;

    Shader* shader;
    
    size_t instanceOffset = 0;
    size_t instnaceOffsetBytes = 0;

    
};

}
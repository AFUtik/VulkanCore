#pragma once


#include "vk/Descriptors.hpp"
#include "vk/RenderSystem.hpp"
#include "rendering/RenderQueue.hpp"

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
    BaseRenderSystem(Renderer& renderer);

    ~BaseRenderSystem();

	Material* getDefaultMaterial();

    inline RenderQueue& getRenderQueue() {return renderQueue;}

    inline DescriptorSetLayout* getMaterialSetLayout() override {return materialSetLayout.get();}

    void flushRenderQueue(RenderState& state);
    inline void clearQueue() noexcept
    {
        renderQueue.batchQueue.clear();
        instanceOffset      = 0;
        instnaceOffsetBytes = 0;
    }

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

    RenderQueue renderQueue;
};

using BSR = BaseRenderSystem;

}
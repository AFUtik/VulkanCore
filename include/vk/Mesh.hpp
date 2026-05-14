#pragma once

#include "../model/Vertex.hpp"

#include <span>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace myvk {

class Buffer;

enum MeshFlags {
	CPUMemory               = 1 << 0,
	GPUMemory               = 1 << 1,
};

enum RenderTopologyFlags {
	Solid     = 1 << 2,
	Wireframe = 1 << 3,
	Line      = 1 << 4
};

enum RenderQueueFlags {
	Opaque      = 1 << 5,
	Cutout      = 1 << 6,
	Transparent = 1 << 7
};

struct alignas(16) InstanceData {
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

class Mesh {
private:
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	uint32_t reservedVertexBufferSize   = 0;
	uint32_t reservedIndexBufferSize    = 0;
	uint32_t reservedInstanceBufferSize = 0;
	uint32_t vertexCount   = 0;
	uint32_t indexCount    = 0;
	uint32_t flags = GPUMemory | Solid | Opaque;
public:
	Mesh();
	~Mesh();

	Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

	inline void setFlags(uint32_t flags)   {this->flags |= flags;}
	inline void resetFlags(uint32_t flags) {this->flags  = flags;}
	inline bool checkFlag(uint32_t flag) {return flags & flag;}
	
	void createBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices);
	void updateBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices);

	void draw(VkCommandBuffer commandBuffer, size_t instanceCount = 1, size_t instanceOffset = 0) const;

	static std::vector<VkVertexInputBindingDescription>   getBindingDescriptions();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

}
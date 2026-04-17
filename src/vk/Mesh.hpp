#pragma once

#include "Buffer.hpp"
#include "Device.hpp"
#include "Scalar.hpp"

#include "../model/Vertex.hpp"

#include <span>

namespace myvk {

enum MeshFlags {
	CPUMemory               = 1 << 0,
	GPUMemory               = 1 << 1,
	CPUMemoryInstanceBuffer = 1 << 2,
	GPUMemoryInstanceBuffer = 1 << 3
};

enum RenderTopologyFlags {
	Solid     = 1 << 4,
	Wireframe = 1 << 5,
	Line      = 1 << 6
};

enum RenderQueueFlags {
	Opaque      = 1 << 7,
	Cutout      = 1 << 8,
	Transparent = 1 << 9
};

struct alignas(16) InstanceData {
	Mat4 model = Mat4(1.0f);
};

class Mesh {
private:
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	std::unique_ptr<Buffer> instanceBuffer;
	uint32_t reservedVertexBufferSize   = 0;
	uint32_t reservedIndexBufferSize    = 0;
	uint32_t reservedInstanceBufferSize = 0;
	uint32_t vertexCount   = 0;
	uint32_t indexCount    = 0;
	uint32_t instanceCount = 0;
	uint32_t flags = GPUMemory | CPUMemoryInstanceBuffer | Solid | Opaque;
public:
	Mesh() {};
	~Mesh() {};

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	inline void setFlags(uint32_t flags)   {this->flags |= flags;}
	inline void resetFlags(uint32_t flags) {this->flags  = flags;}
	inline bool checkFlag(uint32_t flag) {return flags & flag;}
	
	void createBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices);
	void updateBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices);

	void createInstanceBuffer(std::span<InstanceData> instances);
	void updateInstanceBuffer(std::span<InstanceData> instances);
	
	void draw(VkCommandBuffer commandBuffer) const;
	void bind(VkCommandBuffer commandBuffer) const;

	static std::vector<VkVertexInputBindingDescription>   getBindingDescriptions();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

}
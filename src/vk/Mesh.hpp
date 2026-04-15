#pragma once

#include "Buffer.hpp"
#include "Device.hpp"
#include "Scalar.hpp"

#include "../model/Vertex.hpp"

#include <management/collections/SparseSet.hpp>
#include <management/ResourceManager.hpp>
#include <span>
#include <memory>

namespace myvk {

enum MeshFlags {
	Reserve                 = 1 << 0, 
	CPUMemory               = 1 << 1,
	GPUMemory               = 1 << 2,
	CPUMemoryInstanceBuffer = 1 << 3,
	GPUMemoryInstanceBuffer = 1 << 4
};

enum RenderTopologyFlags {
	Solid     = 1 << 5,
	Wireframe = 1 << 6,
	Line      = 1 << 7
};

enum RenderQueueFlags {
	Opaque      = 1 << 8,
	Cutout      = 1 << 9,
	Transparent = 1 << 10
};

struct InstanceData {
	Mat4 model = Mat4(1.0f);
};

struct Mesh {	
	Mesh()  {};
	~Mesh() {};

	inline void setFlags(uint32_t flags)   {this->flags |= flags;}
	inline void resetFlags(uint32_t flags) {this->flags  = flags;}

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void createBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices);
	void updateBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices);
	
	void draw(VkCommandBuffer commandBuffer) const;
	void bind(VkCommandBuffer commandBuffer) const;

	static std::vector<VkVertexInputBindingDescription>   getBindingDescriptions();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
private:
	Device& device = Device::instance();
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	std::unique_ptr<Buffer> instanceBuffer;
	uint32_t reservedVertexBufferSize   = 0;
	uint32_t reservedIndexBufferSize    = 0;
	uint32_t reservedInstanceBufferSize = 0;
	uint32_t vertexCount   = 0;
	uint32_t indexCount    = 0;
	uint32_t instanceCount = 1;
	uint32_t flags = Reserve | GPUMemory | CPUMemoryInstanceBuffer | Solid | Opaque;

	SerialSparseSet<InstanceData, uint32_t> instancePool;
};

}
#pragma once

#include <memory>
#include <cstdint>

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

namespace vk {

class Buffer;

enum MeshFlags {
	CPUMemory               = 1 << 0,
	GPUMemory               = 1 << 1,
};

struct Mesh {
	Mesh();
	~Mesh();

	Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    Mesh(const Mesh&) noexcept = delete;
    Mesh& operator=(const Mesh&) noexcept = delete;

	inline uint32_t getVertexCount()   {return vertexCount;}
	inline uint32_t getIndexCount()    {return indexCount;}
	inline uint32_t getInstanceCount() {return instanceCount;}

	inline void setMemoryUsage(MeshFlags flag)             {this->flags = flag;}
	inline void setVertexStride(uint32_t vertexStride)     {this->vertexStride   = vertexStride;} 
	inline void setIndexStride(uint32_t indexStride)       {this->indexStride    = indexStride;}
	inline void setInstanceStride(uint32_t instanceStride) {this->instanceStride = instanceStride;}

	void createVertexBuffer(const void* vertices, uint64_t size);
	void updateVertexBuffer(const void* vertices, uint64_t size);

	void createIndexBuffer(const void* indices, uint64_t size);
	void updateIndexBuffer(const void* indices, uint64_t size);

	void createInstanceBuffer(const void* instances, uint64_t size);
	void updateInstanceBuffer(const void* instances, uint64_t size);

	void draw(VkCommandBuffer commandBuffer, size_t instanceCount = 1, size_t instanceOffset = 0) const;
	
	#ifndef NDEBUG
	void addDebugInfo(const char* info);
	#endif
private:
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	std::unique_ptr<Buffer> instanceBuffer;
	uint32_t vertexCount   = 0;
	uint32_t indexCount    = 0;
	uint32_t instanceCount = 0;

	uint8_t flags = GPUMemory;
	uint8_t vertexStride   = 0;
	uint8_t indexStride    = 0;
	uint8_t instanceStride = 0;
};

}
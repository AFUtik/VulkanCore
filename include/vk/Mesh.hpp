#pragma once

#include <span>
#include <memory>
#include <cstdint>

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

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

class Mesh {
private:
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	uint32_t reservedVertexBufferSize   = 0;
	uint32_t reservedIndexBufferSize    = 0;
	uint32_t vertexStride  = 0;
	uint32_t vertexCount   = 0;
	uint32_t indexCount    = 0;
	uint32_t flags = (uint32_t)GPUMemory | (uint32_t)Solid | (uint32_t)Opaque;
public:
	Mesh();
	~Mesh();

	Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    Mesh(const Mesh&) noexcept = delete;
    Mesh& operator=(const Mesh&) noexcept = delete;

	inline uint32_t getVertexCount() {return vertexCount;}
	inline uint32_t getIndexCount() {return indexCount;}

	inline void setFlags(uint32_t flags)   {this->flags |= flags;}
	inline void resetFlags(uint32_t flags) {this->flags  = flags;}
	inline bool checkFlag(uint32_t flag) {return flags & flag;}
	
	void setVertexStride(uint32_t vertexStride) {this->vertexStride = vertexStride;} // Call before 'createBuffers' or 'updateBuffers' functions.

	void createBuffers(std::span<const std::byte> vertices, std::span<uint32_t> indices = {});
	void updateBuffers(std::span<const std::byte> vertices, std::span<uint32_t> indices = {});

	void draw(VkCommandBuffer commandBuffer, size_t instanceCount = 1, size_t instanceOffset = 0) const;
	
	#ifndef NDEBUG
	void addDebugInfo(const char* info);
	#endif
};

}
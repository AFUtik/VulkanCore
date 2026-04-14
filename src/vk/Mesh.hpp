#pragma once

#include "Buffer.hpp"
#include "Device.hpp"

#include "../model/Vertex.hpp"

namespace myvk {

enum MeshBufferFlags : uint32_t {
	CreateWithReserve,
	CreateOnGPUMemory
};

enum RenderModes {
	Solid,
	Line
};

class Mesh {
private:
	Device& device;
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	bool uploaded = false;
	bool hasIndexBuffer = false;
	uint32_t reservedVertexBufferSize = 0;
	uint32_t reservedIndexBufferSize  = 0;
	uint32_t vertexCount;
	uint32_t indexCount;
	uint32_t flags;
	
	void createBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	void updateBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
public:
	RenderModes mode = RenderModes::Solid;

	Mesh(Device& device, uint32_t flags = (CreateWithReserve | CreateOnGPUMemory));
	~Mesh() {};

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void update(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	
	void draw(VkCommandBuffer commandBuffer) const;
	void bind(VkCommandBuffer commandBuffer) const;

	static std::vector<VkVertexInputBindingDescription>   getBindingDescriptions();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

}
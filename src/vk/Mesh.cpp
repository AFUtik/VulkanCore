#include "vk/Mesh.hpp"
#include "vk/Device.hpp"
#include "vk/Buffer.hpp"

inline uint32_t nextPow2(uint32_t v) {
    if (v == 0) return 1;

    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}

namespace myvk {
	Mesh::Mesh() = default;

	Mesh::~Mesh() = default;

	Mesh::Mesh(Mesh&&) noexcept = default;
	Mesh& Mesh::operator=(Mesh&&) noexcept = default;

    void Mesh::createBuffers(std::span<const std::byte> vertices, std::span<uint32_t> indices) {
		Device& device = Device::instance();

		vertexCount = static_cast<uint32_t>(vertices.size() / vertexStride);
		indexCount  = static_cast<uint32_t>(indices.size());

		reservedVertexBufferSize = nextPow2(vertexCount);
		reservedIndexBufferSize  = nextPow2(indexCount);

		// VertexBuffer creation //
		if(vertexCount >= 3) {
			VkDeviceSize bufferSize = vertexStride * reservedVertexBufferSize;
			VkDeviceSize copySize   = vertexStride * vertexCount;
			if(flags & MeshFlags::GPUMemory) { // GPU MEMORY
				Buffer stagingBuffer(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					VMA_MEMORY_USAGE_CPU_ONLY
				);
				stagingBuffer.map();
				stagingBuffer.writeToBuffer(vertices.data(), copySize);
				stagingBuffer.unmap();
				

				vertexBuffer = std::make_unique<Buffer>(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					0,
					VMA_MEMORY_USAGE_GPU_ONLY
				);
				
				device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
			} 
			else { // CPU MEMORY

				vertexBuffer = std::make_unique<Buffer>(
					device,
					bufferSize,
					1,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					0,
					VMA_MEMORY_USAGE_CPU_TO_GPU
				);
				vertexBuffer->writeToBuffer(vertices.data(), bufferSize);
			}
		}
		
		// IndexBuffer creation //
		if(indexCount) {
			VkDeviceSize bufferSize = sizeof(indices[0]) * reservedIndexBufferSize;
			if(flags & MeshFlags::GPUMemory) { // GPU MEMORY
				Buffer stagingBuffer(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					VMA_MEMORY_USAGE_CPU_ONLY
				);
				stagingBuffer.map();
				stagingBuffer.writeToBuffer(indices.data(), bufferSize);
				stagingBuffer.unmap();

				indexBuffer = std::make_unique<Buffer>(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					0,
					VMA_MEMORY_USAGE_GPU_ONLY
				);
				
				device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
			} else { // CPU MEMORY
				indexBuffer = std::make_unique<Buffer>(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					VMA_MEMORY_USAGE_CPU_TO_GPU
				);
				indexBuffer->writeToBuffer(indices.data(), bufferSize);
			}
		}
	}

	void Mesh::updateBuffers(std::span<const std::byte> vertices, std::span<uint32_t> indices) {
		Device& device = Device::instance();

		vertexCount = static_cast<uint32_t>(vertices.size() / vertexStride);
		indexCount  = static_cast<uint32_t>(indices.size());

		if (vertexCount > reservedVertexBufferSize || indexCount  > reservedIndexBufferSize)
		{
			createBuffers(vertices, indices);
			return;
		}

		if (vertexCount >= 3)
		{
			VkDeviceSize bufferSize = vertexStride * vertexCount;

			if (flags & MeshFlags::GPUMemory)
			{
				// staging buffer
				Buffer stagingBuffer(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					VMA_MEMORY_USAGE_CPU_ONLY
				);

				stagingBuffer.map();
				stagingBuffer.writeToBuffer(vertices.data(), bufferSize);
				stagingBuffer.unmap();

				device.copyBuffer(
					stagingBuffer.getBuffer(),
					vertexBuffer->getBuffer(),
					bufferSize
				);
			}
			else
			{
				vertexBuffer->map();
				vertexBuffer->writeToBuffer(vertices.data(), bufferSize);
			}
		}

		if (indexCount)
		{
			VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

			if (flags & MeshFlags::GPUMemory)
			{
				Buffer stagingBuffer(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					VMA_MEMORY_USAGE_CPU_ONLY
				);

				stagingBuffer.map();
				stagingBuffer.writeToBuffer(indices.data(), bufferSize);
				stagingBuffer.unmap();

				device.copyBuffer(
					stagingBuffer.getBuffer(),
					indexBuffer->getBuffer(),
					bufferSize
				);
			}
			else
			{
				indexBuffer->map();
				indexBuffer->writeToBuffer(indices.data(), bufferSize);
			}
		}
	}

	void Mesh::draw(VkCommandBuffer commandBuffer, size_t instanceCount, size_t instanceOffset) const {
		// Bind Cmd
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (indexBuffer) vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Draw Cmd 
		if (indexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, instanceOffset);
		} else {
			vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, instanceOffset);
		}
	}

	#ifndef NDEBUG
	void Mesh::addDebugInfo(const char* info)
	{
		if(vertexBuffer)
		{
			std::string modified = std::string(info) + "_VertexBuffer";
			vertexBuffer->addDebugInfo(modified.c_str());
		}
		if(indexBuffer)
		{
			std::string modified = std::string(info) + "_IndexBuffer";
			indexBuffer->addDebugInfo(modified.c_str());
		}
	}
	#endif
}
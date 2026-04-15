#include "Mesh.hpp"

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <iostream>

uint32_t nextPow2(uint32_t x) {
    if (x == 0) return 1;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

namespace myvk {
    void Mesh::createBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		indexCount  = static_cast<uint32_t>(indices.size());

		bool recreateVertexBuffer = vertexCount > reservedVertexBufferSize;

		if(recreateVertexBuffer) {
			reservedVertexBufferSize = (flags & MeshFlags::Reserve) ? 
				nextPow2(vertexCount) : 
				(uint32_t)vertices.size();
		}

		bool recreateIndexBuffer  = indexCount > reservedIndexBufferSize;
		if(recreateIndexBuffer) {
			reservedIndexBufferSize = (flags & MeshFlags::Reserve) ? 
				nextPow2(indexCount) : 
				(uint32_t)indices.size();
		}

		// VertexBuffer creation //
		if(vertexCount >= 3) {
			VkDeviceSize bufferSize = sizeof(vertices[0]) * reservedVertexBufferSize;
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
				stagingBuffer.writeToBuffer(vertices.data(), bufferSize);
				stagingBuffer.unmap();
				
				if(recreateVertexBuffer) {
					vertexBuffer = std::make_unique<Buffer>(
						device,
						bufferSize,
						1,
						VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						0,
						VMA_MEMORY_USAGE_GPU_ONLY
					);
				}
				device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
			} 
			else { // CPU MEMORY
				if(recreateVertexBuffer) {
					vertexBuffer = std::make_unique<Buffer>(
						device,
						bufferSize,
						1,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						0,
						VMA_MEMORY_USAGE_CPU_TO_GPU
					);
				}
				vertexBuffer->map();
				vertexBuffer->writeToBuffer(vertices.data(), bufferSize);
				vertexBuffer->unmap();
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

				if(recreateIndexBuffer) {
					indexBuffer = std::make_unique<Buffer>(
						device,
						bufferSize,
						1,
						VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
						0,
						VMA_MEMORY_USAGE_GPU_ONLY
					);
				}
				device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
			} else { // CPU MEMORY
				if(recreateIndexBuffer) {
					indexBuffer = std::make_unique<Buffer>(
						device,
						bufferSize,
						1,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						0,
						VMA_MEMORY_USAGE_CPU_TO_GPU
					);
				}
				indexBuffer->map();
				indexBuffer->writeToBuffer(indices.data(), bufferSize);
				indexBuffer->unmap();
			}
		}
	}

	void Mesh::updateBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		indexCount  = static_cast<uint32_t>(indices.size());

		if (vertexCount > reservedVertexBufferSize || indexCount  > reservedIndexBufferSize)
		{
			createBuffers(vertices, indices);
			return;
		}

		if (vertexCount >= 3)
		{
			VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

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
				vertexBuffer->unmap();
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
				indexBuffer->unmap();
			}
		}
	}
	
	
	void Mesh::bind(VkCommandBuffer commandBuffer) const {
		VkBuffer buffers[] = { 
			vertexBuffer->getBuffer(),
			instanceBuffer->getBuffer()
		};
		VkDeviceSize offsets[] = { 0, 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offsets);
		if (indexBuffer) vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void Mesh::draw(VkCommandBuffer commandBuffer) const {
		if (indexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
		}
	}

	
	std::vector<VkVertexInputBindingDescription> Mesh::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Mesh::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = 0;

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, u);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, r);

		return attributeDescriptions;
	}
}
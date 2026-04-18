#include "Mesh.hpp"
#include "Scalar.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <iostream>

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
	Mesh::Mesh() {
		InstanceData data[1];
		createInstanceBuffer(data);
	}

    void Mesh::createBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices) {
		Device& device = Device::instance();

		vertexCount = static_cast<uint32_t>(vertices.size());
		indexCount = static_cast<uint32_t>(indices.size());

		reservedVertexBufferSize = nextPow2(vertexCount);
		reservedIndexBufferSize  = nextPow2(indexCount);

		// VertexBuffer creation //
		if(vertexCount >= 3) {
			VkDeviceSize bufferSize = sizeof(vertices[0]) * reservedVertexBufferSize;
			VkDeviceSize copySize   = sizeof(vertices[0]) * vertexCount;
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

	void Mesh::updateBuffers(std::span<Vertex> vertices, std::span<uint32_t> indices) {
		Device& device = Device::instance();

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

	void Mesh::createInstanceBuffer(std::span<InstanceData> instances) {
		Device& device = Device::instance();

		instanceCount = instances.size();
		reservedInstanceBufferSize = nextPow2(instanceCount);

		VkDeviceSize bufferSize = sizeof(instances[0]) * reservedInstanceBufferSize;
		if(flags & MeshFlags::GPUMemoryInstanceBuffer) {
				Buffer stagingBuffer(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					VMA_MEMORY_USAGE_CPU_ONLY
				);

				stagingBuffer.map();
				stagingBuffer.writeToBuffer(instances.data(), sizeof(instances[0]) * instanceCount);
				stagingBuffer.unmap();

				device.copyBuffer(
					stagingBuffer.getBuffer(),
					instanceBuffer->getBuffer(),
					bufferSize
				);
		}
		else 
		{
			instanceBuffer = std::make_unique<Buffer>(
					device,
					bufferSize,
					1,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					VMA_MEMORY_USAGE_CPU_TO_GPU
				);

			instanceBuffer->map();
			instanceBuffer->writeToBuffer(instances.data(), sizeof(instances[0]) * instanceCount);
		}
	}

	void Mesh::updateInstanceBuffer(std::span<InstanceData> instances) 
	{
		Device& device = Device::instance();
		instanceCount = instances.size();
		
		VkDeviceSize bufferSize = sizeof(instances[0]) * instanceCount;

		if(instanceCount > reservedInstanceBufferSize) {
			createInstanceBuffer(instances);
			return;
		}
		else if(flags & MeshFlags::GPUMemoryInstanceBuffer)
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
			stagingBuffer.writeToBuffer(instances.data(), bufferSize);
			stagingBuffer.unmap();
				
			device.copyBuffer(
				stagingBuffer.getBuffer(),
				instanceBuffer->getBuffer(),
				bufferSize
			);
		}
		else 
		{
			instanceBuffer->writeToBuffer(instances.data(), bufferSize);
		}
	}

	void Mesh::draw(VkCommandBuffer commandBuffer) const {
		// Bind Cmd
		VkBuffer buffers[] = { vertexBuffer->getBuffer(), instanceBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0, 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offsets);
		if (indexBuffer) vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Draw Cmd 
		if (indexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
		}
	}

	std::vector<VkVertexInputBindingDescription> Mesh::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(2);

		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		// Instance 
		bindingDescriptions[1].binding = 1;
		bindingDescriptions[1].stride = sizeof(InstanceData);
		bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Mesh::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(7);

		size_t location = 0;
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = location;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = 0;
		location++;

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = location;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, u);
		location++;

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = location;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, r);
		location++;

		attributeDescriptions[3].binding = 1;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[3].offset   = 0;

		attributeDescriptions[4].binding = 1;
		attributeDescriptions[4].location = 4;
		attributeDescriptions[4].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[4].offset   = sizeof(glm::vec4);

		attributeDescriptions[5].binding = 1;
		attributeDescriptions[5].location = 5;
		attributeDescriptions[5].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[5].offset   = sizeof(glm::vec4) * 2;

		attributeDescriptions[6].binding = 1;
		attributeDescriptions[6].location = 6;
		attributeDescriptions[6].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[6].offset   = sizeof(glm::vec4) * 3;

		return attributeDescriptions;
	}
}
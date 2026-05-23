#include "gfx/vk/Mesh.hpp"
#include "gfx/vk/Device.hpp"
#include "gfx/vk/Buffer.hpp"

#include <cassert>
#include <iostream>

static inline uint32_t nextPow2(uint32_t v) noexcept {
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

namespace vk {
	Mesh::Mesh() = default;

	Mesh::~Mesh() = default;

	Mesh::Mesh(Mesh&&) noexcept = default;
	Mesh& Mesh::operator=(Mesh&&) noexcept = default;

	void Mesh::createVertexBuffer(const void* vertices, uint64_t size)
	{
		auto& device = Device::instance();
		assert(size >= 3 && "Not enough vertices");

		vertexCount = size;

		VkDeviceSize bufferSize = static_cast<uint64_t>(vertexStride) * nextPow2(vertexCount);
		VkDeviceSize copySize   = static_cast<uint64_t>(vertexStride) * vertexCount;
		if(flags & MeshFlags::GPUMemory) {
			Buffer stagingBuffer(
				device,
				bufferSize,
				1,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VMA_MEMORY_USAGE_CPU_ONLY
			);
			stagingBuffer.map();
			stagingBuffer.writeToBuffer(vertices, copySize);
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
		else {

			vertexBuffer = std::make_unique<Buffer>(
				device,
				bufferSize,
				1,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				0,
				VMA_MEMORY_USAGE_CPU_TO_GPU
			);
			vertexBuffer->writeToBuffer(vertices, bufferSize);
		}
	};

	void Mesh::updateVertexBuffer(const void* vertices, uint64_t size)
	{
		auto& device = Device::instance();
		VkDeviceSize updateSize = static_cast<uint64_t>(vertexStride) * size;
		if(vertexBuffer==nullptr || updateSize > vertexBuffer->getBufferSize()) {
			createVertexBuffer(vertices, size);
			return;
		}
	
		if (flags & MeshFlags::GPUMemory)
		{
			Buffer stagingBuffer(
				device,
				updateSize,
				1,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VMA_MEMORY_USAGE_CPU_ONLY
			);

			stagingBuffer.map();
			stagingBuffer.writeToBuffer(vertices, updateSize);
			stagingBuffer.unmap();

			device.copyBuffer(
				stagingBuffer.getBuffer(),
				vertexBuffer->getBuffer(),
				updateSize
			);
		}
		else
		{
			vertexBuffer->map();
			vertexBuffer->writeToBuffer(vertices, updateSize);
		}
	};

	void Mesh::createIndexBuffer(const void* indices, uint64_t size)
	{
		auto& device = Device::instance();
		indexCount = size;

		VkDeviceSize bufferSize = static_cast<uint64_t>(indexStride) * nextPow2(indexCount);
		VkDeviceSize copySize   = static_cast<uint64_t>(indexStride) * indexCount;
		if(flags & MeshFlags::GPUMemory) {
			Buffer stagingBuffer(
				device,
				bufferSize,
				1,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VMA_MEMORY_USAGE_CPU_ONLY
			);
			stagingBuffer.map();
			stagingBuffer.writeToBuffer(indices, copySize);
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
		} 
		else {
			indexBuffer = std::make_unique<Buffer>(
				device,
				bufferSize,
				1,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				0,
				VMA_MEMORY_USAGE_CPU_TO_GPU
			);
			indexBuffer->writeToBuffer(indices, bufferSize);
		}
	};

	void Mesh::updateIndexBuffer(const void* indices, uint64_t size)
	{
		auto& device = Device::instance();
		VkDeviceSize updateSize = static_cast<uint64_t>(indexStride) * size;
		if(indexBuffer==nullptr || updateSize > indexBuffer->getBufferSize()) {
			createIndexBuffer(indices, size);
			return;
		}
	
		if (flags & MeshFlags::GPUMemory)
		{
			Buffer stagingBuffer(
				device,
				updateSize,
				1,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VMA_MEMORY_USAGE_CPU_ONLY
			);

			stagingBuffer.map();
			stagingBuffer.writeToBuffer(indices, updateSize);
			stagingBuffer.unmap();

			device.copyBuffer(
				stagingBuffer.getBuffer(),
				indexBuffer->getBuffer(),
				updateSize
			);
		}
		else
		{
			indexBuffer->map();
			indexBuffer->writeToBuffer(indices, updateSize);
		}
	};

	void Mesh::createInstanceBuffer(const void* instances, uint64_t size)
	{
		auto& device = Device::instance();
		assert(size >= 1 && "Requires one instance at least");

		instanceCount = size;

		VkDeviceSize bufferSize = static_cast<uint64_t>(instanceStride) * nextPow2(instanceCount);
		VkDeviceSize copySize   = static_cast<uint64_t>(instanceStride) * instanceCount;
		if(flags & MeshFlags::GPUMemory) {
			Buffer stagingBuffer(
				device,
				bufferSize,
				1,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VMA_MEMORY_USAGE_CPU_ONLY
			);
			stagingBuffer.map();
			stagingBuffer.writeToBuffer(instances, copySize);
			stagingBuffer.unmap();
				
			instanceBuffer = std::make_unique<Buffer>(
				device,
				bufferSize,
				1,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				0,
				VMA_MEMORY_USAGE_GPU_ONLY
			);
				
			device.copyBuffer(stagingBuffer.getBuffer(), instanceBuffer->getBuffer(), bufferSize);
		} 
		else {
			instanceBuffer = std::make_unique<Buffer>(
				device,
				bufferSize,
				1,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				0,
				VMA_MEMORY_USAGE_CPU_TO_GPU
			);
			instanceBuffer->writeToBuffer(instances, bufferSize);
		}
	};

	void Mesh::updateInstanceBuffer(const void* instances, uint64_t size)
	{
		auto& device = Device::instance();
		VkDeviceSize updateSize = instanceStride * size;
		if(instanceBuffer==nullptr || updateSize > instanceBuffer->getBufferSize()) {
			createIndexBuffer(instances, size);
			return;
		}
	
		if (flags & MeshFlags::GPUMemory)
		{
			Buffer stagingBuffer(
				device,
				updateSize,
				1,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VMA_MEMORY_USAGE_CPU_ONLY
			);

			stagingBuffer.map();
			stagingBuffer.writeToBuffer(instances, updateSize);
			stagingBuffer.unmap();

			device.copyBuffer(
				stagingBuffer.getBuffer(),
				instanceBuffer->getBuffer(),
				updateSize
			);
		}
		else
		{
			instanceBuffer->map();
			instanceBuffer->writeToBuffer(instances, updateSize);
		}
	};

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
		if(instanceBuffer)
		{
			std::string modified = std::string(info) + "_InstanceBuffer";
			instanceBuffer->addDebugInfo(modified.c_str());
		}
	}
	#endif
}
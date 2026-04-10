#pragma once

#include <vma/vk_mem_alloc.h>

#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <mutex>

namespace myvk {
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;
        //std::mutex mtx;

		DeletionQueue() = default;
		~DeletionQueue() {}

		DeletionQueue(const DeletionQueue&) = delete;
    	DeletionQueue& operator=(const DeletionQueue&) = delete;

		DeletionQueue(DeletionQueue&&) noexcept = default;
		DeletionQueue& operator=(DeletionQueue&&) noexcept = default;


        void push_function(std::function<void()>&& function) {
            deletors.push_back(function);
        }

        void flush() {
            if(!deletors.size()) return;

            // reverse iterate the deletion queue to execute all the functions
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
                (*it)(); //call the function
            }

            deletors.clear();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class Device {
    public:
        #ifdef NDEBUG
                const bool enableValidationLayers = false;
        #else
                const bool enableValidationLayers = true;
        #endif
        
        Device();
        ~Device();

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(Device&&) = delete;

        static Device& instance() {
            static Device static_device;
            return static_device;
        }

        VkCommandPool getCommandPool() { return commandPool; }
        VkDevice device() { return device_; }
        VkSurfaceKHR surface() { return surface_; }
        VkQueue graphicsQueue() { return graphicsQueue_; }
        VkQueue presentQueue() { return presentQueue_; }
        VmaAllocator allocator() { return allocator_; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VmaAllocation& allocation,
            VmaMemoryUsage memoryUsage);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VmaAllocation& allocation);
            
        void createDeletionQueues(uint64_t amount);
            
        void freeDeletionQueue(uint32_t index) {
            deletionQueues[index].flush();
        }

        inline void setFrameIndex(uint32_t index) {
            frame_index = index;
        }

        // Device Allocation //
        template <typename T> void allocate(T* resource);

        VkPhysicalDeviceProperties properties;
    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();
        void createAllocator();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void hasGflwRequiredInstanceExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkInstance instance_;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkCommandPool commandPool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphicsQueue_;
        VkQueue presentQueue_;
        VmaAllocator allocator_;

        std::vector<DeletionQueue> deletionQueues;
        uint32_t frame_index = 0;

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };
}
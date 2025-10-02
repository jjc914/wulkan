#ifndef wulkan_wk_ALLOCATOR_HPP
#define wulkan_wk_ALLOCATOR_HPP

#include "vma_include.hpp"
#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class Allocator {
public:
    Allocator() = default;
    Allocator(const VmaAllocatorCreateInfo& ci) {
        if (vmaCreateAllocator(&ci, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create vma allocator" << std::endl;
        }
    }

    ~Allocator() {
        if (_handle != VK_NULL_HANDLE) {
            vmaDestroyAllocator(_handle);
        }
    }

    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    Allocator(Allocator&& other) noexcept
        : _handle(other._handle)
    {
        other._handle = VK_NULL_HANDLE;
    }

    Allocator& operator=(Allocator&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vmaDestroyAllocator(_handle);
            }
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VmaAllocator& handle() const { return _handle; }
    
private:
    VmaAllocator _handle = VK_NULL_HANDLE;
};

class AllocationCreateInfo {
public:
    AllocationCreateInfo& set_flags(VmaAllocationCreateFlags flags) { _flags = flags; return *this; }
    AllocationCreateInfo& set_usage(VmaMemoryUsage usage) { _usage = usage; return *this; }
    AllocationCreateInfo& set_required_flags(VkMemoryPropertyFlags required_flags) { _required_flags = required_flags; return *this; }
    AllocationCreateInfo& set_preferred_flags(VkMemoryPropertyFlags preferred_flags) { _preferred_flags = preferred_flags; return *this; }
    AllocationCreateInfo& set_memory_type_bits(uint32_t memory_type_bits) { _memory_type_bits = memory_type_bits; return *this; }
    AllocationCreateInfo& set_pool(VmaPool pool) { _pool = pool; return *this; }
    AllocationCreateInfo& set_user_data(void* user_data) { _user_data = user_data; return *this; }
    AllocationCreateInfo& set_priority(float priority) { _priority = priority; return *this; }

    VmaAllocationCreateInfo to_vk() const {
        VmaAllocationCreateInfo ci{};
        ci.flags = _flags;
        ci.usage = _usage;
        ci.requiredFlags = _required_flags;
        ci.preferredFlags = _preferred_flags;
        ci.memoryTypeBits = _memory_type_bits;
        ci.pool = _pool;
        ci.pUserData = _user_data;
        ci.priority = _priority;
        return ci;
    }

private:
    VmaAllocationCreateFlags _flags = 0;
    VmaMemoryUsage _usage = VMA_MEMORY_USAGE_AUTO;
    VkMemoryPropertyFlags _required_flags = 0;
    VkMemoryPropertyFlags _preferred_flags = 0;
    uint32_t _memory_type_bits = 0;
    VmaPool _pool = VK_NULL_HANDLE;
    void* _user_data = nullptr;
    float _priority = 1.0f;
};

class AllocatorCreateInfo {
public:
    AllocatorCreateInfo& set_flags(VmaAllocatorCreateFlags flags) { _flags = flags; return *this; }
    AllocatorCreateInfo& set_vulkan_api_version(uint32_t version) { _vulkan_api_version = version; return *this; }
    AllocatorCreateInfo& set_instance(VkInstance instance) { _instance = instance; return *this; }
    AllocatorCreateInfo& set_physical_device(VkPhysicalDevice physical_device) { _physical_device = physical_device; return *this; }
    AllocatorCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    AllocatorCreateInfo& set_preferred_large_heap_block_size(VkDeviceSize size) { _preferred_large_heap_block_size = size; return *this; }
    AllocatorCreateInfo& set_p_vulkan_functions(VmaVulkanFunctions* p_vulkan_functions) { _p_vulkan_functions = p_vulkan_functions; return *this; }
    AllocatorCreateInfo& set_p_allocation_callbacks(const VkAllocationCallbacks* callbacks) { _p_allocation_callbacks = callbacks; return *this; }
    AllocatorCreateInfo& set_p_device_memory_callbacks(const VmaDeviceMemoryCallbacks* callbacks) { _p_device_memory_callbacks = callbacks; return *this; }
    AllocatorCreateInfo& set_p_heap_size_limits(const VkDeviceSize* heap_size_limits) { _p_heap_size_limits = heap_size_limits; return *this; }
    AllocatorCreateInfo& set_p_type_external_memory_handle_types(const uint32_t* handle_types) { _p_type_external_memory_handle_types = handle_types; return *this; }
    
    VmaAllocatorCreateInfo to_vk() {
        if (_p_vulkan_functions != nullptr) {
            if (_p_vulkan_functions->vkGetInstanceProcAddr == nullptr) {
                _p_vulkan_functions->vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
            }
            if (_p_vulkan_functions->vkGetDeviceProcAddr == nullptr) {
                _p_vulkan_functions->vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
            }
            if (_p_vulkan_functions->vkGetPhysicalDeviceMemoryProperties2KHR == nullptr) {
                _p_vulkan_functions->vkGetPhysicalDeviceMemoryProperties2KHR = 
                    reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(
                        vkGetInstanceProcAddr(_instance, "vkGetPhysicalDeviceMemoryProperties2KHR"));
            }
            if (_p_vulkan_functions->vkGetBufferMemoryRequirements2KHR == nullptr) {
                _p_vulkan_functions->vkGetBufferMemoryRequirements2KHR = 
                    reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(
                        vkGetDeviceProcAddr(_device, "vkGetBufferMemoryRequirements2KHR"));
            }
            if (_p_vulkan_functions->vkGetImageMemoryRequirements2KHR == nullptr) {
                _p_vulkan_functions->vkGetImageMemoryRequirements2KHR = 
                    reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(
                        vkGetDeviceProcAddr(_device, "vkGetImageMemoryRequirements2KHR"));
            }
            if (_p_vulkan_functions->vkBindBufferMemory2KHR == nullptr) {
                _p_vulkan_functions->vkBindBufferMemory2KHR = 
                    reinterpret_cast<PFN_vkBindBufferMemory2KHR>(
                        vkGetDeviceProcAddr(_device, "vkBindBufferMemory2KHR"));
            }
            if (_p_vulkan_functions->vkBindImageMemory2KHR == nullptr) {
                _p_vulkan_functions->vkBindImageMemory2KHR = 
                    reinterpret_cast<PFN_vkBindImageMemory2KHR>(
                        vkGetDeviceProcAddr(_device, "vkBindImageMemory2KHR"));
            }
        }

        VmaAllocatorCreateInfo ci{};
        ci.flags = _flags;
        ci.vulkanApiVersion = _vulkan_api_version;
        ci.instance = _instance;
        ci.physicalDevice = _physical_device;
        ci.device = _device;
        ci.pVulkanFunctions = _p_vulkan_functions;
        ci.pAllocationCallbacks = _p_allocation_callbacks;
        ci.pDeviceMemoryCallbacks = _p_device_memory_callbacks;
        ci.pHeapSizeLimit = _p_heap_size_limits;
        ci.pTypeExternalMemoryHandleTypes = _p_type_external_memory_handle_types;
        ci.preferredLargeHeapBlockSize = _preferred_large_heap_block_size;
        return ci;
    }

private:
    VmaAllocatorCreateFlags _flags = 0;
    uint32_t _vulkan_api_version = VK_API_VERSION_1_2;
    VkInstance _instance = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VmaVulkanFunctions* _p_vulkan_functions = nullptr;
    const VkAllocationCallbacks* _p_allocation_callbacks = nullptr;
    const VmaDeviceMemoryCallbacks* _p_device_memory_callbacks = nullptr;
    const VkDeviceSize* _p_heap_size_limits = nullptr;
    const uint32_t* _p_type_external_memory_handle_types = nullptr;
    VkDeviceSize _preferred_large_heap_block_size = 0;
};

}

#endif

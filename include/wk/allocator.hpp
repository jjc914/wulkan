#ifndef wulkan_wk_ALLOCATOR_HPP
#define wulkan_wk_ALLOCATOR_HPP

#include "vma_include.hpp"
#include "wulkan_internal.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>

namespace wk {

class AllocatorCreateInfo {
public:
    AllocatorCreateInfo& set_version(uint32_t version) { _version = version; return *this; }
    AllocatorCreateInfo& set_instance(VkInstance instance) { _instance = instance; return *this; }
    AllocatorCreateInfo& set_physical_device(VkPhysicalDevice physical_device) { _physical_device = physical_device; return *this; }
    AllocatorCreateInfo& set_device(VkDevice device) { _device = device; return *this; }

    VmaAllocatorCreateInfo to_vk_allocator_create_info() {
        _vulkan_functions = {};
        _vulkan_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        _vulkan_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        _vulkan_functions.vkGetPhysicalDeviceMemoryProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(
            vkGetDeviceProcAddr(_device, "vkGetPhysicalDeviceMemoryProperties2KHR"));
        _vulkan_functions.vkGetBufferMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(
            vkGetDeviceProcAddr(_device, "vkGetBufferMemoryRequirements2KHR"));
        _vulkan_functions.vkGetImageMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(
            vkGetDeviceProcAddr(_device, "vkGetImageMemoryRequirements2KHR"));
        _vulkan_functions.vkBindBufferMemory2KHR = reinterpret_cast<PFN_vkBindBufferMemory2KHR>(
            vkGetDeviceProcAddr(_device, "vkBindBufferMemory2KHR"));
        _vulkan_functions.vkBindImageMemory2KHR = reinterpret_cast<PFN_vkBindImageMemory2KHR>(
            vkGetDeviceProcAddr(_device, "vkBindImageMemory2KHR"));

        VmaAllocatorCreateInfo ci{};
        ci.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        ci.vulkanApiVersion = _version;
        ci.instance = _instance;
        ci.physicalDevice = _physical_device;
        ci.device = _device;
        ci.pVulkanFunctions = &_vulkan_functions;

        return ci;
    }
private:
    uint32_t _version = VK_API_VERSION_1_2;
    VkInstance _instance = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

    VmaVulkanFunctions _vulkan_functions{};
};

class Allocator {
public:
    Allocator(const VmaAllocatorCreateInfo& ci) {
        if (vmaCreateAllocator(&ci, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create VMA allocator" << std::endl;
            std::exit(-1);
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

    VmaAllocator handle() const { return _handle; }

private:
    VmaAllocator _handle = VK_NULL_HANDLE;
};

}

#endif

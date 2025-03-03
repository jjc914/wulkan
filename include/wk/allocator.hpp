#ifndef wulkan_wk_ALLOCATOR_HPP
#define wulkan_wk_ALLOCATOR_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "vma_include.hpp"

namespace wk {

class AllocatorCreateInfo {
public:
    AllocatorCreateInfo& set_version(uint32_t version) { _version = version; return *this; }
    AllocatorCreateInfo& set_instance(VkInstance instance) { _instance = instance; return *this; }
    AllocatorCreateInfo& set_physical_device(VkPhysicalDevice physical_device) { _physical_device = physical_device; return *this; }
    AllocatorCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
private:
    uint32_t _version = 0;
    VkInstance _instance = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

    friend class Allocator;
};

class Allocator {
public:
    Allocator(const AllocatorCreateInfo& ci) {
        PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR = (PFN_vkGetBufferMemoryRequirements2KHR)vkGetDeviceProcAddr(ci._device, "vkGetBufferMemoryRequirements2KHR");
        PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR = (PFN_vkGetImageMemoryRequirements2KHR)vkGetDeviceProcAddr(ci._device, "vkGetImageMemoryRequirements2KHR");
        PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR = (PFN_vkBindBufferMemory2KHR)vkGetDeviceProcAddr(ci._device, "vkBindBufferMemory2KHR");
        PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR = (PFN_vkBindImageMemory2KHR)vkGetDeviceProcAddr(ci._device, "vkBindImageMemory2KHR");

        VmaVulkanFunctions vulkan_functions = {};
        vulkan_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkan_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
        vulkan_functions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
        vulkan_functions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
        vulkan_functions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
        vulkan_functions.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
        vulkan_functions.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
        
        VmaAllocatorCreateInfo vmaci = {};
        vmaci.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        vmaci.vulkanApiVersion = VK_API_VERSION_1_2;
        vmaci.instance = ci._instance;
        vmaci.physicalDevice = ci._physical_device;
        vmaci.device = ci._device;
        vmaci.pVulkanFunctions = &vulkan_functions;
        
        vmaCreateAllocator(&vmaci, &_handle);

        _instance = ci._instance;
        _device = ci._device;
    }

    ~Allocator() {
        vmaDestroyAllocator(_handle);
    }

    VmaAllocator handle() const { return _handle; }
private:
    VmaAllocator _handle;
    VkInstance _instance;
    VkDevice _device;
};

}

#endif
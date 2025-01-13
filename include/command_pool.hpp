#ifndef wulkan_wk_COMMAND_POOL_HPP
#define wulkan_wk_COMMAND_POOL_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "wulkan_internal.hpp"

namespace wk {

class CommandPoolCreateInfo {
public:
    CommandPoolCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    CommandPoolCreateInfo& set_queue_family_indices(QueueFamilyIndices queue_family_indices) { _queue_family_indices = queue_family_indices; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    QueueFamilyIndices _queue_family_indices{};

    friend class CommandPool;
};

class CommandPool {
public:
    CommandPool(const CommandPoolCreateInfo& ci) {
        VkCommandPoolCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        create_info.queueFamilyIndex = ci._queue_family_indices.graphics_family.value();
        
        if (vkCreateCommandPool(ci._device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create command pool" << std::endl;
        }

        _device = ci._device;

        std::clog << "created command pool" << std::endl;
    }

    ~CommandPool() {
        vkDestroyCommandPool(_device, _handle, nullptr);
    }

    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;

    VkCommandPool handle() const { return _handle; }
private:
    VkCommandPool _handle;
    VkDevice _device;
};

}

#endif
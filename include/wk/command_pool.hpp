#ifndef wulkan_wk_COMMAND_POOL_HPP
#define wulkan_wk_COMMAND_POOL_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class CommandPoolCreateInfo {
public:
    CommandPoolCreateInfo& set_queue_family_indices(QueueFamilyIndices queue_family_indices) { 
        _queue_family_indices = queue_family_indices; 
        return *this; 
    }

    VkCommandPoolCreateInfo to_vk_command_pool_create_info() {
        VkCommandPoolCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        ci.queueFamilyIndex = _queue_family_indices.graphics_family.value();
        return ci;
    }
private:
    QueueFamilyIndices _queue_family_indices{};
};

class CommandPool {
public:
    CommandPool(VkDevice device, const VkCommandPoolCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateCommandPool(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool");
        }
    }

    ~CommandPool() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyCommandPool(_device, _handle, nullptr);
        }
    }

    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;

    CommandPool(CommandPool&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    CommandPool& operator=(CommandPool&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyCommandPool(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkCommandPool& handle() const { return _handle; }
private:
    VkCommandPool _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

}

#endif

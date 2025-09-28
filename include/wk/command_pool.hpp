#ifndef wulkan_wk_COMMAND_POOL_HPP
#define wulkan_wk_COMMAND_POOL_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class CommandPoolCreateInfo {
public:
    CommandPoolCreateInfo& set_flags(VkCommandPoolCreateFlags flags) { _flags = flags; return *this; }
    CommandPoolCreateInfo& set_queue_family_index(uint32_t queue_family_index) { _queue_family_index = queue_family_index; return *this; }
    CommandPoolCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }

    VkCommandPoolCreateInfo to_vk() const {
        VkCommandPoolCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.queueFamilyIndex = _queue_family_index;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    uint32_t _queue_family_index = 0;
};
        

class CommandPool {
public:
    CommandPool() noexcept = default;
    CommandPool(VkDevice device, const VkCommandPoolCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateCommandPool(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create command pool" << std::endl;
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

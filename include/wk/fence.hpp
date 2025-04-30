#ifndef wulkan_wk_FENCE_HPP
#define wulkan_wk_FENCE_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class FenceCreateInfo {
public:
    FenceCreateInfo& set_signaled(bool signaled = true) {
        _signaled = signaled;
        return *this;
    }

    VkFenceCreateInfo to_vk_fence_create_info() const {
        VkFenceCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        ci.flags = _signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        return ci;
    }
private:
    bool _signaled = true;
};

class Fence {
public:
    Fence(VkDevice device, const VkFenceCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateFence(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create fence");
        }
    }

    ~Fence() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyFence(_device, _handle, nullptr);
        }
    }

    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;

    Fence(Fence&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    Fence& operator=(Fence&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyFence(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkFence& handle() const { return _handle; }
private:
    VkFence _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

}

#endif

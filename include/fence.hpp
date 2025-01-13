#ifndef wulkan_wk_FENCE_HPP
#define wulkan_wk_FENCE_HPP

#include <cstdint>
#include <iostream>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace wk {

class FenceCreateInfo {
public:
    FenceCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;

    friend class Fence;
};

class Fence {
public:
    Fence(const FenceCreateInfo& ci) {
        VkFenceCreateInfo vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        vkci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        
        if (vkCreateFence(ci._device, &vkci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create fence" << std::endl;;
        }

        _device = ci._device;

        std::clog << "created fence" << std::endl;
    }

    ~Fence() {
        vkDestroyFence(_device, _handle, nullptr);
    }

    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;

    void Await() const {
        vkWaitForFences(_device, 1, &_handle, VK_TRUE, UINT64_MAX);
    }

    void Reset() {
        vkResetFences(_device, 1, &_handle);
    }

    VkFence handle() const { return _handle; }
private:
    VkFence _handle;
    VkDevice _device;
};

}

#endif
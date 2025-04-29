#ifndef wulkan_wk_SEMAPHORE_HPP
#define wulkan_wk_SEMAPHORE_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class SemaphoreCreateInfo {
public:
    VkSemaphoreCreateInfo to_vk_semaphore_create_info() {
        VkSemaphoreCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        return ci;
    }
};

class Semaphore {
public:
    Semaphore(VkDevice device, const VkSemaphoreCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateSemaphore(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphore");
        }
    }

    ~Semaphore() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroySemaphore(_device, _handle, nullptr);
        }
    }

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    Semaphore(Semaphore&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    Semaphore& operator=(Semaphore&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroySemaphore(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    VkSemaphore handle() const { return _handle; }
private:
    VkSemaphore _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

}

#endif

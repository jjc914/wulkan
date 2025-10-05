#ifndef wulkan_wk_SEMAPHORE_HPP
#define wulkan_wk_SEMAPHORE_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class Semaphore {
public:
    Semaphore() = default;
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
        _handle = VK_NULL_HANDLE;
        _device = VK_NULL_HANDLE;
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

    const VkSemaphore& handle() const { return _handle; }
private:
    VkSemaphore _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

class SemaphoreCreateInfo {
public:
    SemaphoreCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    SemaphoreCreateInfo& set_flags(VkSemaphoreCreateFlags flags) { _flags = flags; return *this; }

    VkSemaphoreCreateInfo to_vk() const {
        VkSemaphoreCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkSemaphoreCreateFlags _flags = 0;
};

}

#endif

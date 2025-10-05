#ifndef wulkan_WK_EXT_RT_DEFERRED_OPERATION_HPP
#define wulkan_WK_EXT_RT_DEFERRED_OPERATION_HPP

#include "wk/ext/rt/rt_internal.hpp"

#include <cstdint>
#include <iostream>

namespace wk::ext::rt {

class DeferredOperation {
public:
    DeferredOperation() = default;

    DeferredOperation(VkDevice device, const DeviceFunctions& f)
        : _device(device),
          _vkCreateDeferredOperationKHR(f.vkCreateDeferredOperationKHR),
          _vkDestroyDeferredOperationKHR(f.vkDestroyDeferredOperationKHR)
    {
        if (_vkCreateDeferredOperationKHR == VK_NULL_HANDLE) {
            throw std::runtime_error("device function vkCreateDeferredOperationKHR not set");
        } else if (_vkCreateDeferredOperationKHR(_device, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create deferred operation");
        }
    }

    ~DeferredOperation() {
        if (_handle != VK_NULL_HANDLE) {
            _vkDestroyDeferredOperationKHR(_device, _handle, nullptr);
        }
        _handle = VK_NULL_HANDLE;
        _device = VK_NULL_HANDLE;
        _vkCreateDeferredOperationKHR = VK_NULL_HANDLE;
        _vkDestroyDeferredOperationKHR = VK_NULL_HANDLE;
    }

    // Non-copyable
    DeferredOperation(const DeferredOperation&) = delete;
    DeferredOperation& operator=(const DeferredOperation&) = delete;

    DeferredOperation(DeferredOperation&& other) noexcept
        : _device(other._device),
          _handle(other._handle),
          _vkCreateDeferredOperationKHR(other._vkCreateDeferredOperationKHR),
          _vkDestroyDeferredOperationKHR(other._vkDestroyDeferredOperationKHR) 
    {
        other._device = VK_NULL_HANDLE;
        other._handle = VK_NULL_HANDLE;
        other._vkCreateDeferredOperationKHR = VK_NULL_HANDLE;
        other._vkDestroyDeferredOperationKHR = VK_NULL_HANDLE;
    }

    DeferredOperation& operator=(DeferredOperation&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                _vkDestroyDeferredOperationKHR(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            _vkCreateDeferredOperationKHR = other._vkCreateDeferredOperationKHR;
            _vkDestroyDeferredOperationKHR = other._vkDestroyDeferredOperationKHR;

            other._device = VK_NULL_HANDLE;
            other._handle = VK_NULL_HANDLE;
            other._vkCreateDeferredOperationKHR = VK_NULL_HANDLE;
            other._vkDestroyDeferredOperationKHR = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkDeferredOperationKHR& handle() const { return _handle; }

private:
    VkDeferredOperationKHR _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    PFN_vkCreateDeferredOperationKHR _vkCreateDeferredOperationKHR = VK_NULL_HANDLE;
    PFN_vkDestroyDeferredOperationKHR _vkDestroyDeferredOperationKHR = VK_NULL_HANDLE;
};

} // namespace wk::ext::rt

#endif // wulkan_WK_EXT_RT_DEFERRED_OPERATION_HPP

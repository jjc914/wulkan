#ifndef wulkan_wk_VALIDATION_CACHE_HPP
#define wulkan_wk_VALIDATION_CACHE_HPP

#include "wulkan_internal.hpp"

namespace wk {

class ValidationCache {
public:
    ValidationCache() = default;
    ValidationCache(VkDevice device, const VkValidationCacheCreateInfoEXT& ci) 
        : _device(device) 
    {
        if (vkCreateValidationCacheEXT(device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create validation cache");
        }
    }

    ~ValidationCache() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyValidationCacheEXT(_device, _handle, nullptr);
            _handle = VK_NULL_HANDLE;
            _device = VK_NULL_HANDLE;
        }
    }

    ValidationCache(const ValidationCache&) = delete;
    ValidationCache& operator=(const ValidationCache&) = delete;

    ValidationCache(ValidationCache&& other) noexcept : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    ValidationCache& operator=(ValidationCache&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyValidationCacheEXT(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkValidationCacheEXT& handle() const { return _handle; }

private:
    VkValidationCacheEXT _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

class ValidationCacheCreateInfo {
public:
    ValidationCacheCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    ValidationCacheCreateInfo& set_flags(VkValidationCacheCreateFlagsEXT flags) { _flags = flags; return *this; }
    ValidationCacheCreateInfo& set_initial_data_size(size_t size) { _initial_data_size = size; return *this; }
    ValidationCacheCreateInfo& set_p_initial_data(const void* data) { _p_initial_data = data; return *this; }

    VkValidationCacheCreateInfoEXT to_vk() const {
        VkValidationCacheCreateInfoEXT ci{};
        ci.sType = VK_STRUCTURE_TYPE_VALIDATION_CACHE_CREATE_INFO_EXT;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.initialDataSize = _initial_data_size;
        ci.pInitialData = _p_initial_data;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkValidationCacheCreateFlagsEXT _flags = 0;
    size_t _initial_data_size = 0;
    const void* _p_initial_data = nullptr;
};

} // namespace wk

#endif

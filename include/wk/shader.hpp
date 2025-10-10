#ifndef wulkan_wk_SHADER_HPP
#define wulkan_wk_SHADER_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class ShaderModule {
public:
    ShaderModule() = default;
    ShaderModule(VkDevice device, const VkShaderModuleCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateShaderModule(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
            _handle = VK_NULL_HANDLE;
            _device = VK_NULL_HANDLE;
        }
    }

    ~ShaderModule() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyShaderModule(_device, _handle, nullptr);
        }
        _handle = VK_NULL_HANDLE;
        _device = VK_NULL_HANDLE;
    }

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;

    ShaderModule(ShaderModule&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    ShaderModule& operator=(ShaderModule&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyShaderModule(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkShaderModule& handle() const { return _handle; }
private:
    VkShaderModule _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

class ShaderModuleCreateInfo {
public:
    ShaderModuleCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    ShaderModuleCreateInfo& set_flags(VkShaderModuleCreateFlags flags) { _flags = flags; return *this; }
    ShaderModuleCreateInfo& set_byte_code(size_t size, const void* p_code) { 
        _code_size = size;
        _p_code = p_code;
        return *this;
    }

    VkShaderModuleCreateInfo to_vk() const {
        VkShaderModuleCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.codeSize = _code_size;
        ci.pCode = reinterpret_cast<const uint32_t*>(_p_code);
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkShaderModuleCreateFlags _flags = 0;
    size_t _code_size = 0;
    const void* _p_code = nullptr;
};

}

#endif

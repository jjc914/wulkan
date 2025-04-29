#ifndef wulkan_wk_SHADER_HPP
#define wulkan_wk_SHADER_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class ShaderCreateInfo {
public:
    ShaderCreateInfo& set_byte_code(std::vector<uint8_t> byte_code) {
        _byte_code = byte_code;
        return *this;
    }

    VkShaderModuleCreateInfo to_vk_shader_module_create_info() {
        _vkci = {};
        _vkci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        _vkci.codeSize = _byte_code.size();
        _vkci.pCode = reinterpret_cast<const uint32_t*>(_byte_code.data());
        return _vkci;
    }
private:
    std::vector<uint8_t> _byte_code{};
    VkShaderModuleCreateInfo _vkci{};
};

class Shader {
public:
    Shader(VkDevice device, const VkShaderModuleCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateShaderModule(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        }
    }

    ~Shader() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyShaderModule(_device, _handle, nullptr);
        }
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    Shader& operator=(Shader&& other) noexcept {
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

    VkShaderModule handle() const { return _handle; }
private:
    VkShaderModule _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

}

#endif

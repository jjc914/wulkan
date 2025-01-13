#ifndef wulkan_wk_SHADER_HPP
#define wulkan_wk_SHADER_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace wk {

class ShaderCreateInfo {
public:
    ShaderCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    ShaderCreateInfo& set_byte_code(std::vector<uint8_t> byte_code) { _byte_code = byte_code; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    std::vector<uint8_t> _byte_code{};

    friend class Shader;
};

class Shader {
public:
    Shader(const ShaderCreateInfo& ci) {
        VkShaderModuleCreateInfo vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vkci.flags = 0;
        vkci.codeSize = ci._byte_code.size();
        vkci.pCode = reinterpret_cast<const uint32_t*>(ci._byte_code.data());
 
        if (vkCreateShaderModule(ci._device, &vkci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create shader module" << std::endl;
        }

        _device = ci._device;

        std::cerr << "created shader module" << std::endl;
    }

    ~Shader() {
        vkDestroyShaderModule(_device, _handle, nullptr);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    VkShaderModule handle() const { return _handle; }
private:
    VkShaderModule _handle;
    VkDevice _device;
};

}

#endif
#ifndef wulkan_vk_VKCORE_HPP
#define wulkan_vk_VKCORE_HPP

#include <cstdint>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace vk {

bool IsValidationLayersSupported() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layer_properties(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layer_properties.data());
    
    for (const auto& required_layer : _VALIDATION_LAYERS) {
        bool is_supported = false;
        for (const auto& available_layer_property : available_layer_properties) {
            if (strcmp(required_layer, available_layer_property.layerName) == 0) {
                is_supported = true;
                break;
            }
        }
        if (!is_supported) {
            return false;
        }
    }
    return true;
}

}

#endif
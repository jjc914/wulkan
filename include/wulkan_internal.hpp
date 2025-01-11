#ifndef wulkan_vk_WULKANINTERNAL_HPP
#define wulkan_vk_WULKANINTERNAL_HPP

#include <cstdint>
#include <iostream>
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
    
    for (const auto& available_layer_property : available_layer_properties) {
        if (strcmp("VK_LAYER_KHRONOS_validation", available_layer_property.layerName) == 0) {
            return true;
        }
    }
    return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DefaultDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cerr << pCallbackData->pMessage << std::endl;
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::clog << pCallbackData->pMessage << std::endl;
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        std::clog << pCallbackData->pMessage << std::endl;
    } else {
        std::clog << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() const {
        return graphics_family.has_value() && present_family.has_value();
    }
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices{};
        
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    
    for (uint32_t i = 0; i < queue_families.size(); ++i) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }
        
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
        }
        
        if (indices.is_complete()) {
            break;
        }
    }
    return indices;
}

bool IsPhysicalDeviceExtensionSupported(VkPhysicalDevice device, const std::vector<const char*>& required_extensions) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
    
    for(const auto& required_extension : required_extensions) {
        bool is_supported = false;
        for(const auto& available_extension : available_extensions) {
            if(strcmp(required_extension, available_extension.extensionName) == 0) {
                is_supported = true;
                break;
            }
        }
        if(!is_supported) {
            return false;
        }
    }
    return true;
}

struct SurfaceSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

SurfaceSupportDetails QuerySurfaceSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SurfaceSupportDetails details;
        
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
    if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }
    
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    if (present_mode_count != 0) {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
    }
    
    return details;
}

bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);
    bool is_extensions_supported = IsPhysicalDeviceExtensionSupported(device, required_extensions);
    
    bool is_swapchain_adequate = false;
    if (is_extensions_supported) {
        SurfaceSupportDetails swapchain_support = QuerySurfaceSupport(device, surface);
        is_swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty();
    }

    return indices.is_complete() && is_extensions_supported && is_swapchain_adequate;
}

int32_t RatePhysicalDevice(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(device, &device_features);
    
    int32_t score = 0;
    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    } else if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU){
        score += 500;
    } else {
        return 0;
    }
    if (!IsPhysicalDeviceSuitable(device, required_extensions, surface)) {
        return 0;
    }
    return score;
}

}

#endif
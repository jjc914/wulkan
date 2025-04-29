#include "../include/wk/wulkan_internal.hpp"

namespace wk {

std::vector<const char*> GetDefaultRequiredDeviceExtensions() {
    std::vector<const char*> device_extensions;
#ifdef __APPLE__
    device_extensions.emplace_back("VK_KHR_portability_subset");
#endif
    device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    return device_extensions;
}

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

VKAPI_ATTR VkBool32 VKAPI_CALL DefaultDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                             void* pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        throw std::runtime_error(pCallbackData->pMessage);
    } else {
        std::clog << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

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

    for (const auto& required_extension : required_extensions) {
        bool is_supported = false;
        for (const auto& available_extension : available_extensions) {
            if (strcmp(required_extension, available_extension.extensionName) == 0) {
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

PhysicalDeviceSurfaceSupportDetails QueryPhysicalDeviceSurfaceSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    PhysicalDeviceSurfaceSupportDetails details;

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
        PhysicalDeviceSurfaceSupportDetails swapchain_support = QueryPhysicalDeviceSurfaceSupport(device, surface);
        is_swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty();
    }

    return indices.is_complete() && is_extensions_supported && is_swapchain_adequate;
}

int32_t RatePhysicalDevice(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    int32_t score = 0;
    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    } else if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        score += 500;
    } else {
        return 0;
    }
    if (!IsPhysicalDeviceSuitable(device, required_extensions, surface)) {
        return 0;
    }
    return score;
}

VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {
    for (const auto& available_format : available_formats) {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available_format;
        }
    }

    return available_formats[0];
}

VkPresentModeKHR ChooseSurfacePresentationMode(const std::vector<VkPresentModeKHR>& available_present_modes) {
    for (const auto& available_present_mode : available_present_modes) {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSurfaceExtent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    VkExtent2D actual_extent = { width, height };
    actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
}

std::vector<uint8_t> ReadSpirvShader(const char* file_name) {
    std::filesystem::path bin_dir;
#ifdef __APPLE__
    char buf[PATH_MAX];
    uint32_t bufsize = PATH_MAX;
    if (_NSGetExecutablePath(buf, &bufsize) != 0) {
        throw std::runtime_error("failed to get executable path");
    }
    bin_dir = buf;
    bin_dir.remove_filename();
#endif

    std::ifstream file(bin_dir / file_name, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open shader");
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<uint8_t> buffer(file_size);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    file.close();

    return buffer;
}

}
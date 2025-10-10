#include "../include/wk/wulkan_internal.hpp"

#include "../include/wk/queue.hpp"

#include <functional>

namespace wk {

std::vector<const char*> GetRequiredDeviceExtensions() {
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
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << pCallbackData->pMessage << std::endl;
    } else {
        std::clog << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}

DeviceQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
    DeviceQueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (uint32_t i = 0; i < queue_families.size(); ++i) {
        const auto& q = queue_families[i];

        if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphics_family = i;

        if (q.queueFlags & VK_QUEUE_COMPUTE_BIT)
            indices.compute_family = i;

        if (q.queueFlags & VK_QUEUE_TRANSFER_BIT)
            indices.transfer_family = i;

        if (indices.is_complete())
            break;
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

PhysicalDeviceSurfaceSupport GetPhysicalDeviceSurfaceSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    PhysicalDeviceSurfaceSupport support;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
    if (format_count != 0) {
        support.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, support.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    if (present_mode_count != 0) {
        support.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, support.present_modes.data());
    }

    return support;
}

bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface) {
    DeviceQueueFamilyIndices indices = FindQueueFamilies(device);
    bool is_extensions_supported = IsPhysicalDeviceExtensionSupported(device, required_extensions);

    bool is_swapchain_adequate = false;
    if (is_extensions_supported) {
        PhysicalDeviceSurfaceSupport swapchain_support = GetPhysicalDeviceSurfaceSupport(device, surface);
        is_swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty();
    }

    return indices.is_complete() && is_extensions_supported && is_swapchain_adequate;
}

int32_t RatePhysicalDevice(VkPhysicalDevice device,
    const std::vector<const char*>& required_exts,
    VkPhysicalDeviceFeatures2* features_chain,
    PhysicalDeviceFeatureScorer scorer)
{
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(device, &props);

    if (!IsPhysicalDeviceExtensionSupported(device, required_exts))
        return 0;

    int32_t score = 0;
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        score += 500;

    vkGetPhysicalDeviceFeatures2(device, features_chain);
    score += (*scorer)(device, features_chain);
    return score;
}

int32_t DefaultPhysicalDeviceFeatureScorer(VkPhysicalDevice device, const VkPhysicalDeviceFeatures2* feats2) {
    int32_t score = 0;

    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(device, &props);

    switch (props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   score += 1000; break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: score +=  500; break;
        default: break;
    }

    VkPhysicalDeviceMemoryProperties mem{};
    vkGetPhysicalDeviceMemoryProperties(device, &mem);
    VkDeviceSize vram = 0;
    for (uint32_t i = 0; i < mem.memoryHeapCount; ++i)
        if (mem.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            vram += mem.memoryHeaps[i].size;

    score += static_cast<int32_t>(vram / (256ull * 1024ull * 1024ull));

    if (!feats2) return score;

    const VkPhysicalDeviceFeatures& f = feats2->features;
    if (f.geometryShader)      score += 100;
    if (f.tessellationShader)  score += 100;
    if (f.sampleRateShading)   score += 100;

    for (const VkBaseOutStructure* p = reinterpret_cast<const VkBaseOutStructure*>(feats2->pNext);
         p;
         p = p->pNext) {
        switch (p->sType) {
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR: {
                auto* rtp = reinterpret_cast<const VkPhysicalDeviceRayTracingPipelineFeaturesKHR*>(p);
                if (rtp->rayTracingPipeline) score += 400;
                break;
            }
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT: {
                auto* ms = reinterpret_cast<const VkPhysicalDeviceMeshShaderFeaturesEXT*>(p);
                if (ms->meshShader) score += 300;
                break;
            }
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES:
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES:
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR:
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
                break;
            default:
                if (p->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) break;
        }
    }
    if (feats2->features.shaderInt64) score += 150;

    return score;
}

VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {
    for (const auto& available_format : available_formats) {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available_format;
        }
    }
    throw std::runtime_error("failed to find recommended surface format");
    return available_formats[0];
}

VkFormat ChooseDepthFormat(VkPhysicalDevice physical_device) {
    static const std::vector<VkFormat> depth_formats = {
        VK_FORMAT_D32_SFLOAT, 
        VK_FORMAT_D32_SFLOAT_S8_UINT, 
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : depth_formats) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported depth format");
    return VK_FORMAT_UNDEFINED;
}

VkPresentModeKHR ChooseSurfacePresentationMode(const std::vector<VkPresentModeKHR>& available_present_modes) {
#if defined(__APPLE__)
    // On macOS (MoltenVK), only FIFO is guaranteed to work.
    return VK_PRESENT_MODE_FIFO_KHR;
#else
    for (const auto& available_present_mode : available_present_modes) {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
#endif
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
    std::ifstream file(file_name, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    if (file_size == 0) {
        return {};
    }
    std::vector<uint8_t> buffer(file_size);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    file.close();

    return buffer;
}

VkImageAspectFlags GetAspectFlags(VkFormat format) {
    switch (format) {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case VK_FORMAT_S8_UINT:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

void ImmediateSubmit(VkDevice device, uint32_t queueFamilyIndex,
        const std::function<void(VkDevice, VkCommandPool, VkCommandBuffer)>& record) {
    VkQueue graphics_queue = wk::Queue(device, queueFamilyIndex).handle();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool pool;
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create transient command pool");
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmd;
    if (vkAllocateCommandBuffers(device, &allocInfo, &cmd) != VK_SUCCESS) {
        vkDestroyCommandPool(device, pool, nullptr);
        throw std::runtime_error("failed to allocate transient command buffer");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    record(device, pool, cmd);

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, pool, 1, &cmd);
    vkDestroyCommandPool(device, pool, nullptr);
}

} // wk
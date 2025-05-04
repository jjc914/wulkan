#ifndef wulkan_wk_WULKAN_INTERNAL_HPP
#define wulkan_wk_WULKAN_INTERNAL_HPP

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <optional>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace wk {

struct DeviceQueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() const {
        return graphics_family.has_value() && present_family.has_value();
    }
    bool is_unique() const {
        return graphics_family.value() != present_family.value();
    }
    std::vector<uint32_t> to_vec() const {
        return { graphics_family.value(), present_family.value() };
    }
};

struct PhysicalDeviceSurfaceSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

std::vector<const char*> GetDefaultRequiredDeviceExtensions();
bool IsValidationLayersSupported();
VKAPI_ATTR VkBool32 VKAPI_CALL DefaultDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                             void* pUserData);
DeviceQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
bool IsPhysicalDeviceExtensionSupported(VkPhysicalDevice device, const std::vector<const char*>& required_extensions);
PhysicalDeviceSurfaceSupport GetPhysicalDeviceSurfaceSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface);
int32_t RatePhysicalDevice(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface);
VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
VkFormat ChooseDepthFormat(VkPhysicalDevice physical_device, const std::vector<VkFormat>& requested_formats);
VkPresentModeKHR ChooseSurfacePresentationMode(const std::vector<VkPresentModeKHR>& available_present_modes);
VkExtent2D ChooseSurfaceExtent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR& capabilities);
std::vector<uint8_t> ReadSpirvShader(const char* file_name);
VkImageAspectFlags GetAspectFlags(VkFormat format);

}

#endif 

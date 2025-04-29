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

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() const {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct PhysicalDeviceSurfaceSupportDetails {
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
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
bool IsPhysicalDeviceExtensionSupported(VkPhysicalDevice device, const std::vector<const char*>& required_extensions);
PhysicalDeviceSurfaceSupportDetails QueryPhysicalDeviceSurfaceSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface);
int32_t RatePhysicalDevice(VkPhysicalDevice device, const std::vector<const char*>& required_extensions, VkSurfaceKHR surface);
VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
VkPresentModeKHR ChooseSurfacePresentationMode(const std::vector<VkPresentModeKHR>& available_present_modes);
VkExtent2D ChooseSurfaceExtent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR& capabilities);
std::vector<uint8_t> ReadSpirvShader(const char* file_name);

}

#endif 

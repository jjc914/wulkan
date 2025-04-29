#ifndef wulkan_wk_PHYSICAL_DEVICE_HPP
#define wulkan_wk_PHYSICAL_DEVICE_HPP

#include "wulkan_internal.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <set>

namespace wk {

class PhysicalDevice {
public:
    PhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& extensions)
        : _extensions(extensions) 
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if (device_count == 0) {
            std::cerr << "failed to find GPUs with Vulkan support" << std::endl;
            exit(-1);
        }
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

        std::multimap<int32_t, VkPhysicalDevice> device_scores;
        for (const auto& device : devices) {
            int32_t score = RatePhysicalDevice(device, extensions, surface);
            device_scores.insert({ score, device });
        }

        if (device_scores.rbegin()->first > 0) {
            _handle = device_scores.rbegin()->second;
        } else {
            std::cerr << "failed to find a suitable GPU" << std::endl;
            exit(-1);
        }

        vkGetPhysicalDeviceProperties(_handle, &_properties);
        vkGetPhysicalDeviceFeatures(_handle, &_features);
        _queue_family_indices = FindQueueFamilies(_handle, surface);
    }

    VkPhysicalDevice handle() const { return _handle; }
    VkPhysicalDeviceFeatures features() const { return _features; }
    QueueFamilyIndices queue_family_indices() const { return _queue_family_indices; }
    const std::vector<const char*>& extensions() const { return _extensions; }
private:
    VkPhysicalDevice _handle = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties _properties{};
    VkPhysicalDeviceFeatures _features{};
    QueueFamilyIndices _queue_family_indices{};

    std::vector<const char*> _extensions{};
};

}

#endif
#ifndef wulkan_vk_DEVICE_HPP
#define wulkan_vk_DEVICE_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include <map>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "wulkan_internal.hpp"

namespace vk {

class DeviceCreateInfo {
public:
    DeviceCreateInfo& set_instance(VkInstance instance) { _instance = instance; return *this; }
    DeviceCreateInfo& set_surface(VkSurfaceKHR surface) { _surface = surface; return *this; }
    DeviceCreateInfo& set_required_extensions(const std::vector<const char*>& required_extensions) { _required_extensions = required_extensions; return *this; }
private:
    VkInstance _instance;
    VkSurfaceKHR _surface;
    std::vector<const char*> _required_extensions;

    friend class Device;
};

class Device {
public:
    Device(const DeviceCreateInfo& ci) {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(ci._instance, &device_count, nullptr);
        if (device_count == 0) {
            std::cerr << "failed to find GPUs with Vulkan support" << std::endl;
        }
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(ci._instance, &device_count, devices.data());

        std::multimap<int32_t, VkPhysicalDevice> device_scores;
        for (const auto& device : devices) {
            int32_t score = RatePhysicalDevice(device, ci._required_extensions, ci._surface);
            device_scores.insert({ score, device });
        }

        if (device_scores.rbegin()->first > 0) {
            _physical_device = device_scores.rbegin()->second;
        } else {
            std::cerr << "failed to find a suitable GPU" << std::endl;
        }

        vkGetPhysicalDeviceProperties(_physical_device, &_physical_device_properties);
        std::clog << "found GPU: " << _physical_device_properties.deviceName << std::endl;

        VkDeviceCreateInfo kvci{};
    }

    ~Device() {

    }

private:
    VkDevice _handle;
    VkPhysicalDevice _physical_device;
    VkPhysicalDeviceProperties _physical_device_properties;
};

}

#endif
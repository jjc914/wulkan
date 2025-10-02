#ifndef wulkan_wk_PHYSICAL_DEVICE_HPP
#define wulkan_wk_PHYSICAL_DEVICE_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <set>

namespace wk {

class PhysicalDevice {
public:
    PhysicalDevice() = default;
    PhysicalDevice(VkInstance instance, VkSurfaceKHR surface, 
            const std::vector<const char*>& extensions, VkPhysicalDeviceFeatures2* feature_chain, 
            PhysicalDeviceFeatureScorer scorer = DefaultPhysicalDeviceFeatureScorer)
        : _extensions(extensions) 
    {
        if (!feature_chain) {
            std::cerr << "must supply feature chain" << std::endl;
            return;
        }

        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if (device_count == 0) {
            std::cerr << "failed to find suitable gpu with vulkan support" << std::endl;
            return;
        }
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

        std::multimap<int32_t, VkPhysicalDevice> device_scores;
        for (const auto& device : devices) {
            int32_t score = RatePhysicalDevice(device, extensions, surface, feature_chain, scorer);
            device_scores.insert({ score, device });
        }

        if (device_scores.rbegin()->first > 0) {
            _handle = device_scores.rbegin()->second;
        } else {
            std::cerr << "failed to find suitable gpu" << std::endl;
            return;
        }

        vkGetPhysicalDeviceProperties(_handle, &_properties);
        vkGetPhysicalDeviceFeatures2(_handle, feature_chain);

        _queue_family_indices = FindQueueFamilies(_handle, surface);
    }

    const VkPhysicalDevice& handle() const { return _handle; }
    const VkPhysicalDeviceFeatures& features() const { return _features; }
    const DeviceQueueFamilyIndices& queue_family_indices() const { return _queue_family_indices; }
    const std::vector<const char*>& extensions() const { return _extensions; }
private:
    VkPhysicalDevice _handle = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties _properties{};
    VkPhysicalDeviceFeatures _features{};
    DeviceQueueFamilyIndices _queue_family_indices{};

    std::vector<const char*> _extensions{};
};

class PhysicalDeviceFeatures2 {
public:
    PhysicalDeviceFeatures2& set_features(const VkPhysicalDeviceFeatures* features) { _features = features; return *this; }
    PhysicalDeviceFeatures2& set_p_next(void* p_next) { _p_next = p_next; return *this; }

    VkPhysicalDeviceFeatures2 to_vk() const {
        VkPhysicalDeviceFeatures2 vk{};
        vk.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        vk.pNext = _p_next;
        if (_features) {
            vk.features = *_features;
        } else {
            vk.features = VkPhysicalDeviceFeatures{};
        }
        return vk;
    }

private:
    void* _p_next   = nullptr;
    const VkPhysicalDeviceFeatures* _features = nullptr;
};

class PhysicalDeviceProperties2 {
public:
    PhysicalDeviceProperties2& set_p_next(void* p_next) {
        _p_next = p_next;
        return *this;
    }

    VkPhysicalDeviceProperties2 to_vk() const {
        VkPhysicalDeviceProperties2 vk{};
        vk.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vk.pNext = _p_next;
        return vk;
    }

private:
    void* _p_next   = nullptr;
};

}

#endif
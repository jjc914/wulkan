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
    PhysicalDevice(VkInstance instance,
                   const std::vector<const char*>& extensions,
                   VkPhysicalDeviceFeatures2* feature_chain,
                   PhysicalDeviceFeatureScorer scorer = DefaultPhysicalDeviceFeatureScorer)
        : _extensions(extensions), _features2(*feature_chain)
    {
        if (!feature_chain)
            throw std::runtime_error("wk::PhysicalDevice: must supply feature chain");

        // Enumerate all available physical devices
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if (device_count == 0)
            throw std::runtime_error("wk::PhysicalDevice: no Vulkan-capable GPUs found");

        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

        // Score and select the best GPU
        std::multimap<int32_t, VkPhysicalDevice> candidates;
        for (auto device : devices) {
            int32_t score = RatePhysicalDevice(device, extensions, feature_chain, scorer);
            candidates.insert({ score, device });
        }

        if (candidates.empty() || candidates.rbegin()->first <= 0)
            throw std::runtime_error("wk::PhysicalDevice: no suitable GPU found");

        _handle = candidates.rbegin()->second;

        // Retrieve static device info
        vkGetPhysicalDeviceProperties(_handle, &_properties);
        vkGetPhysicalDeviceFeatures(_handle, &_features);
        vkGetPhysicalDeviceFeatures2(_handle, &_features2);
        vkGetPhysicalDeviceMemoryProperties(_handle, &_memory_properties);
    }

    const VkPhysicalDevice& handle() const { return _handle; }
    const VkPhysicalDeviceFeatures& features() const { return _features; }
    const VkPhysicalDeviceFeatures2& features2() const { return _features2; }
    const std::vector<const char*>& extensions() const { return _extensions; }
private:
    VkPhysicalDevice _handle = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties _properties{};
    VkPhysicalDeviceFeatures _features{};
    VkPhysicalDeviceFeatures2 _features2{};
    VkPhysicalDeviceMemoryProperties _memory_properties{};

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
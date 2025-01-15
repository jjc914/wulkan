#ifndef wulkan_wk_DEVICE_HPP
#define wulkan_wk_DEVICE_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <set>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "wulkan_internal.hpp"

namespace wk {

class GraphicsQueueSubmitInfo {
public:
    GraphicsQueueSubmitInfo& set_wait_semaphores(std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> wait_semaphores) { _wait_semaphores = wait_semaphores; return *this; }
    GraphicsQueueSubmitInfo& set_command_buffers(std::vector<VkCommandBuffer> command_buffers) { _command_buffers = command_buffers; return *this; }
    GraphicsQueueSubmitInfo& set_signal_semaphores(std::vector<VkSemaphore> signal_semaphores) { _signal_semaphores = signal_semaphores; return *this; }
    GraphicsQueueSubmitInfo& set_fence_to_signal(VkFence fence_to_signal) { _fence_to_signal = fence_to_signal; return *this; }
private:
    std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> _wait_semaphores{};
    std::vector<VkCommandBuffer> _command_buffers{};
    std::vector<VkSemaphore> _signal_semaphores{};
    VkFence _fence_to_signal = VK_NULL_HANDLE;

    friend class Device;
};

class DeviceCreateInfo {
public:
    DeviceCreateInfo& set_instance(VkInstance instance) { _instance = instance; return *this; }
    DeviceCreateInfo& set_surface(VkSurfaceKHR surface) { _surface = surface; return *this; }
    DeviceCreateInfo& set_extensions(const std::vector<const char*>& extensions) { _extensions = extensions; return *this; }
    DeviceCreateInfo& set_layers(const std::vector<const char*>& layers) { _layers = layers; return *this; }
private:
    VkInstance _instance;
    VkSurfaceKHR _surface;
    std::vector<const char*> _extensions;
    std::vector<const char*> _layers;

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
            int32_t score = RatePhysicalDevice(device, ci._extensions, ci._surface);
            device_scores.insert({ score, device });
        }

        if (device_scores.rbegin()->first > 0) {
            _physical_device = device_scores.rbegin()->second;
        } else {
            std::cerr << "failed to find a suitable GPU" << std::endl;
        }

        vkGetPhysicalDeviceProperties(_physical_device, &_properties);
        std::clog << "found GPU: " << _properties.deviceName << std::endl;

        vkGetPhysicalDeviceFeatures(_physical_device, &_features);

        _queue_family_indices = FindQueueFamilies(_physical_device, ci._surface);

        std::set<uint32_t> unique_queue_families = { _queue_family_indices.graphics_family.value(), _queue_family_indices.present_family.value() };
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos(unique_queue_families.size());
        int32_t i = 0;
        for (uint32_t index : unique_queue_families) {
            queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[i].queueFamilyIndex = index;
            queue_create_infos[i].queueCount = 1;
            float queue_priority = 1.0;
            queue_create_infos[i].pQueuePriorities = &queue_priority;
            ++i;
        }
        
        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = queue_create_infos.size();
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.pEnabledFeatures = &_features;
        create_info.enabledExtensionCount = (uint32_t)ci._extensions.size();
        create_info.ppEnabledExtensionNames = ci._extensions.data();
#ifdef VLK_ENABLE_VALIDATION_LAYERS
        create_info.enabledLayerCount = static_cast<uint32_t>(ci._layers.size());
        create_info.ppEnabledLayerNames = ci._layers.data();
#else
        create_info.enabledLayerCount = 0;
#endif
        if (vkCreateDevice(_physical_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create logical device" << std::endl;
        }
        
        vkGetDeviceQueue(_handle, _queue_family_indices.graphics_family.value(), 0, &_graphics_queue);
        vkGetDeviceQueue(_handle, _queue_family_indices.present_family.value(), 0, &_present_queue);
        std::clog << "created logical device" << std::endl;
    }

    ~Device() {
        vkDestroyDevice(_handle, nullptr);
    }

    void AwaitIdle() const {
        vkDeviceWaitIdle(_handle);
    }

    void GraphicsQueueAwaitIdle() const {
        vkQueueWaitIdle(_graphics_queue);
    }

    void GraphicsQueueSubmit(GraphicsQueueSubmitInfo si) {
        std::vector<VkSemaphore> wait_semaphores{};
        for (int i = 0; i < si._wait_semaphores.size(); ++i) {
            wait_semaphores.push_back(si._wait_semaphores[i].first);
        }

        std::vector<VkPipelineStageFlags> wait_stage_flags{};
        for (int i = 0; i < si._wait_semaphores.size(); ++i) {
            wait_stage_flags.push_back(si._wait_semaphores[i].second);
        }

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = si._wait_semaphores.size();
        submit_info.pWaitSemaphores = wait_semaphores.data();
        submit_info.pWaitDstStageMask = wait_stage_flags.data();
        submit_info.commandBufferCount = si._command_buffers.size();
        submit_info.pCommandBuffers = si._command_buffers.data();
        submit_info.signalSemaphoreCount = si._signal_semaphores.size();
        submit_info.pSignalSemaphores = si._signal_semaphores.data();
        
        vkQueueSubmit(_graphics_queue, 1, &submit_info, si._fence_to_signal);
    }

    void PresentQueueSubmit(VkSwapchainKHR swapchain, uint32_t available_image_index, VkSemaphore semaphore_to_wait) {
        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &semaphore_to_wait;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &swapchain;
        present_info.pImageIndices = &available_image_index;

        VkResult result = vkQueuePresentKHR(_present_queue, &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR/* || _vlk_is_resized_framebuffer*/) {
            // _vlk_is_resized_framebuffer = false;
            // _vlk_update_swapchain();
            std::cerr << "resize framebuffer" << std::endl;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            std::cerr << "failed to acquire swap chain image" << std::endl;
        }
    }

    VkDevice handle() const { return _handle; }
    VkPhysicalDevice physical_device() const { return _physical_device; }
    QueueFamilyIndices queue_family_indices() const { return _queue_family_indices; }
    VkQueue graphics_queue() const { return _graphics_queue; }
    VkQueue present_queue() const { return _present_queue; }
private:
    VkDevice _handle;
    VkPhysicalDevice _physical_device;
    VkPhysicalDeviceProperties _properties;
    VkPhysicalDeviceFeatures _features;
    QueueFamilyIndices _queue_family_indices;
    VkQueue _graphics_queue;
    VkQueue _present_queue;
};

}

#endif
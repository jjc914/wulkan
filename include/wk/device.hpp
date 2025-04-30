#ifndef wulkan_wk_DEVICE_HPP
#define wulkan_wk_DEVICE_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <set>

namespace wk {

class GraphicsQueueSubmitInfo {
public:
    GraphicsQueueSubmitInfo& set_wait_semaphores(std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> wait_semaphores) { _wait_semaphores = wait_semaphores; return *this; }
    GraphicsQueueSubmitInfo& set_command_buffers(std::vector<VkCommandBuffer> command_buffers) { _command_buffers = command_buffers; return *this; }
    GraphicsQueueSubmitInfo& set_signal_semaphores(std::vector<VkSemaphore> signal_semaphores) { _signal_semaphores = signal_semaphores; return *this; }

    VkSubmitInfo to_vk_submit_info() {
        _wait_semaphores_vec.clear();
        for (int i = 0; i < _wait_semaphores.size(); ++i) {
            _wait_semaphores_vec.push_back(_wait_semaphores[i].first);
        }

        _wait_stage_flags.clear();
        for (int i = 0; i < _wait_semaphores.size(); ++i) {
            _wait_stage_flags.push_back(_wait_semaphores[i].second);
        }

        VkSubmitInfo si{};
        si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        si.waitSemaphoreCount = static_cast<uint32_t>(_wait_semaphores.size());
        si.pWaitSemaphores = _wait_semaphores_vec.data();
        si.pWaitDstStageMask = _wait_stage_flags.data();
        si.commandBufferCount = static_cast<uint32_t>(_command_buffers.size());
        si.pCommandBuffers = _command_buffers.data();
        si.signalSemaphoreCount = static_cast<uint32_t>(_signal_semaphores.size());
        si.pSignalSemaphores = _signal_semaphores.data();
        return si;
    }
private:
    std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> _wait_semaphores{};
    std::vector<VkCommandBuffer> _command_buffers{};
    std::vector<VkSemaphore> _signal_semaphores{};

    std::vector<VkSemaphore> _wait_semaphores_vec{};
    std::vector<VkPipelineStageFlags> _wait_stage_flags{};
};

class PresentInfo {
public:
    PresentInfo& set_wait_semaphores(const std::vector<VkSemaphore>& semaphores) { _wait_semaphores = semaphores; return *this;}
    PresentInfo& set_swapchains(const std::vector<VkSwapchainKHR>& swapchains) { _swapchains = swapchains; return *this;}
    PresentInfo& set_image_indices(const std::vector<uint32_t>& indices) { _image_indices = indices; return *this;}

    VkPresentInfoKHR to_vk_present_info() {
        VkPresentInfoKHR pi{};
        pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        pi.waitSemaphoreCount = static_cast<uint32_t>(_wait_semaphores.size());
        pi.pWaitSemaphores = _wait_semaphores.data();
        pi.swapchainCount = static_cast<uint32_t>(_swapchains.size());
        pi.pSwapchains = _swapchains.data();
        pi.pImageIndices = _image_indices.data();
        pi.pResults = nullptr;
        return pi;
    }
private:
    std::vector<VkSemaphore> _wait_semaphores;
    std::vector<VkSwapchainKHR> _swapchains;
    std::vector<uint32_t> _image_indices;
};

class DeviceCreateInfo {
public:
    DeviceCreateInfo& set_extensions(const std::vector<const char*>& extensions) { _extensions = extensions; return *this; }
    DeviceCreateInfo& set_layers(const std::vector<const char*>& layers) { _layers = layers; return *this; }
    DeviceCreateInfo& set_queue_family_indices(QueueFamilyIndices indices) { _indices = indices; return *this; }
    DeviceCreateInfo& set_features(VkPhysicalDeviceFeatures features) { _features = features; return *this; }

    VkDeviceCreateInfo to_vk_device_create_info() {
        std::set<uint32_t> unique_queue_families = {
            _indices.graphics_family.value(),
            _indices.present_family.value()
        };

        _queue_create_infos.clear();
        _queue_priorities.clear();
        for (uint32_t family : unique_queue_families) {
            _queue_priorities.push_back(1.0f);
            VkDeviceQueueCreateInfo qci{};
            qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qci.queueFamilyIndex = family;
            qci.queueCount = 1;
            qci.pQueuePriorities = &_queue_priorities.back();
            _queue_create_infos.push_back(qci);
        }

        VkDeviceCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        ci.queueCreateInfoCount = static_cast<uint32_t>(_queue_create_infos.size());
        ci.pQueueCreateInfos = _queue_create_infos.data();
        ci.pEnabledFeatures = &_features;
        ci.enabledExtensionCount = static_cast<uint32_t>(_extensions.size());
        ci.ppEnabledExtensionNames = _extensions.data();
#ifdef VLK_ENABLE_VALIDATION_LAYERS
        ci.enabledLayerCount = static_cast<uint32_t>(_layers.size());
        ci.ppEnabledLayerNames = _layers.data();
#else
        ci.enabledLayerCount = 0;
#endif
        return ci;
    }
private:
    std::vector<const char*> _extensions{};
    std::vector<const char*> _layers{};
    QueueFamilyIndices _indices{};
    VkPhysicalDeviceFeatures _features{};
    std::vector<VkDeviceQueueCreateInfo> _queue_create_infos{};
    std::vector<float> _queue_priorities;
};

class Device {
public:
    Device(VkPhysicalDevice physical_device, QueueFamilyIndices indices, VkSurfaceKHR surface, const VkDeviceCreateInfo& ci) {
        if (vkCreateDevice(physical_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device");
        }
        
        vkGetDeviceQueue(_handle, indices.graphics_family.value(), 0, &_graphics_queue);
        vkGetDeviceQueue(_handle, indices.present_family.value(), 0, &_present_queue);
    }

    ~Device() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyDevice(_handle, nullptr);
        }
    }

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    Device(Device&& other) noexcept
        : _handle(other._handle),
          _graphics_queue(other._graphics_queue),
          _present_queue(other._present_queue) 
    {
        other._handle = VK_NULL_HANDLE;
    }

    Device& operator=(Device&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyDevice(_handle, nullptr);
            }

            _handle = other._handle;
            _graphics_queue = other._graphics_queue;
            _present_queue = other._present_queue;

            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkDevice& handle() const { return _handle; }
    const VkQueue& graphics_queue() const { return _graphics_queue; }
    const VkQueue& present_queue() const { return _present_queue; }
private:
    VkDevice _handle;
    VkQueue _graphics_queue;
    VkQueue _present_queue;
};

}

#endif
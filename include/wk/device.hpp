#ifndef wulkan_wk_DEVICE_HPP
#define wulkan_wk_DEVICE_HPP

#include "wulkan_internal.hpp"
#include "queue.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <set>

namespace wk {

class Device {
public:
    Device() = default;
    Device(VkPhysicalDevice physical_device, DeviceQueueFamilyIndices indices, const VkDeviceCreateInfo& ci) {
        if (vkCreateDevice(physical_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device");
        }

        if (indices.graphics_family)
            _graphics_queue = wk::Queue(_handle, indices.graphics_family.value());
        if (indices.compute_family)
            _compute_queue = wk::Queue(_handle, indices.graphics_family.value());
        if (indices.transfer_family)
            _transfer_queue = wk::Queue(_handle, indices.graphics_family.value());
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
          _graphics_queue(std::move(other._graphics_queue)),
          _compute_queue(std::move(other._compute_queue)),
          _transfer_queue(std::move(other._transfer_queue))
    {
        other._handle = VK_NULL_HANDLE;
    }

    Device& operator=(Device&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyDevice(_handle, nullptr);
            }

            _handle = other._handle;
            _graphics_queue = std::move(other._graphics_queue);
            _compute_queue = std::move(other._compute_queue);
            _transfer_queue = std::move(other._transfer_queue);

            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkDevice& handle() const { return _handle; }
    const Queue& graphics_queue() const { return _graphics_queue; }
    const Queue& compute_queue()  const { return _compute_queue; }
    const Queue& transfer_queue() const { return _transfer_queue; }

private:
    VkDevice _handle = VK_NULL_HANDLE;
    Queue _graphics_queue;
    Queue _compute_queue;
    Queue _transfer_queue;
};

class GraphicsQueueSubmitInfo {
public:
    GraphicsQueueSubmitInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    GraphicsQueueSubmitInfo& set_wait_semaphores(uint32_t count, const VkSemaphore* semaphores) {
        _wait_semaphore_count = count;
        _p_wait_semaphores = semaphores;
        return *this;
    }
    GraphicsQueueSubmitInfo& set_wait_dst_stage_masks(uint32_t count, const VkPipelineStageFlags* stage_masks) {
        _wait_dst_stage_mask_count = count;
        _p_wait_dst_stage_masks = stage_masks;
        return *this;
    }
    GraphicsQueueSubmitInfo& set_command_buffers(uint32_t count, const VkCommandBuffer* command_buffers) {
        _command_buffer_count = count;
        _p_command_buffers = command_buffers;
        return *this;
    }
    GraphicsQueueSubmitInfo& set_signal_semaphores(uint32_t count, const VkSemaphore* semaphores) {
        _signal_semaphore_count = count;
        _p_signal_semaphores = semaphores;
        return *this;
    }
    
    VkSubmitInfo to_vk() const {
        VkSubmitInfo si{};
        si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        si.pNext = _p_next;
        si.waitSemaphoreCount = _wait_semaphore_count;
        si.pWaitSemaphores = _p_wait_semaphores;
        si.pWaitDstStageMask = _p_wait_dst_stage_masks;
        si.commandBufferCount = _command_buffer_count;
        si.pCommandBuffers = _p_command_buffers;
        si.signalSemaphoreCount = _signal_semaphore_count;
        si.pSignalSemaphores = _p_signal_semaphores;
        return si;
    }

private:
    const void* _p_next = nullptr;
    uint32_t _wait_semaphore_count = 0;
    const VkSemaphore* _p_wait_semaphores = nullptr;
    uint32_t _wait_dst_stage_mask_count = 0;
    const VkPipelineStageFlags* _p_wait_dst_stage_masks = nullptr;
    uint32_t _command_buffer_count = 0;
    const VkCommandBuffer* _p_command_buffers = nullptr;
    uint32_t _signal_semaphore_count = 0;
    const VkSemaphore* _p_signal_semaphores = nullptr;
};

class PresentInfo {
public:
    PresentInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PresentInfo& set_wait_semaphores(uint32_t count, const VkSemaphore* semaphores) {
        _wait_semaphore_count = count;
        _p_wait_semaphores = semaphores;
        return *this;
    }
    PresentInfo& set_swapchains(uint32_t count, const VkSwapchainKHR* swapchains) {
        _swapchain_count = count;
        _p_swapchains = swapchains;
        return *this;
    }
    PresentInfo& set_image_indices(const uint32_t* indices) {
        _p_image_indices = indices;
        return *this;
    }

    VkPresentInfoKHR to_vk() const {
        VkPresentInfoKHR pi{};
        pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        pi.pNext = _p_next;
        pi.waitSemaphoreCount = _wait_semaphore_count;
        pi.pWaitSemaphores = _p_wait_semaphores;
        pi.swapchainCount = _swapchain_count;
        pi.pSwapchains = _p_swapchains;
        pi.pImageIndices = _p_image_indices;
        pi.pResults = nullptr;
        return pi;
    }

private:
    const void* _p_next = nullptr;
    uint32_t _wait_semaphore_count = 0;
    const VkSemaphore* _p_wait_semaphores = nullptr;
    uint32_t _swapchain_count = 0;
    const VkSwapchainKHR* _p_swapchains = nullptr;
    const uint32_t* _p_image_indices = nullptr;
};

class DeviceQueueCreateInfo {
public:
    DeviceQueueCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    DeviceQueueCreateInfo& set_flags(VkDeviceQueueCreateFlags flags) { _flags = flags; return *this; }
    DeviceQueueCreateInfo& set_queue_family_index(uint32_t index) { _queue_family_index = index; return *this; }
    DeviceQueueCreateInfo& set_queue_count(uint32_t count) { _queue_count = count; return *this; }
    DeviceQueueCreateInfo& set_p_queue_priorities(const float* priorities) { _p_queue_priorities = priorities; return *this; }

    VkDeviceQueueCreateInfo to_vk() const {
        VkDeviceQueueCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.queueFamilyIndex = _queue_family_index;
        ci.queueCount = _queue_count;
        ci.pQueuePriorities = _p_queue_priorities;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkDeviceQueueCreateFlags _flags = 0;
    uint32_t _queue_family_index = 0;
    uint32_t _queue_count = 1;
    const float* _p_queue_priorities = nullptr;
};

class DeviceCreateInfo {
public:
    DeviceCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    DeviceCreateInfo& set_flags(VkDeviceCreateFlags flags) { _flags = flags; return *this; }
    DeviceCreateInfo& set_p_enabled_features(const VkPhysicalDeviceFeatures* p_enabled_features) { _p_enabled_features = p_enabled_features; return *this; }
    DeviceCreateInfo& set_enabled_extensions(uint32_t count, const char* const* pp_names) {
        _enabled_extension_count = count;
        _pp_enabled_extension_names = pp_names; 
        return *this; 
    }
    DeviceCreateInfo& set_queue_create_infos(uint32_t count, const VkDeviceQueueCreateInfo* infos) {
        _queue_create_info_count = count;
        _p_queue_create_infos = infos;
        return *this;
    }    

    VkDeviceCreateInfo to_vk() {
        VkDeviceCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.queueCreateInfoCount = _queue_create_info_count;
        ci.pQueueCreateInfos = _p_queue_create_infos;
        ci.enabledExtensionCount = _enabled_extension_count;
        ci.ppEnabledExtensionNames = _pp_enabled_extension_names;
        ci.pEnabledFeatures = _p_enabled_features;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkDeviceCreateFlags _flags = 0;
    const VkPhysicalDeviceFeatures* _p_enabled_features = nullptr;
    uint32_t _enabled_extension_count = 0;
    const char* const* _pp_enabled_extension_names = nullptr;
    uint32_t _queue_create_info_count = 0;
    const VkDeviceQueueCreateInfo* _p_queue_create_infos = nullptr;
};

} // namespace wk

#endif
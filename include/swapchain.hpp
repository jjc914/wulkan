#ifndef wulkan_wk_SWAPCHAIN_HPP
#define wulkan_wk_SWAPCHAIN_HPP

#include <cstdint>
#include <iostream>
#include <algorithm>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "wulkan_internal.hpp"

namespace wk {

class SwapchainCreateInfo {
public:
    SwapchainCreateInfo& set_surface(VkSurfaceKHR surface) { _surface = surface; return *this; }
    SwapchainCreateInfo& set_physical_device(VkPhysicalDevice physical_device) { _physical_device = physical_device; return *this; }
    SwapchainCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    SwapchainCreateInfo& set_width(uint32_t width) { _width = width; return *this; }
    SwapchainCreateInfo& set_height(uint32_t height) { _height = height; return *this; }
    SwapchainCreateInfo& set_queue_family_indices(QueueFamilyIndices queue_family_indices) { _queue_family_indices = queue_family_indices; return *this; }
private:
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    uint32_t _width = 0;
    uint32_t _height = 0;
    QueueFamilyIndices _queue_family_indices{};

    friend class Swapchain;
};

class Swapchain {
public:
    Swapchain(const SwapchainCreateInfo& ci) {
        PhysicalDeviceSurfaceSupportDetails details = QueryPhysicalDeviceSurfaceSupport(ci._physical_device, ci._surface);

        VkSurfaceFormatKHR surface_format = ChooseSurfaceFormat(details.formats);
        VkPresentModeKHR presentMode = ChooseSurfacePresentationMode(details.present_modes);
        VkExtent2D extent = ChooseSurfaceExtent(ci._width, ci._height, details.capabilities);
        
        uint32_t image_count = details.capabilities.minImageCount + 1;
        if (details.capabilities.maxImageCount > 0) {
            image_count = std::clamp(image_count,
                                    details.capabilities.minImageCount,
                                    details.capabilities.maxImageCount);
        }

        VkSwapchainCreateInfoKHR vkci{};
        uint32_t queue_family_indices[] = {ci._queue_family_indices.graphics_family.value(), ci._queue_family_indices.present_family.value()};
        
        vkci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        vkci.surface = ci._surface;
        vkci.minImageCount = image_count;
        vkci.imageFormat = surface_format.format;
        vkci.imageColorSpace = surface_format.colorSpace;
        vkci.imageExtent = extent;
        vkci.imageArrayLayers = 1;
        vkci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (ci._queue_family_indices.graphics_family != ci._queue_family_indices.present_family) {
            vkci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            vkci.queueFamilyIndexCount = 2;
            vkci.pQueueFamilyIndices = queue_family_indices;
        } else {
            vkci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        vkci.preTransform = details.capabilities.currentTransform;
        vkci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        vkci.presentMode = presentMode;
        vkci.clipped = VK_TRUE;
        vkci.oldSwapchain = VK_NULL_HANDLE;
        
        if (vkCreateSwapchainKHR(ci._device, &vkci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create swapchain" << std::endl;
        }

        vkGetSwapchainImagesKHR(ci._device, _handle, &image_count, nullptr);
        _images.resize(image_count);
        vkGetSwapchainImagesKHR(ci._device, _handle, &image_count, _images.data());

        _device = ci._device;
        _image_format = surface_format.format;
        _extent = extent;
        
        std::clog << "created swapchain" << std::endl;;

        _image_views.resize(_images.size());
        for (size_t i = 0; i < _image_views.size(); ++i) {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = _images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = _image_format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_device, &create_info, nullptr, &_image_views[i]) != VK_SUCCESS) {
                std::cerr << "failed to create image views" << std::endl;
            }
        }
        std::clog << "swapchain: created " << _image_views.size() << " image views" << std::endl;
    }

    ~Swapchain() {
        for (size_t i = 0; i < _image_views.size(); ++i) {
            vkDestroyImageView(_device, _image_views[i], nullptr);
        }

        vkDestroySwapchainKHR(_device, _handle, nullptr);
    }

    uint32_t AcquireNextImageIndex(uint64_t timeout, VkSemaphore semaphore_to_signal, VkFence fence_to_signal) {
        uint32_t image_index;
        VkResult result = vkAcquireNextImageKHR(_device, _handle, UINT64_MAX, semaphore_to_signal, fence_to_signal, &image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR/* || _vlk_is_resized_framebuffer*/) {
            // _vlk_update_swapchain();
            // _vlk_reset_semaphores();
            // return;
            std::cerr << "resize framebuffer" << std::endl;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            std::cerr << "failed to acquire swap chain image" << std::endl;;
        }
        return image_index;
    }

    VkSwapchainKHR handle() const { return _handle; }
    VkFormat image_format() const { return _image_format; }
    std::vector<VkImageView> image_views() const { return _image_views; }
    VkExtent2D extent() const { return _extent; }
private:
    VkSwapchainKHR _handle;
    VkDevice _device;
    std::vector<VkImage> _images;
    VkFormat _image_format;
    VkExtent2D _extent;

    std::vector<VkImageView> _image_views;
};

}

#endif
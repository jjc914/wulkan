#ifndef wulkan_wk_SWAPCHAIN_HPP
#define wulkan_wk_SWAPCHAIN_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <optional>

namespace wk {

class SwapchainCreateInfo {
public:
    SwapchainCreateInfo& set_surface(VkSurfaceKHR surface) { _surface = surface; return *this; }
    SwapchainCreateInfo& set_physical_device(VkPhysicalDevice physical_device) { _physical_device = physical_device; return *this; }
    SwapchainCreateInfo& set_width(uint32_t width) { _width = width; return *this; }
    SwapchainCreateInfo& set_height(uint32_t height) { _height = height; return *this; }
    SwapchainCreateInfo& set_queue_family_indices(QueueFamilyIndices indices) { _queue_family_indices = indices; return *this; }
    SwapchainCreateInfo& set_old_swapchain(VkSwapchainKHR old_swapchain) { _old_swapchain = old_swapchain; return *this; }

    VkSwapchainCreateInfoKHR to_vk_swapchain_create_info() {
        PhysicalDeviceSurfaceSupportDetails details = QueryPhysicalDeviceSurfaceSupport(_physical_device, _surface);

        VkSurfaceFormatKHR format = ChooseSurfaceFormat(details.formats);
        VkPresentModeKHR present_mode = ChooseSurfacePresentationMode(details.present_modes);
        VkExtent2D extent = ChooseSurfaceExtent(_width, _height, details.capabilities);

        uint32_t image_count = details.capabilities.minImageCount + 1;
        if (details.capabilities.maxImageCount > 0) {
            image_count = std::clamp(image_count, details.capabilities.minImageCount, details.capabilities.maxImageCount);
        }

        VkSwapchainCreateInfoKHR ci{};
        ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        ci.surface = _surface;
        ci.minImageCount = image_count;
        ci.imageFormat = format.format;
        ci.imageColorSpace = format.colorSpace;
        ci.imageExtent = extent;
        ci.imageArrayLayers = 1;
        ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (_queue_family_indices.graphics_family != _queue_family_indices.present_family) {
            ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            _queue_family_indices_vec = { 
                _queue_family_indices.graphics_family.value(), 
                _queue_family_indices.present_family.value() 
            };
            ci.queueFamilyIndexCount = static_cast<uint32_t>(_queue_family_indices_vec.size());
            ci.pQueueFamilyIndices = _queue_family_indices_vec.data();
        } else {
            ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            ci.queueFamilyIndexCount = 0;
            ci.pQueueFamilyIndices = nullptr;
        }

        ci.preTransform = details.capabilities.currentTransform;
        ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        ci.presentMode = present_mode;
        ci.clipped = VK_TRUE;
        ci.oldSwapchain = _old_swapchain;

        return ci;
    }
private:
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    uint32_t _width = 0;
    uint32_t _height = 0;
    QueueFamilyIndices _queue_family_indices{};
    std::vector<uint32_t> _queue_family_indices_vec{};
    VkSwapchainKHR _old_swapchain = VK_NULL_HANDLE;
};

class Swapchain {
public:
    Swapchain(VkDevice device, const VkSwapchainCreateInfoKHR& ci)
        : _device(device), _image_format(ci.imageFormat), _extent(ci.imageExtent)
    {
        if (vkCreateSwapchainKHR(_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swapchain");
        }

        uint32_t image_count = 0;
        vkGetSwapchainImagesKHR(_device, _handle, &image_count, nullptr);
        _images.resize(image_count);
        vkGetSwapchainImagesKHR(_device, _handle, &image_count, _images.data());

        _image_views.resize(_images.size());
        for (size_t i = 0; i < _images.size(); ++i) {
            VkImageViewCreateInfo view_info{};
            view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image = _images[i];
            view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format = _image_format;
            view_info.components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            };
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.subresourceRange.baseMipLevel = 0;
            view_info.subresourceRange.levelCount = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_device, &view_info, nullptr, &_image_views[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views");
            }
        }
    }

    ~Swapchain() {
        Destroy();
    }

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    Swapchain(Swapchain&& other) noexcept
        : _handle(other._handle),
          _device(other._device),
          _images(std::move(other._images)),
          _image_format(other._image_format),
          _extent(other._extent),
          _image_views(std::move(other._image_views))
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    Swapchain& operator=(Swapchain&& other) noexcept {
        if (this != &other) {
            Destroy();

            _handle = other._handle;
            _device = other._device;
            _images = std::move(other._images);
            _image_format = other._image_format;
            _extent = other._extent;
            _image_views = std::move(other._image_views);

            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkSwapchainKHR& handle() const { return _handle; }
    const VkFormat& image_format() const { return _image_format; }
    const VkFormat& depth_format() const { return _depth_format; }
    const std::vector<VkImageView>& image_views() const { return _image_views; }
    const VkExtent2D& extent() const { return _extent; }
private:
    void Destroy() {
        for (size_t i = 0; i < _image_views.size(); ++i) {
            if (_image_views[i] != VK_NULL_HANDLE) {
                vkDestroyImageView(_device, _image_views[i], nullptr);
            }
        }
        if (_handle != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(_device, _handle, nullptr);
        }
    }

    VkSwapchainKHR _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    std::vector<VkImage> _images;
    VkFormat _image_format;
    VkFormat _depth_format;
    VkExtent2D _extent;
    std::vector<VkImageView> _image_views;
};

}

#endif
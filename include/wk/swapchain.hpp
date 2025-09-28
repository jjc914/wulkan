#ifndef wulkan_wk_SWAPCHAIN_HPP
#define wulkan_wk_SWAPCHAIN_HPP

#include "wulkan_internal.hpp"
#include "image_view.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <optional>

namespace wk {

class SwapchainCreateInfo {
public:
    SwapchainCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    SwapchainCreateInfo& set_flags(VkSwapchainCreateFlagsKHR flags) { _flags = flags; return *this; }
    SwapchainCreateInfo& set_surface(VkSurfaceKHR surface) { _surface = surface; return *this; }
    SwapchainCreateInfo& set_min_image_count(uint32_t count) { _min_image_count = count; return *this; }
    SwapchainCreateInfo& set_image_format(VkFormat format) { _image_format = format; return *this; }
    SwapchainCreateInfo& set_image_color_space(VkColorSpaceKHR color_space) { _image_color_space = color_space; return *this; }
    SwapchainCreateInfo& set_image_extent(VkExtent2D extent) { _image_extent = extent; return *this; }
    SwapchainCreateInfo& set_image_array_layers(uint32_t layers) { _image_array_layers = layers; return *this; }
    SwapchainCreateInfo& set_image_usage(VkImageUsageFlags usage) { _image_usage = usage; return *this; }
    SwapchainCreateInfo& set_image_sharing_mode(VkSharingMode mode) { _image_sharing_mode = mode; return *this; }
    SwapchainCreateInfo& set_queue_family_indices(uint32_t count, const uint32_t* indices) { 
        _queue_family_index_count = count; 
        _p_queue_family_indices = indices; 
        return *this; 
    }
    SwapchainCreateInfo& set_pre_transform(VkSurfaceTransformFlagBitsKHR transform) { _pre_transform = transform; return *this; }
    SwapchainCreateInfo& set_composite_alpha(VkCompositeAlphaFlagBitsKHR alpha) { _composite_alpha = alpha; return *this; }
    SwapchainCreateInfo& set_present_mode(VkPresentModeKHR mode) { _present_mode = mode; return *this; }
    SwapchainCreateInfo& set_clipped(VkBool32 clipped) { _clipped = clipped; return *this; }
    SwapchainCreateInfo& set_old_swapchain(VkSwapchainKHR swapchain) { _old_swapchain = swapchain; return *this; }

    VkSwapchainCreateInfoKHR to_vk() const {
        VkSwapchainCreateInfoKHR ci{};
        ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.surface = _surface;
        ci.minImageCount = _min_image_count;
        ci.imageFormat = _image_format;
        ci.imageColorSpace = _image_color_space;
        ci.imageExtent = _image_extent;
        ci.imageArrayLayers = _image_array_layers;
        ci.imageUsage = _image_usage;
        ci.imageSharingMode = _image_sharing_mode;
        ci.queueFamilyIndexCount = _queue_family_index_count;
        ci.pQueueFamilyIndices = _p_queue_family_indices;
        ci.preTransform = _pre_transform;
        ci.compositeAlpha = _composite_alpha;
        ci.presentMode = _present_mode;
        ci.clipped = _clipped;
        ci.oldSwapchain = _old_swapchain;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkSwapchainCreateFlagsKHR _flags = 0;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    uint32_t _min_image_count = 0;
    VkFormat _image_format = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR _image_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D _image_extent = {0, 0};
    uint32_t _image_array_layers = 1;
    VkImageUsageFlags _image_usage = 0;
    VkSharingMode _image_sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    uint32_t _queue_family_index_count = 0;
    const uint32_t* _p_queue_family_indices = nullptr;
    VkSurfaceTransformFlagBitsKHR _pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR _composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkPresentModeKHR _present_mode = VK_PRESENT_MODE_FIFO_KHR;
    VkBool32 _clipped = VK_TRUE;
    VkSwapchainKHR _old_swapchain = VK_NULL_HANDLE;
};

class Swapchain {
public:
    Swapchain() noexcept = default;
    Swapchain(VkDevice device, const VkSwapchainCreateInfoKHR& ci)
        : _device(device), _image_format(ci.imageFormat), _extent(ci.imageExtent), 
          _queue_family_indices(ci.pQueueFamilyIndices, ci.pQueueFamilyIndices + ci.queueFamilyIndexCount),
          _image_sharing_mode(ci.imageSharingMode) 
    {
        if (vkCreateSwapchainKHR(_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create swapchain" << std::endl;
        }

        uint32_t image_count = 0;
        vkGetSwapchainImagesKHR(_device, _handle, &image_count, nullptr);
        _images.resize(image_count);
        vkGetSwapchainImagesKHR(_device, _handle, &image_count, _images.data());

        _image_views.resize(_images.size());
        for (size_t i = 0; i < _images.size(); ++i) {
            VkImageViewCreateInfo view_info = ImageViewCreateInfo{}
                .set_image(_images[i])
                .set_view_type(VK_IMAGE_VIEW_TYPE_2D)
                .set_format(_image_format)
                .set_components(ComponentMapping::identity().to_vk())
                .set_subresource_range(ImageSubresourceRange::color().to_vk())
                .to_vk();
            if (vkCreateImageView(_device, &view_info, nullptr, &_image_views[i]) != VK_SUCCESS) {
                std::cerr << "failed to create image views" << std::endl;
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
          _image_views(std::move(other._image_views)),
          _queue_family_indices(other._queue_family_indices),
          _image_sharing_mode(other._image_sharing_mode)
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
            _queue_family_indices = other._queue_family_indices;
            _image_sharing_mode = other._image_sharing_mode;

            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkSwapchainKHR& handle() const { return _handle; }
    const VkFormat& image_format() const { return _image_format; }
    const std::vector<VkImageView>& image_views() const { return _image_views; }
    uint32_t image_count() const { return static_cast<uint32_t>(_images.size()); }
    const VkExtent2D& extent() const { return _extent; }
    const std::vector<uint32_t>& queue_family_indices() const { return _queue_family_indices; }
    const VkSharingMode& image_sharing_mode() const { return _image_sharing_mode; }
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
    VkExtent2D _extent;
    std::vector<VkImageView> _image_views;
    std::vector<uint32_t> _queue_family_indices;
    VkSharingMode _image_sharing_mode;
};

}

#endif
#ifndef wulkan_wk_IMAGE_VIEW_HPP
#define wulkan_wk_IMAGE_VIEW_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace wk {

class ComponentMapping {
public:
    ComponentMapping& set_r(VkComponentSwizzle swizzle) { _r = swizzle; return *this; }
    ComponentMapping& set_g(VkComponentSwizzle swizzle) { _g = swizzle; return *this; }
    ComponentMapping& set_b(VkComponentSwizzle swizzle) { _b = swizzle; return *this; }
    ComponentMapping& set_a(VkComponentSwizzle swizzle) { _a = swizzle; return *this; }

    VkComponentMapping to_vk() const {
        VkComponentMapping cm{};
        cm.r = _r;
        cm.g = _g;
        cm.b = _b;
        cm.a = _a;
        return cm;
    }

    // Static helper to create identity mapping
    static ComponentMapping identity() {
        return ComponentMapping()
            .set_r(VK_COMPONENT_SWIZZLE_IDENTITY)
            .set_g(VK_COMPONENT_SWIZZLE_IDENTITY)
            .set_b(VK_COMPONENT_SWIZZLE_IDENTITY)
            .set_a(VK_COMPONENT_SWIZZLE_IDENTITY);
    }

private:
    VkComponentSwizzle _r = VK_COMPONENT_SWIZZLE_IDENTITY;
    VkComponentSwizzle _g = VK_COMPONENT_SWIZZLE_IDENTITY;
    VkComponentSwizzle _b = VK_COMPONENT_SWIZZLE_IDENTITY;
    VkComponentSwizzle _a = VK_COMPONENT_SWIZZLE_IDENTITY;
};

class ImageSubresourceRange {
public:
    ImageSubresourceRange& set_aspect_mask(VkImageAspectFlags aspect_mask) { _aspect_mask = aspect_mask; return *this; }
    ImageSubresourceRange& set_base_mip_level(uint32_t base_mip_level) { _base_mip_level = base_mip_level; return *this; }
    ImageSubresourceRange& set_level_count(uint32_t level_count) { _level_count = level_count; return *this; }
    ImageSubresourceRange& set_base_array_layer(uint32_t base_array_layer) { _base_array_layer = base_array_layer; return *this; }
    ImageSubresourceRange& set_layer_count(uint32_t layer_count) { _layer_count = layer_count; return *this; }

    VkImageSubresourceRange to_vk() const {
        VkImageSubresourceRange range{};
        range.aspectMask = _aspect_mask;
        range.baseMipLevel = _base_mip_level;
        range.levelCount = _level_count;
        range.baseArrayLayer = _base_array_layer;
        range.layerCount = _layer_count;
        return range;
    }

    // presets
    static ImageSubresourceRange color() {
        return ImageSubresourceRange()
            .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT)
            .set_base_mip_level(0)
            .set_level_count(1)
            .set_base_array_layer(0)
            .set_layer_count(1);
    }
    static ImageSubresourceRange depth() {
        return ImageSubresourceRange()
            .set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT)
            .set_base_mip_level(0)
            .set_level_count(1)
            .set_base_array_layer(0)
            .set_layer_count(1);
    }
    static ImageSubresourceRange depth_stencil() {
        return ImageSubresourceRange()
            .set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
            .set_base_mip_level(0)
            .set_level_count(1)
            .set_base_array_layer(0)
            .set_layer_count(1);
    }

private:
    VkImageAspectFlags _aspect_mask = 0;
    uint32_t _base_mip_level = 0;
    uint32_t _level_count = 1;
    uint32_t _base_array_layer = 0;
    uint32_t _layer_count = 1;
};

class ImageViewCreateInfo {
public:
    ImageViewCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    ImageViewCreateInfo& set_flags(VkImageViewCreateFlags flags) { _flags = flags; return *this; }
    ImageViewCreateInfo& set_image(VkImage image) { _image = image; return *this; }
    ImageViewCreateInfo& set_view_type(VkImageViewType view_type) { _view_type = view_type; return *this; }
    ImageViewCreateInfo& set_format(VkFormat format) { _format = format; return *this; }
    ImageViewCreateInfo& set_components(VkComponentMapping components) { _components = components; return *this; }
    ImageViewCreateInfo& set_subresource_range(VkImageSubresourceRange subresource_range) { _subresource_range = subresource_range; return *this; }

    VkImageViewCreateInfo to_vk() const {
        VkImageViewCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.image = _image;
        ci.viewType = _view_type;
        ci.format = _format;
        ci.components = _components;
        ci.subresourceRange = _subresource_range;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkImageViewCreateFlags _flags = 0;
    VkImage _image = VK_NULL_HANDLE;
    VkImageViewType _view_type = VK_IMAGE_VIEW_TYPE_2D;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkComponentMapping _components{};
    VkImageSubresourceRange _subresource_range{};
};

class ImageView {
public:
    ImageView() noexcept = default;
    ImageView(VkDevice device, const VkImageViewCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateImageView(device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create image view!" << std::endl;
        }
    }

    ~ImageView() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyImageView(_device, _handle, nullptr);
        }
    }

    ImageView(const ImageView&) = delete;
    ImageView& operator=(const ImageView&) = delete;

    ImageView(ImageView&& other) noexcept
        : _device(other._device), _handle(other._handle)
    {
        other._handle = VK_NULL_HANDLE;
    }

    ImageView& operator=(ImageView&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyImageView(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkImageView& handle() const { return _handle; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkImageView _handle = VK_NULL_HANDLE;
};

}

#endif

#ifndef wulkan_wk_IMAGE_CREATE_INFO_HPP
#define wulkan_wk_IMAGE_CREATE_INFO_HPP

#include "vma_include.hpp"
#include "wulkan_internal.hpp"
#include "allocator.hpp"

#include <cstdint>

namespace wk {

class Extent {
public:
    Extent(VkExtent2D e) {
        _width = e.width;
        _height = e.height;
        _depth = 1;
    }
    Extent(VkExtent3D e) {
        _width = e.width;
        _height = e.height;
        _depth = e.depth;
    }

    Extent& set_width(uint32_t width) { _width = width; return *this; }
    Extent& set_height(uint32_t height) { _height = height; return *this; }
    Extent& set_depth(uint32_t depth) { _depth = depth; return *this; }

    VkExtent2D to_vk_extent_2d() const {
        return VkExtent2D{ _width, _height };
    }
    VkExtent3D to_vk_extent_3d() const {
        return VkExtent3D{ _width, _height, _depth };
    }

private:
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _depth = 1;
};

class ImageCreateInfo {
public:
    ImageCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    ImageCreateInfo& set_flags(VkImageCreateFlags flags) { _flags = flags; return *this; }
    ImageCreateInfo& set_image_type(VkImageType type) { _image_type = type; return *this; }
    ImageCreateInfo& set_format(VkFormat format) { _format = format; return *this; }
    ImageCreateInfo& set_extent(VkExtent3D extent) { _extent = extent; return *this; }
    ImageCreateInfo& set_mip_levels(uint32_t mip_levels) { _mip_levels = mip_levels; return *this; }
    ImageCreateInfo& set_array_layers(uint32_t array_layers) { _array_layers = array_layers; return *this; }
    ImageCreateInfo& set_samples(VkSampleCountFlagBits samples) { _samples = samples; return *this; }
    ImageCreateInfo& set_tiling(VkImageTiling tiling) { _tiling = tiling; return *this; }
    ImageCreateInfo& set_usage(VkImageUsageFlags usage) { _usage = usage; return *this; }
    ImageCreateInfo& set_sharing_mode(VkSharingMode sharing_mode) { _sharing_mode = sharing_mode; return *this; }
    ImageCreateInfo& set_queue_family_indices(uint32_t count, const uint32_t* indices) {
        _queue_family_index_count = count;
        _p_queue_family_indices = indices;
        return *this;
    }
    ImageCreateInfo& set_initial_layout(VkImageLayout layout) { _initial_layout = layout; return *this; }

    VkImageCreateInfo to_vk_image_create_info() const {
        VkImageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.imageType = _image_type;
        ci.format = _format;
        ci.extent = _extent;
        ci.mipLevels = _mip_levels;
        ci.arrayLayers = _array_layers;
        ci.samples = _samples;
        ci.tiling = _tiling;
        ci.usage = _usage;
        ci.sharingMode = _sharing_mode;
        ci.queueFamilyIndexCount = _queue_family_index_count;
        ci.pQueueFamilyIndices = _p_queue_family_indices;
        ci.initialLayout = _initial_layout;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkImageCreateFlags _flags = 0;
    VkImageType _image_type = VK_IMAGE_TYPE_2D;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkExtent3D _extent = {0, 0, 0};
    uint32_t _mip_levels = 1;
    uint32_t _array_layers = 1;
    VkSampleCountFlagBits _samples = VK_SAMPLE_COUNT_1_BIT;
    VkImageTiling _tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags _usage = 0;
    VkSharingMode _sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    uint32_t _queue_family_index_count = 0;
    const uint32_t* _p_queue_family_indices = nullptr;
    VkImageLayout _initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

class Image {
public:
    Image(VmaAllocator allocator, const VkImageCreateInfo& create_info, const VmaAllocationCreateInfo& alloc_info)
        : _allocator(allocator)
    {
        if (vmaCreateImage(allocator, &create_info, &alloc_info, &_handle, &_allocation, nullptr) != VK_SUCCESS) {
            std::cerr << "failed to create image!" << std::endl;
        }
    }

    ~Image() {
        if (_handle != VK_NULL_HANDLE) {
            vmaDestroyImage(_allocator, _handle, _allocation);
        }
    }

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    Image(Image&& other) noexcept
        : _allocator(other._allocator),
          _handle(other._handle),
          _allocation(other._allocation)
    {
        other._handle = VK_NULL_HANDLE;
        other._allocation = VK_NULL_HANDLE;
    }

    Image& operator=(Image&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vmaDestroyImage(_allocator, _handle, _allocation);
            }
            _allocator = other._allocator;
            _handle = other._handle;
            _allocation = other._allocation;

            other._handle = VK_NULL_HANDLE;
            other._allocation = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkImage& handle() const { return _handle; }
    const VmaAllocation& allocation() const { return _allocation; }

private:
    VmaAllocator _allocator = VK_NULL_HANDLE;
    VkImage _handle = VK_NULL_HANDLE;
    VmaAllocation _allocation = VK_NULL_HANDLE;
};

}

#endif

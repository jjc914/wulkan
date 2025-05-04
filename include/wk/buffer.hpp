#ifndef wulkan_wk_BUFFER_HPP
#define wulkan_wk_BUFFER_HPP

#include "vma_include.hpp"
#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class BufferCreateInfo {
public:
    BufferCreateInfo& set_flags(VkBufferCreateFlags flags) { _flags = flags; return *this; }
    BufferCreateInfo& set_size(VkDeviceSize size) { _size = size; return *this; }
    BufferCreateInfo& set_usage(VkBufferUsageFlags usage) { _usage = usage; return *this; }
    BufferCreateInfo& set_sharing_mode(VkSharingMode sharing_mode) { _sharing_mode = sharing_mode; return *this; }
    BufferCreateInfo& set_queue_family_indices(uint32_t count, const uint32_t* indices) {
        _queue_family_index_count = count;
        _p_queue_family_indices = indices;
        return *this;
    }

    VkBufferCreateInfo to_vk_buffer_create_info() const {
        VkBufferCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        ci.flags = _flags;
        ci.size = _size;
        ci.usage = _usage;
        ci.sharingMode = _sharing_mode;
        ci.queueFamilyIndexCount = _queue_family_index_count;
        ci.pQueueFamilyIndices = _p_queue_family_indices;
        return ci;
    }

private:
    VkBufferCreateFlags _flags = 0;
    VkDeviceSize _size = 0;
    VkBufferUsageFlags _usage = 0;
    VkSharingMode _sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    uint32_t _queue_family_index_count = 0;
    const uint32_t* _p_queue_family_indices = nullptr;
};

class Buffer {
public:
    Buffer(VmaAllocator allocator, const VkBufferCreateInfo& ci, const VmaAllocationCreateInfo& aci)
        : _allocator(allocator)
    {
        if (vmaCreateBuffer(_allocator, &ci, &aci, &_handle, &_allocation, nullptr) != VK_SUCCESS) {
            std::cerr << "failed to create buffer" << std::endl;
        }
    }

    ~Buffer() {
        if (_handle != VK_NULL_HANDLE) {
            vmaDestroyBuffer(_allocator, _handle, _allocation);
        }
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept
        : _handle(other._handle),
          _allocator(other._allocator),
          _allocation(other._allocation)
    {
        other._handle = VK_NULL_HANDLE;
        other._allocation = nullptr;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vmaDestroyBuffer(_allocator, _handle, _allocation);
            }
            _handle = other._handle;
            _allocator = other._allocator;
            _allocation = other._allocation;

            other._handle = VK_NULL_HANDLE;
            other._allocation = nullptr;
        }
        return *this;
    }

    const VkBuffer& handle() const { return _handle; }
    const VmaAllocation& allocation() const { return _allocation; }

private:
    VkBuffer _handle = VK_NULL_HANDLE;
    VmaAllocator _allocator = VK_NULL_HANDLE;
    VmaAllocation _allocation = nullptr;
};

}

#endif
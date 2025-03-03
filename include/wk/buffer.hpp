#ifndef wulkan_wk_BUFFER_HPP
#define wulkan_wk_BUFFER_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "vma_include.hpp"

namespace wk {

class BufferCreateInfo {
public:
    BufferCreateInfo& set_allocator(VmaAllocator allocator) { _allocator = allocator; return *this; }
    BufferCreateInfo& set_size(VkDeviceSize size) { _size = size; return *this; }
    BufferCreateInfo& set_usage(VkBufferUsageFlags usage) { _usage = usage; return *this; }
    BufferCreateInfo& set_allocation_usage(VmaMemoryUsage usage) { _allocation_usage = usage; return *this; }
    BufferCreateInfo& set_sharing_mode(VkSharingMode sharing_mode) { _sharing_mode = sharing_mode; return *this; }
private:
    VmaAllocator _allocator = VMA_NULL;
    VkDeviceSize _size = 0;
    VkBufferUsageFlags _usage = 0;
    VmaMemoryUsage _allocation_usage = VMA_MEMORY_USAGE_AUTO;
    VkSharingMode _sharing_mode = VK_SHARING_MODE_EXCLUSIVE;

    friend class Buffer;
};

class Buffer {
public:
    Buffer(const BufferCreateInfo& ci) {
        VkBufferCreateInfo vkci = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        vkci.size = ci._size;
        vkci.usage = ci._usage;
        vkci.sharingMode = ci._sharing_mode;
        
        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage = ci._allocation_usage;
        
        VkBuffer buffer;
        VmaAllocation allocation;
        vmaCreateBuffer(ci._allocator, &vkci, &alloc_info, &_handle, &_allocation, nullptr);

        _allocator = ci._allocator;
    }

    ~Buffer() {
        vmaDestroyBuffer(_allocator, _handle, _allocation);
    }

    VkBuffer handle() const { return _handle; }
    const VkBuffer* phandle() const { return &_handle; }
    VmaAllocation allocation() const { return _allocation; }
private:
    VkBuffer _handle;
    VmaAllocator _allocator;
    VmaAllocation _allocation;
};

}

#endif
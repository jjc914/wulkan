#ifndef wulkan_wk_BUFFER_HPP
#define wulkan_wk_BUFFER_HPP

#include "vma_include.hpp"
#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class Buffer {
public:
    Buffer() = default;
    Buffer(VmaAllocator allocator, const VkBufferCreateInfo& ci, const VmaAllocationCreateInfo& aci)
        : _allocator(allocator)
    {
        if (vmaCreateBuffer(_allocator, &ci, &aci, &_handle, &_allocation, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer");
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

class BufferDeviceAddressInfo {
public:
    BufferDeviceAddressInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    BufferDeviceAddressInfo& set_buffer(VkBuffer buffer) { _buffer = buffer; return *this; }

    VkBufferDeviceAddressInfoKHR to_vk() const {
        VkBufferDeviceAddressInfoKHR info{};
        info.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR; // extension enum
        info.pNext  = _p_next;
        info.buffer = _buffer;
        return info;
    }

private:
    const void* _p_next = nullptr;
    VkBuffer _buffer = VK_NULL_HANDLE;
};

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

    VkBufferCreateInfo to_vk() const {
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

class DeviceOrHostAddress {
public:
    DeviceOrHostAddress& set_device_address(VkDeviceAddress addr) {
        _use_device = true;
        _device_address = addr;
        return *this;
    }
    DeviceOrHostAddress& set_host_address(void* ptr) {
        _use_device = false;
        _host_address = ptr;
        return *this;
    }

    VkDeviceOrHostAddressKHR to_vk() const {
        VkDeviceOrHostAddressKHR addr{};
        if (_use_device) {
            addr.deviceAddress = _device_address;
        } else {
            addr.hostAddress = _host_address;
        }
        return addr;
    }

private:
    bool _use_device = true;
    VkDeviceAddress _device_address = 0;
    void* _host_address = nullptr;
};

class DeviceOrHostAddressConst {
public:
    DeviceOrHostAddressConst& set_device_address(VkDeviceAddress addr) {
        _use_device = true;
        _device_address = addr;
        return *this;
    }
    DeviceOrHostAddressConst& set_host_address(const void* ptr) {
        _use_device = false;
        _host_address = ptr;
        return *this;
    }

    VkDeviceOrHostAddressConstKHR to_vk() const {
        VkDeviceOrHostAddressConstKHR addr{};
        if (_use_device) {
            addr.deviceAddress = _device_address;
        } else {
            addr.hostAddress = _host_address;
        }
        return addr;
    }

private:
    bool _use_device = true;
    VkDeviceAddress _device_address = 0;
    const void* _host_address = nullptr;
};

}

#endif
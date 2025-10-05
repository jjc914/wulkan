#ifndef wulkan_wk_DESCRIPTOR_POOL_HPP
#define wulkan_wk_DESCRIPTOR_POOL_HPP

#include "wulkan_internal.hpp"

#include <vector>
#include <stdexcept>

namespace wk {

class DescriptorPool {
public:
    DescriptorPool() = default;
    DescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo& create_info)
        : _device(device) 
    {
        if (vkCreateDescriptorPool(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool");
        }
    }

    ~DescriptorPool() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(_device, _handle, nullptr);
        }
    }

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    DescriptorPool(DescriptorPool&& other) noexcept
        : _handle(other._handle), _device(other._device) {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    DescriptorPool& operator=(DescriptorPool&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyDescriptorPool(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkDescriptorPool& handle() const { return _handle; }
    
private:
    VkDescriptorPool _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

class DescriptorPoolSize {
public:
    DescriptorPoolSize& set_type(VkDescriptorType type) { _type = type; return *this; }
    DescriptorPoolSize& set_descriptor_count(uint32_t count) { _descriptor_count = count; return *this; }

    VkDescriptorPoolSize to_vk() const {
        VkDescriptorPoolSize ps{};
        ps.type = _type;
        ps.descriptorCount = _descriptor_count;
        return ps;
    }
private:
    VkDescriptorType _type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    uint32_t _descriptor_count = 0;
};

class DescriptorPoolCreateInfo {
public:
    DescriptorPoolCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    DescriptorPoolCreateInfo& set_flags(VkDescriptorPoolCreateFlags flags) { _flags = flags; return *this; }
    DescriptorPoolCreateInfo& set_max_sets(uint32_t max_sets) { _max_sets = max_sets; return *this; }
    DescriptorPoolCreateInfo& set_pool_sizes(uint32_t count, const VkDescriptorPoolSize* pool_sizes) {
        _pool_size_count = count;
        _pool_sizes = pool_sizes;
        return *this;
    }

    VkDescriptorPoolCreateInfo to_vk() const {
        VkDescriptorPoolCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.maxSets = _max_sets;
        ci.poolSizeCount = _pool_size_count;
        ci.pPoolSizes = _pool_sizes;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkDescriptorPoolCreateFlags _flags = 0;
    uint32_t _max_sets = 0;
    uint32_t _pool_size_count = 0;
    const VkDescriptorPoolSize* _pool_sizes = nullptr;
};

}

#endif

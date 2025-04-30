#ifndef wulkan_wk_DESCRIPTOR_POOL_HPP
#define wulkan_wk_DESCRIPTOR_POOL_HPP

#include "wulkan_internal.hpp"

#include <vector>
#include <stdexcept>

namespace wk {

class DescriptorPoolSize {
public:
    DescriptorPoolSize& set_type(VkDescriptorType type) { _type = type; return *this; }
    DescriptorPoolSize& set_descriptor_count(uint32_t count) { _descriptor_count = count; return *this; }

    VkDescriptorPoolSize to_vk_descriptor_pool_size() const {
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
    DescriptorPoolCreateInfo& set_flags(VkDescriptorPoolCreateFlags flags) { _flags = flags; return *this; }
    DescriptorPoolCreateInfo& set_max_sets(uint32_t max_sets) { _max_sets = max_sets; return *this; }
    DescriptorPoolCreateInfo& set_pool_sizes(const std::vector<VkDescriptorPoolSize>& pool_sizes) { _pool_sizes = pool_sizes; return *this; }

    VkDescriptorPoolCreateInfo to_vk_descriptor_pool_create_info() {
        VkDescriptorPoolCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        ci.flags = _flags;
        ci.poolSizeCount = static_cast<uint32_t>(_pool_sizes.size());
        ci.pPoolSizes = _pool_sizes.data();
        ci.maxSets = _max_sets;
        return ci;
    }
private:
    uint32_t _max_sets = 0;
    std::vector<VkDescriptorPoolSize> _pool_sizes;
    VkDescriptorPoolCreateFlags _flags;
};

class DescriptorPool {
public:
    DescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo& create_info)
        : _device(device) {
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

}

#endif

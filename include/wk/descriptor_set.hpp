#ifndef wulkan_wk_DESCRIPTOR_SET_HPP
#define wulkan_wk_DESCRIPTOR_SET_HPP

#include "wulkan_internal.hpp"

#include <stdexcept>
#include <iostream>
#include <vector>

namespace wk {

class DescriptorBufferInfo {
public:
    DescriptorBufferInfo& set_buffer(VkBuffer buffer) { _buffer = buffer; return *this; }
    DescriptorBufferInfo& set_offset(VkDeviceSize offset) { _offset = offset; return *this; }
    DescriptorBufferInfo& set_range(VkDeviceSize range) { _range = range; return *this; }

    VkDescriptorBufferInfo to_vk_descriptor_buffer_info() const {
        VkDescriptorBufferInfo bi{};
        bi.buffer = _buffer;
        bi.offset = _offset;
        bi.range = _range;
        return bi;
    }

private:
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceSize _offset = 0;
    VkDeviceSize _range = VK_WHOLE_SIZE;
};

class WriteDescriptorSet {
public:
    WriteDescriptorSet& set_dst_set(VkDescriptorSet dst_set) { _dst_set = dst_set; return *this; }
    WriteDescriptorSet& set_dst_binding(uint32_t dst_binding) { _dst_binding = dst_binding; return *this; }
    WriteDescriptorSet& set_descriptor_type(VkDescriptorType type) { _descriptor_type = type; return *this; }
    WriteDescriptorSet& set_buffer_info(const VkDescriptorBufferInfo* buffer_info) { _buffer_info = buffer_info; return *this; }
    WriteDescriptorSet& set_descriptor_count(uint32_t count) { _descriptor_count = count; return *this; }
    
    VkWriteDescriptorSet to_vk_write_descriptor_set() {
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = _dst_set;
        write.dstBinding = _dst_binding;
        write.dstArrayElement = 0;
        write.descriptorCount = _descriptor_count;
        write.descriptorType = _descriptor_type;
        write.pBufferInfo = _buffer_info;
        write.pImageInfo = nullptr;
        write.pTexelBufferView = nullptr;
        return write;
    }
private:
    VkDescriptorSet _dst_set = VK_NULL_HANDLE;
    uint32_t _dst_binding = 0;
    uint32_t _descriptor_count = 1;
    VkDescriptorType _descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    const VkDescriptorBufferInfo* _buffer_info = nullptr;
};

class DescriptorSetAllocateInfo {
public:
    DescriptorSetAllocateInfo& set_descriptor_pool(VkDescriptorPool pool) { _pool = pool; return *this; }
    DescriptorSetAllocateInfo& set_layouts(const std::vector<VkDescriptorSetLayout>& layouts) { _layouts = layouts; return *this; }

    VkDescriptorSetAllocateInfo to_vk_descriptor_set_allocate_info() {
        VkDescriptorSetAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        ai.descriptorPool = _pool;
        ai.descriptorSetCount = static_cast<uint32_t>(_layouts.size());
        ai.pSetLayouts = _layouts.data();
        return ai;
    }
private:
    VkDescriptorPool _pool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayout> _layouts{};
};

class DescriptorSet {
public:
    DescriptorSet(VkDevice device, const VkDescriptorSetAllocateInfo& ai)
        : _device(device), _pool(ai.descriptorPool)
    {
        if (vkAllocateDescriptorSets(device, &ai, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor set");
        }
    }

    ~DescriptorSet() {
        if (_handle != VK_NULL_HANDLE) {
            vkFreeDescriptorSets(_device, _pool, 1, &_handle);
        }
    }

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    DescriptorSet(DescriptorSet&& other) noexcept
        : _device(other._device), _pool(other._pool), _handle(other._handle) {
        other._device = VK_NULL_HANDLE;
        other._pool = VK_NULL_HANDLE;
        other._handle = VK_NULL_HANDLE;
    }

    DescriptorSet& operator=(DescriptorSet&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkFreeDescriptorSets(_device, _pool, 1, &_handle);
            }
            _device = other._device;
            _pool = other._pool;
            _handle = other._handle;
            other._device = VK_NULL_HANDLE;
            other._pool = VK_NULL_HANDLE;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkDescriptorSet& handle() const { return _handle; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    VkDescriptorPool _pool = VK_NULL_HANDLE;
    VkDescriptorSet _handle = VK_NULL_HANDLE;
};

}

#endif

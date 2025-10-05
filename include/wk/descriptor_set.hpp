#ifndef wulkan_wk_DESCRIPTOR_SET_HPP
#define wulkan_wk_DESCRIPTOR_SET_HPP

#include "wulkan_internal.hpp"

#include <stdexcept>
#include <iostream>
#include <vector>

namespace wk {

class DescriptorSet {
public:
    DescriptorSet() = default;
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

class DescriptorBufferInfo {
public:
    DescriptorBufferInfo& set_buffer(VkBuffer buffer) { _buffer = buffer; return *this;}
    DescriptorBufferInfo& set_offset(VkDeviceSize offset) { _offset = offset; return *this;}
    DescriptorBufferInfo& set_range(VkDeviceSize range) { _range = range; return *this;}

    VkDescriptorBufferInfo to_vk() const {
        VkDescriptorBufferInfo info{};
        info.buffer = _buffer;
        info.offset = _offset;
        info.range = _range;
        return info;
    }

private:
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceSize _offset = 0;
    VkDeviceSize _range = VK_WHOLE_SIZE;
};

class DescriptorImageInfo {
public:
    DescriptorImageInfo& set_sampler(VkSampler sampler) {
        _sampler = sampler;
        return *this;
    }
    DescriptorImageInfo& set_image_view(VkImageView view) {
        _view = view;
        return *this;
    }
    DescriptorImageInfo& set_image_layout(VkImageLayout layout) {
        _layout = layout;
        return *this;
    }

    VkDescriptorImageInfo to_vk() const {
        VkDescriptorImageInfo info{};
        info.sampler     = _sampler;
        info.imageView   = _view;
        info.imageLayout = _layout;
        return info;
    }

private:
    VkSampler _sampler = VK_NULL_HANDLE;
    VkImageView _view = VK_NULL_HANDLE;
    VkImageLayout _layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

class WriteDescriptorSet {
public:
    WriteDescriptorSet& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    WriteDescriptorSet& set_dst_set(VkDescriptorSet dst_set) { _dst_set = dst_set; return *this; }
    WriteDescriptorSet& set_dst_binding(uint32_t dst_binding) { _dst_binding = dst_binding; return *this; }
    WriteDescriptorSet& set_dst_array_element(uint32_t dst_array_element) { _dst_array_element = dst_array_element; return *this; }
    WriteDescriptorSet& set_descriptor_count(uint32_t descriptor_count) { _descriptor_count = descriptor_count; return *this; }
    WriteDescriptorSet& set_descriptor_type(VkDescriptorType descriptor_type) { _descriptor_type = descriptor_type; return *this; }
    WriteDescriptorSet& set_p_buffer_info(const VkDescriptorBufferInfo* p_buffer_info) { _p_buffer_info = p_buffer_info; return *this; }
    WriteDescriptorSet& set_p_image_info(const VkDescriptorImageInfo* p_image_info) { _p_image_info = p_image_info; return *this; }
    WriteDescriptorSet& set_p_texel_buffer_view(const VkBufferView* p_texel_buffer_view) { _p_texel_buffer_view = p_texel_buffer_view; return *this; }

    VkWriteDescriptorSet to_vk() const {
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = _p_next;
        write.dstSet = _dst_set;
        write.dstBinding = _dst_binding;
        write.dstArrayElement = _dst_array_element;
        write.descriptorCount = _descriptor_count;
        write.descriptorType = _descriptor_type;
        write.pBufferInfo = _p_buffer_info;
        write.pImageInfo = _p_image_info;
        write.pTexelBufferView = _p_texel_buffer_view;
        return write;
    }

private:
    const void* _p_next = nullptr;
    VkDescriptorSet _dst_set = VK_NULL_HANDLE;
    uint32_t _dst_binding = 0;
    uint32_t _dst_array_element = 0;
    uint32_t _descriptor_count = 1;
    VkDescriptorType _descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    const VkDescriptorBufferInfo* _p_buffer_info = nullptr;
    const VkDescriptorImageInfo* _p_image_info = nullptr;
    const VkBufferView* _p_texel_buffer_view = nullptr;
};

class DescriptorSetAllocateInfo {
public:
    DescriptorSetAllocateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    DescriptorSetAllocateInfo& set_descriptor_pool(VkDescriptorPool descriptor_pool) { _descriptor_pool = descriptor_pool; return *this; }
    DescriptorSetAllocateInfo& set_set_layouts(uint32_t count, const VkDescriptorSetLayout* p_set_layouts) { 
        _descriptor_set_count = count;
        _p_set_layouts = p_set_layouts; 
        return *this; 
    }

    VkDescriptorSetAllocateInfo to_vk() const {
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.pNext = _p_next;
        info.descriptorPool = _descriptor_pool;
        info.descriptorSetCount = _descriptor_set_count;
        info.pSetLayouts = _p_set_layouts;
        return info;
    }

private:
    const void* _p_next = nullptr;
    VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
    uint32_t _descriptor_set_count = 0;
    const VkDescriptorSetLayout* _p_set_layouts = nullptr;
};

}

#endif

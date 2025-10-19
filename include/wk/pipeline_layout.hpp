#ifndef wulkan_wk_PIPELINE_LAYOUT_HPP
#define wulkan_wk_PIPELINE_LAYOUT_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class PipelineLayout {
public:
    PipelineLayout() = default;
    PipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& ci)
        : _device(device)
    {
        if (vkCreatePipelineLayout(_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    ~PipelineLayout() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(_device, _handle, nullptr);
        }
    }

    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;

    PipelineLayout(PipelineLayout&& other) noexcept
        : _handle(other._handle), _device(other._device) {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    PipelineLayout& operator=(PipelineLayout&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkPipelineLayout& handle() const { return _handle; }
private:
    VkPipelineLayout _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

class PushConstantRange {
public:
    PushConstantRange& set_stage_flags(VkShaderStageFlags flags) { _stage_flags = flags; return *this; }
    PushConstantRange& set_offset(uint32_t offset) { _offset = offset; return *this; }
    PushConstantRange& set_size(uint32_t size) { _size = size; return *this; }

    VkPushConstantRange to_vk() {
        VkPushConstantRange pcr{};
        pcr.stageFlags = _stage_flags;
        pcr.offset = _offset;
        pcr.size = _size;
        return pcr;
    }
private:
    VkShaderStageFlags _stage_flags = 0;
    uint32_t _offset = 0;
    uint32_t _size = 0;
};

class PipelineLayoutCreateInfo {
public:
    PipelineLayoutCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineLayoutCreateInfo& set_flags(VkPipelineLayoutCreateFlags flags) { _flags = flags; return *this; }
    PipelineLayoutCreateInfo& set_set_layouts(uint32_t count, const VkDescriptorSetLayout* p_set_layouts) { 
        _set_layout_count = count; 
        _p_set_layouts = p_set_layouts; 
        return *this;
    }
    PipelineLayoutCreateInfo& set_push_constant_ranges(uint32_t count, const VkPushConstantRange* p_push_constant_ranges) {
        _push_constant_range_count = count;
        _p_push_constant_ranges = p_push_constant_ranges;
        return *this;
    }

    VkPipelineLayoutCreateInfo to_vk() const {
        VkPipelineLayoutCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.setLayoutCount = _set_layout_count;
        ci.pSetLayouts = _p_set_layouts;
        ci.pushConstantRangeCount = _push_constant_range_count;
        ci.pPushConstantRanges = _p_push_constant_ranges;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineLayoutCreateFlags _flags = 0;
    uint32_t _set_layout_count = 0;
    const VkDescriptorSetLayout* _p_set_layouts = nullptr;
    uint32_t _push_constant_range_count = 0;
    const VkPushConstantRange* _p_push_constant_ranges = nullptr;
};

}

#endif

#ifndef wulkan_wk_PIPELINE_LAYOUT_HPP
#define wulkan_wk_PIPELINE_LAYOUT_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class PushConstantRange {
public:
    PushConstantRange& set_stage_flags(VkShaderStageFlags flags) { _stage_flags = flags; return *this; }
    PushConstantRange& set_offset(uint32_t offset) { _offset = offset; return *this; }
    PushConstantRange& set_size(uint32_t size) { _size = size; return *this; }

    VkPushConstantRange to_vk_push_constant_range() {
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
    PipelineLayoutCreateInfo& set_push_constant_ranges(const std::vector<VkPushConstantRange>& ranges) {
        _push_constant_ranges = ranges;
        return *this;
    }

    PipelineLayoutCreateInfo& set_set_layouts(const std::vector<VkDescriptorSetLayout>& set_layouts) {
        _set_layouts = set_layouts;
        return *this;
    }

    VkPipelineLayoutCreateInfo to_vk_pipeline_layout_create_info() {
        VkPipelineLayoutCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        ci.setLayoutCount = static_cast<uint32_t>(_set_layouts.size());
        ci.pSetLayouts = _set_layouts.data();
        ci.pushConstantRangeCount = static_cast<uint32_t>(_push_constant_ranges.size());
        ci.pPushConstantRanges = _push_constant_ranges.data();
        return ci;
    }
private:
    std::vector<VkPushConstantRange> _push_constant_ranges{};
    std::vector<VkDescriptorSetLayout> _set_layouts{};
};

class PipelineLayout {
public:
    PipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& ci) : _device(device) {
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

}

#endif

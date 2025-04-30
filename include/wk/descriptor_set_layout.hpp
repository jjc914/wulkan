#ifndef wulkan_wk_DESCRIPTOR_SET_LAYOUT_HPP
#define wulkan_wk_DESCRIPTOR_SET_LAYOUT_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace wk {

class DescriptorSetLayoutBinding {
public:
    DescriptorSetLayoutBinding& set_binding(uint32_t binding) { _binding = binding; return *this; }
    DescriptorSetLayoutBinding& set_descriptor_type(VkDescriptorType type) { _descriptor_type = type; return *this; }
    DescriptorSetLayoutBinding& set_descriptor_count(uint32_t count) { _descriptor_count = count; return *this; }
    DescriptorSetLayoutBinding& set_stage_flags(VkShaderStageFlags flags) { _stage_flags = flags; return *this; }
    DescriptorSetLayoutBinding& set_immutable_samplers(const VkSampler* samplers) { _immutable_samplers = samplers; return *this; }

    VkDescriptorSetLayoutBinding to_vk_descriptor_set_layout_binding() {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = _binding;
        binding.descriptorType = _descriptor_type;
        binding.descriptorCount = _descriptor_count;
        binding.stageFlags = _stage_flags;
        binding.pImmutableSamplers = _immutable_samplers;
        return binding;
    }
private:
    uint32_t _binding = 0;
    VkDescriptorType _descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uint32_t _descriptor_count = 1;
    VkShaderStageFlags _stage_flags = VK_SHADER_STAGE_VERTEX_BIT;
    const VkSampler* _immutable_samplers = nullptr;
};

class DescriptorSetLayoutCreateInfo {
public:
    DescriptorSetLayoutCreateInfo& set_bindings(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
        _bindings = bindings;
        return *this;
    }

    VkDescriptorSetLayoutCreateInfo to_vk_descriptor_set_layout_create_info() {
        VkDescriptorSetLayoutCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        ci.bindingCount = static_cast<uint32_t>(_bindings.size());
        ci.pBindings = _bindings.data();
        return ci;
    }
private:
    std::vector<VkDescriptorSetLayoutBinding> _bindings{};
};

class DescriptorSetLayout {
public:
    DescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo& ci)
        : _device(device)
    {
        if (vkCreateDescriptorSetLayout(_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout");
        }
    }

    ~DescriptorSetLayout() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(_device, _handle, nullptr);
        }
    }

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
        : _device(other._device), _handle(other._handle)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyDescriptorSetLayout(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkDescriptorSetLayout& handle() const { return _handle; }
private:
    VkDescriptorSetLayout _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

}

#endif

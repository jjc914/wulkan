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
    DescriptorSetLayoutBinding& set_descriptor_type(VkDescriptorType descriptor_type) { _descriptor_type = descriptor_type; return *this; }
    DescriptorSetLayoutBinding& set_descriptor_count(uint32_t descriptor_count) { _descriptor_count = descriptor_count; return *this; }
    DescriptorSetLayoutBinding& set_stage_flags(VkShaderStageFlags stage_flags) { _stage_flags = stage_flags; return *this; }
    DescriptorSetLayoutBinding& set_immutable_samplers(const VkSampler* immutable_samplers) { 
        _immutable_samplers = immutable_samplers; 
        return *this; 
    }

    VkDescriptorSetLayoutBinding to_vk() const {
        VkDescriptorSetLayoutBinding binding_info{};
        binding_info.binding = _binding;
        binding_info.descriptorType = _descriptor_type;
        binding_info.descriptorCount = _descriptor_count;
        binding_info.stageFlags = _stage_flags;
        binding_info.pImmutableSamplers = _immutable_samplers;
        return binding_info;
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
    DescriptorSetLayoutCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    DescriptorSetLayoutCreateInfo& set_flags(VkDescriptorSetLayoutCreateFlags flags) { _flags = flags; return *this; }
    DescriptorSetLayoutCreateInfo& set_bindings(uint32_t binding_count, const VkDescriptorSetLayoutBinding* bindings) { _binding_count = binding_count; _p_bindings = bindings; return *this; }
    
    VkDescriptorSetLayoutCreateInfo to_vk() const {
        VkDescriptorSetLayoutCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.bindingCount = _binding_count;
        ci.pBindings = _p_bindings;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkDescriptorSetLayoutCreateFlags _flags = 0;
    uint32_t _binding_count = 0;
    const VkDescriptorSetLayoutBinding* _p_bindings = nullptr;
};
    

class DescriptorSetLayout {
public:
    DescriptorSetLayout() noexcept = default;
    DescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo& ci)
        : _device(device)
    {
        if (vkCreateDescriptorSetLayout(_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create descriptor set layout" << std::endl;
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

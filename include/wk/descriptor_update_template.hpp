#ifndef wulkan_wk_DESCRIPTOR_UPDATE_TEMPLATE_HPP
#define wulkan_wk_DESCRIPTOR_UPDATE_TEMPLATE_HPP

#include "wulkan_internal.hpp"

namespace wk {

class DescriptorUpdateTemplateCreateInfo {
public:
    DescriptorUpdateTemplateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_flags(VkDescriptorUpdateTemplateCreateFlags flags) { _flags = flags; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_descriptor_update_entry_count(uint32_t count) { _descriptor_update_entry_count = count; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_p_descriptor_update_entries(const VkDescriptorUpdateTemplateEntry* entries) { _p_descriptor_update_entries = entries; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_template_type(VkDescriptorUpdateTemplateType type) { _template_type = type; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_descriptor_set_layout(VkDescriptorSetLayout layout) { _descriptor_set_layout = layout; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_pipeline_bind_point(VkPipelineBindPoint bind_point) { _pipeline_bind_point = bind_point; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_pipeline_layout(VkPipelineLayout layout) { _pipeline_layout = layout; return *this; }
    DescriptorUpdateTemplateCreateInfo& set_set(uint32_t set) { _set = set; return *this; }

    VkDescriptorUpdateTemplateCreateInfo to_vk() const {
        VkDescriptorUpdateTemplateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.descriptorUpdateEntryCount = _descriptor_update_entry_count;
        ci.pDescriptorUpdateEntries = _p_descriptor_update_entries;
        ci.templateType = _template_type;
        ci.descriptorSetLayout = _descriptor_set_layout;
        ci.pipelineBindPoint = _pipeline_bind_point;
        ci.pipelineLayout = _pipeline_layout;
        ci.set = _set;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkDescriptorUpdateTemplateCreateFlags _flags = 0;
    uint32_t _descriptor_update_entry_count = 0;
    const VkDescriptorUpdateTemplateEntry* _p_descriptor_update_entries = nullptr;
    VkDescriptorUpdateTemplateType _template_type = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
    VkDescriptorSetLayout _descriptor_set_layout = VK_NULL_HANDLE;
    VkPipelineBindPoint _pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
    uint32_t _set = 0;
};

class DescriptorUpdateTemplate {
public:
    DescriptorUpdateTemplate() noexcept = default;
    DescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo& ci) 
        : _device(device)
    {
        if (vkCreateDescriptorUpdateTemplate(device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create descriptor update template" << std::endl;
        }
    }

    ~DescriptorUpdateTemplate() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyDescriptorUpdateTemplate(_device, _handle, nullptr);
        }
    }

    DescriptorUpdateTemplate(const DescriptorUpdateTemplate&) = delete;
    DescriptorUpdateTemplate& operator=(const DescriptorUpdateTemplate&) = delete;

    DescriptorUpdateTemplate(DescriptorUpdateTemplate&& other) noexcept : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    DescriptorUpdateTemplate& operator=(DescriptorUpdateTemplate&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyDescriptorUpdateTemplate(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkDescriptorUpdateTemplate& handle() const { return _handle; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkDescriptorUpdateTemplate _handle = VK_NULL_HANDLE;
};

} // namespace wk

#endif

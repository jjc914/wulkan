#ifndef wulkan_wk_RENDER_PASS_HPP
#define wulkan_wk_RENDER_PASS_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class RenderPass {
public:
    RenderPass() = default;
    RenderPass(VkDevice device, const VkRenderPassCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateRenderPass(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create render pass" << std::endl;
        }
    }

    ~RenderPass() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyRenderPass(_device, _handle, nullptr);
        }
    }

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    RenderPass(RenderPass&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    RenderPass& operator=(RenderPass&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyRenderPass(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkRenderPass& handle() const { return _handle; }
private:
    VkRenderPass _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

class AttachmentDescription {
public:
    AttachmentDescription& set_flags(VkAttachmentDescriptionFlags flags) { _flags = flags; return *this; }
    AttachmentDescription& set_format(VkFormat format) { _format = format; return *this; }
    AttachmentDescription& set_samples(VkSampleCountFlagBits samples) { _samples = samples; return *this; }
    AttachmentDescription& set_load_op(VkAttachmentLoadOp op) { _load_op = op; return *this; }
    AttachmentDescription& set_store_op(VkAttachmentStoreOp op) { _store_op = op; return *this; }
    AttachmentDescription& set_stencil_load_op(VkAttachmentLoadOp op) { _stencil_load_op = op; return *this; }
    AttachmentDescription& set_stencil_store_op(VkAttachmentStoreOp op) { _stencil_store_op = op; return *this; }
    AttachmentDescription& set_initial_layout(VkImageLayout layout) { _initial_layout = layout; return *this; }
    AttachmentDescription& set_final_layout(VkImageLayout layout) { _final_layout = layout; return *this; }

    VkAttachmentDescription to_vk() const {
        VkAttachmentDescription desc{};
        desc.flags = _flags;
        desc.format = _format;
        desc.samples = _samples;
        desc.loadOp = _load_op;
        desc.storeOp = _store_op;
        desc.stencilLoadOp = _stencil_load_op;
        desc.stencilStoreOp = _stencil_store_op;
        desc.initialLayout = _initial_layout;
        desc.finalLayout = _final_layout;
        return desc;
    }

private:
    VkAttachmentDescriptionFlags _flags = 0;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits _samples = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentLoadOp _load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp _store_op = VK_ATTACHMENT_STORE_OP_STORE;
    VkAttachmentLoadOp _stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp _stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkImageLayout _initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout _final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
};

class AttachmentReference {
public:
    AttachmentReference& set_attachment(uint32_t attachment) { _attachment = attachment; return *this; }
    AttachmentReference& set_layout(VkImageLayout layout) { _layout = layout; return *this; }

    VkAttachmentReference to_vk() const {
        VkAttachmentReference ref{};
        ref.attachment = _attachment;
        ref.layout = _layout;
        return ref;
    }

private:
    uint32_t _attachment = 0;
    VkImageLayout _layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
};

class SubpassDescription {
public:
    SubpassDescription& set_flags(VkSubpassDescriptionFlags flags) { _flags = flags; return *this; }
    SubpassDescription& set_pipeline_bind_point(VkPipelineBindPoint bind_point) { _pipeline_bind_point = bind_point; return *this; }
    SubpassDescription& set_color_attachments(uint32_t count, const VkAttachmentReference* attachments) {
        _color_attachment_count = count; _color_attachments = attachments; return *this;
    }
    SubpassDescription& set_input_attachments(uint32_t count, const VkAttachmentReference* attachments) {
        _input_attachment_count = count; _input_attachments = attachments; return *this;
    }
    SubpassDescription& set_resolve_attachments(const VkAttachmentReference* attachments) {
        _resolve_attachments = attachments; return *this;
    }
    SubpassDescription& set_depth_stencil_attachment(const VkAttachmentReference* attachment) {
        _depth_stencil_attachment = attachment; return *this;
    }
    SubpassDescription& set_preserve_attachments(const uint32_t* attachments, uint32_t count) {
        _preserve_attachments = attachments; _preserve_attachment_count = count; return *this;
    }

    VkSubpassDescription to_vk() const {
        VkSubpassDescription desc{};
        desc.flags = _flags;
        desc.pipelineBindPoint = _pipeline_bind_point;
        desc.inputAttachmentCount = _input_attachment_count;
        desc.pInputAttachments = _input_attachments;
        desc.colorAttachmentCount = _color_attachment_count;
        desc.pColorAttachments = _color_attachments;
        desc.pResolveAttachments = _resolve_attachments;
        desc.pDepthStencilAttachment = _depth_stencil_attachment;
        desc.preserveAttachmentCount = _preserve_attachment_count;
        desc.pPreserveAttachments = _preserve_attachments;
        return desc;
    }

private:
    VkSubpassDescriptionFlags _flags = 0;
    VkPipelineBindPoint _pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    const VkAttachmentReference* _input_attachments = nullptr;
    uint32_t _input_attachment_count = 0;
    const VkAttachmentReference* _color_attachments = nullptr;
    uint32_t _color_attachment_count = 0;
    const VkAttachmentReference* _resolve_attachments = nullptr;
    const VkAttachmentReference* _depth_stencil_attachment = nullptr;
    const uint32_t* _preserve_attachments = nullptr;
    uint32_t _preserve_attachment_count = 0;
};

class SubpassDependency {
public:
    SubpassDependency& set_src_subpass(uint32_t subpass) { _src_subpass = subpass; return *this; }
    SubpassDependency& set_dst_subpass(uint32_t subpass) { _dst_subpass = subpass; return *this; }
    SubpassDependency& set_src_stage_mask(VkPipelineStageFlags mask) { _src_stage_mask = mask; return *this; }
    SubpassDependency& set_dst_stage_mask(VkPipelineStageFlags mask) { _dst_stage_mask = mask; return *this; }
    SubpassDependency& set_src_access_mask(VkAccessFlags mask) { _src_access_mask = mask; return *this; }
    SubpassDependency& set_dst_access_mask(VkAccessFlags mask) { _dst_access_mask = mask; return *this; }
    SubpassDependency& set_dependency_flags(VkDependencyFlags flags) { _dependency_flags = flags; return *this; }

    VkSubpassDependency to_vk() const {
        VkSubpassDependency dep{};
        dep.srcSubpass = _src_subpass;
        dep.dstSubpass = _dst_subpass;
        dep.srcStageMask = _src_stage_mask;
        dep.dstStageMask = _dst_stage_mask;
        dep.srcAccessMask = _src_access_mask;
        dep.dstAccessMask = _dst_access_mask;
        dep.dependencyFlags = _dependency_flags;
        return dep;
    }

private:
    uint32_t _src_subpass = VK_SUBPASS_EXTERNAL;
    uint32_t _dst_subpass = 0;
    VkPipelineStageFlags _src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkPipelineStageFlags _dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkAccessFlags _src_access_mask = 0;
    VkAccessFlags _dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkDependencyFlags _dependency_flags = 0;
};

class RenderPassCreateInfo {
public:
    RenderPassCreateInfo& set_attachments(uint32_t count, const VkAttachmentDescription* attachments) {
        _attachment_count = count;
        _p_attachments = attachments;
        return *this;
    }
    RenderPassCreateInfo& set_subpasses(uint32_t count, const VkSubpassDescription* subpasses) {
        _subpass_count = count;
        _p_subpasses = subpasses;
        return *this;
    }
    RenderPassCreateInfo& set_dependencies(uint32_t count, const VkSubpassDependency* dependencies) {
        _dependency_count = count;
        _p_dependencies = dependencies;
        return *this;
    }
    RenderPassCreateInfo& set_p_next(const void* p_next) {
        _p_next = p_next;
        return *this;
    }
    RenderPassCreateInfo& set_flags(VkRenderPassCreateFlags flags) {
        _flags = flags;
        return *this;
    }

    VkRenderPassCreateInfo to_vk() const {
        VkRenderPassCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.attachmentCount = _attachment_count;
        ci.pAttachments = _p_attachments;
        ci.subpassCount = _subpass_count;
        ci.pSubpasses = _p_subpasses;
        ci.dependencyCount = _dependency_count;
        ci.pDependencies = _p_dependencies;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkRenderPassCreateFlags _flags = 0;
    uint32_t _attachment_count = 0;
    const VkAttachmentDescription* _p_attachments = nullptr;
    uint32_t _subpass_count = 0;
    const VkSubpassDescription* _p_subpasses = nullptr;
    uint32_t _dependency_count = 0;
    const VkSubpassDependency* _p_dependencies = nullptr;
};

}

#endif

#ifndef wulkan_wk_RENDER_PASS_HPP
#define wulkan_wk_RENDER_PASS_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class RenderPassCreateInfo {
public:
    RenderPassCreateInfo& set_image_format(VkFormat format) { _color_format = format; return *this; }
    RenderPassCreateInfo& set_depth_format(VkFormat format) { _depth_format = format; return *this; }
    RenderPassCreateInfo& set_samples(VkSampleCountFlagBits samples) { _samples = samples; return *this; }

    VkRenderPassCreateInfo to_vk_render_pass_create_info() {
        _attachment_description = {};
        _attachment_description.format = _color_format;
        _attachment_description.samples = _samples;
        _attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        _attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        _attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        _attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        _attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        _attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        _attachment_reference = {};
        _attachment_reference.attachment = 0;
        _attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        _subpass_description = {};
        _subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        _subpass_description.colorAttachmentCount = 1;
        _subpass_description.pColorAttachments = &_attachment_reference;

        _subpass_dependency = {};
        _subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        _subpass_dependency.dstSubpass = 0;
        _subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        _subpass_dependency.srcAccessMask = 0;
        _subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        _subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        ci.attachmentCount = 1;
        ci.pAttachments = &_attachment_description;
        ci.subpassCount = 1;
        ci.pSubpasses = &_subpass_description;
        ci.dependencyCount = 1;
        ci.pDependencies = &_subpass_dependency;

        return ci;
    }
private:
    VkFormat _color_format = VK_FORMAT_UNDEFINED;
    VkFormat _depth_format = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits _samples = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;

    VkAttachmentDescription _attachment_description{};
    VkAttachmentReference _attachment_reference{};
    VkSubpassDescription _subpass_description{};
    VkSubpassDependency _subpass_dependency{};
};

class RenderPass {
public:
    RenderPass(VkDevice device, const VkRenderPassCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateRenderPass(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass");
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

}

#endif

#ifndef wulkan_wk_RENDER_PASS_HPP
#define wulkan_wk_RENDER_PASS_HPP

#include <cstdint>
#include <iostream>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace wk {

class RenderPassCreateInfo {
public:
    RenderPassCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    RenderPassCreateInfo& set_image_format(VkFormat image_format) { _image_format = image_format; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    VkFormat _image_format = VK_FORMAT_UNDEFINED;

    friend class RenderPass;
};

class RenderPass {
public:
    RenderPass(const RenderPassCreateInfo& ci) {
        VkAttachmentDescription color_attachment_description{};
        color_attachment_description.format = ci._image_format;
        color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_reference{};
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass_description{};
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_attachment_reference;
        
        VkSubpassDependency subpass_dependency{};
        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.dstSubpass = 0;
        subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.srcAccessMask = 0;
        subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        VkRenderPassCreateInfo vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vkci.attachmentCount = 1;
        vkci.pAttachments = &color_attachment_description;
        vkci.subpassCount = 1;
        vkci.pSubpasses = &subpass_description;
        vkci.dependencyCount = 1;
        vkci.pDependencies = &subpass_dependency;

        if (vkCreateRenderPass(ci._device, &vkci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create render pass" << std::endl;
        }

        _device = ci._device;

        std::clog << "created render pass" << std::endl;
    }

    ~RenderPass() {
        vkDestroyRenderPass(_device, _handle, nullptr);
    }

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    VkRenderPass handle() const { return _handle; }
private:
    VkRenderPass _handle;
    VkDevice _device;
};

}

#endif
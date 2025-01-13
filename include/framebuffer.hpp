#ifndef wulkan_wk_FRAMEBUFFER_HPP
#define wulkan_wk_FRAMEBUFFER_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace wk {

class FramebufferCreateInfo {
public:
    FramebufferCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    FramebufferCreateInfo& set_image_view(VkImageView image_view) { _image_view = image_view; return *this; }
    FramebufferCreateInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    FramebufferCreateInfo& set_extent(VkExtent2D extent) { _extent = extent; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    VkImageView _image_view = VK_NULL_HANDLE;
    VkExtent2D _extent{};

    friend class Framebuffer;
};

class Framebuffer {
public:
    Framebuffer(const FramebufferCreateInfo& ci) {
        std::vector<VkImageView> attachments(1);
        attachments[0] = ci._image_view;

        VkFramebufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = ci._render_pass;
        create_info.attachmentCount = 1;
        create_info.pAttachments = attachments.data();
        create_info.width = ci._extent.width;
        create_info.height = ci._extent.height;
        create_info.layers = 1;
        
        if (vkCreateFramebuffer(ci._device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create framebuffer" << std::endl;
        }

        _device = ci._device;

        std::clog << "created framebuffer" << std::endl;
    }

    ~Framebuffer() {
        vkDestroyFramebuffer(_device, _handle, nullptr);
    }

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    VkFramebuffer handle() const { return _handle; }
private:
    VkFramebuffer _handle;
    VkDevice _device;
};

}

#endif
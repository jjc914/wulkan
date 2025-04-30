#ifndef wulkan_wk_FRAMEBUFFER_HPP
#define wulkan_wk_FRAMEBUFFER_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class FramebufferCreateInfo {
public:
    FramebufferCreateInfo& set_image_view(VkImageView image_view) { 
        _attachments.clear();
        _attachments.push_back(image_view);
        return *this; 
    }
    FramebufferCreateInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    FramebufferCreateInfo& set_extent(VkExtent2D extent) { _extent = extent; return *this; }

    VkFramebufferCreateInfo to_vk_framebuffer_create_info() {
        VkFramebufferCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        ci.renderPass = _render_pass;
        ci.attachmentCount = static_cast<uint32_t>(_attachments.size());
        ci.pAttachments = _attachments.data();
        ci.width = _extent.width;
        ci.height = _extent.height;
        ci.layers = 1;
        return ci;
    }
private:
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    std::vector<VkImageView> _attachments{};
    VkExtent2D _extent{};
};

class Framebuffer {
public:
    Framebuffer(VkDevice device, const VkFramebufferCreateInfo& create_info)
        : _device(device)
    {
        if (vkCreateFramebuffer(_device, &create_info, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer");
        }
    }

    ~Framebuffer() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(_device, _handle, nullptr);
        }
    }

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    Framebuffer& operator=(Framebuffer&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkFramebuffer& handle() const { return _handle; }
private:
    VkFramebuffer _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

}

#endif

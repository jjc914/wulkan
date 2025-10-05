#ifndef wulkan_wk_FRAMEBUFFER_HPP
#define wulkan_wk_FRAMEBUFFER_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class Framebuffer {
public:
    Framebuffer() = default;
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

class FramebufferCreateInfo {
public:
    FramebufferCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    FramebufferCreateInfo& set_flags(VkFramebufferCreateFlags flags) { _flags = flags; return *this; }
    FramebufferCreateInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    FramebufferCreateInfo& set_attachments(uint32_t attachment_count, const VkImageView* p_attachments) {
        _attachment_count = attachment_count;
        _p_attachments = p_attachments;
        return *this;
    }
    FramebufferCreateInfo& set_extent(VkExtent2D extent) { _extent = extent; return *this; }
    FramebufferCreateInfo& set_layers(uint32_t layers) { _layers = layers; return *this; }

    VkFramebufferCreateInfo to_vk() const {
        VkFramebufferCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.renderPass = _render_pass;
        ci.attachmentCount = _attachment_count;
        ci.pAttachments = _p_attachments;
        ci.width = _extent.width;
        ci.height = _extent.height;
        ci.layers = _layers;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkFramebufferCreateFlags _flags = 0;
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    uint32_t _attachment_count = 0;
    const VkImageView* _p_attachments = nullptr;
    VkExtent2D _extent{};
    uint32_t _layers = 1;
};

}

#endif

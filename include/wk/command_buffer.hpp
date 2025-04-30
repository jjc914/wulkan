#ifndef wulkan_wk_COMMAND_BUFFER_HPP
#define wulkan_wk_COMMAND_BUFFER_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class BufferCopy {
public:
    BufferCopy& set_src_offset(VkDeviceSize src_offset) { _src_offset = src_offset; return *this; }
    BufferCopy& set_dst_offset(VkDeviceSize dst_offset) { _dst_offset = dst_offset; return *this; }
    BufferCopy& set_size(VkDeviceSize size) { _size = size; return *this; }

    VkBufferCopy to_vk_buffer_copy() {
        VkBufferCopy copy{};
        copy.srcOffset = _src_offset;
        copy.dstOffset = _dst_offset;
        copy.size = _size;
        return copy;
    }
private:
    VkDeviceSize _src_offset = 0;
    VkDeviceSize _dst_offset = 0;
    VkDeviceSize _size = 0;
};

class CommandBufferBeginInfo {
public:
    CommandBufferBeginInfo& set_flags(VkCommandBufferUsageFlags flags) { _flags = flags; return *this; }
    CommandBufferBeginInfo& set_inheritance_info(const VkCommandBufferInheritanceInfo* inheritance_info) { 
        _inheritance_info = inheritance_info; 
        return *this; 
    }

    VkCommandBufferBeginInfo to_vk_command_buffer_begin_info() {
        VkCommandBufferBeginInfo bi{};
        bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bi.flags = _flags;
        bi.pInheritanceInfo = _inheritance_info;
        return bi;
    }
private:
    VkCommandBufferUsageFlags _flags = 0;
    const VkCommandBufferInheritanceInfo* _inheritance_info = nullptr;
};

class RenderPassBeginInfo {
public:
    RenderPassBeginInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this;}
    RenderPassBeginInfo& set_framebuffer(VkFramebuffer framebuffer) { _framebuffer = framebuffer; return *this;}
    RenderPassBeginInfo& set_render_area(VkRect2D render_area) { _render_area = render_area; return *this;}
    RenderPassBeginInfo& set_clear_values(const std::vector<VkClearValue>& clear_values) { _clear_values = clear_values; return *this;}

    VkRenderPassBeginInfo to_vk_render_pass_begin_info() {
        VkRenderPassBeginInfo bi{};
        bi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        bi.renderPass = _render_pass;
        bi.framebuffer = _framebuffer;
        bi.renderArea = _render_area;
        bi.clearValueCount = static_cast<uint32_t>(_clear_values.size());
        bi.pClearValues = _clear_values.data();
        return bi;
    }

private:
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    VkFramebuffer _framebuffer = VK_NULL_HANDLE;
    VkRect2D _render_area{};
    std::vector<VkClearValue> _clear_values{};
};

class CommandBufferAllocateInfo {
public:
    CommandBufferAllocateInfo& set_command_pool(VkCommandPool command_pool) { 
        _command_pool = command_pool; 
        return *this;
    }

    VkCommandBufferAllocateInfo to_vk_command_buffer_allocate_info() {
        VkCommandBufferAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        ai.commandPool = _command_pool;
        ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        ai.commandBufferCount = 1;
        return ai;
    }

private:
    VkCommandPool _command_pool = VK_NULL_HANDLE;
};

class CommandBuffer {
public:
    CommandBuffer(VkDevice device, const VkCommandBufferAllocateInfo& ai)
        : _device(device), _command_pool(ai.commandPool)
    {
        if (vkAllocateCommandBuffers(_device, &ai, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffer");
        }
    }

    ~CommandBuffer() {
        if (_handle != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(_device, _command_pool, 1, &_handle);
        }
    }

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    CommandBuffer(CommandBuffer&& other) noexcept
        : _handle(other._handle),
          _device(other._device),
          _command_pool(other._command_pool)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
        other._command_pool = VK_NULL_HANDLE;
    }

    CommandBuffer& operator=(CommandBuffer&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkFreeCommandBuffers(_device, _command_pool, 1, &_handle);
            }
            _handle = other._handle;
            _device = other._device;
            _command_pool = other._command_pool;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
            other._command_pool = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkCommandBuffer& handle() const { return _handle; }
private:
    VkCommandBuffer _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkCommandPool _command_pool = VK_NULL_HANDLE;
};

}

#endif

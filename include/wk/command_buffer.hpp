#ifndef wulkan_wk_COMMAND_BUFFER_HPP
#define wulkan_wk_COMMAND_BUFFER_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class CommandBuffer {
public:
    CommandBuffer() = default;
    CommandBuffer(VkDevice device, const VkCommandBufferAllocateInfo& ai)
        : _device(device), _command_pool(ai.commandPool)
    {
        if (vkAllocateCommandBuffers(_device, &ai, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to allocate command buffer" << std::endl;
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
                vkDeviceWaitIdle(_device);
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

class BufferCopy {
public:
    BufferCopy& set_src_offset(VkDeviceSize src_offset) {_src_offset = src_offset; return *this; }
    BufferCopy& set_dst_offset(VkDeviceSize dst_offset) {_dst_offset = dst_offset; return *this; }
    BufferCopy& set_size(VkDeviceSize size) {_size = size; return *this; }

    VkBufferCopy to_vk() {
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
    CommandBufferBeginInfo& set_inheritance_info(const VkCommandBufferInheritanceInfo* info) {
        _inheritance_info = info; return *this;
    }
    CommandBufferBeginInfo& set_p_next(const void* pnext) { _p_next = pnext; return *this; }

    VkCommandBufferBeginInfo to_vk() const {
        VkCommandBufferBeginInfo bi{};
        bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bi.pNext = _p_next;
        bi.flags = _flags;
        bi.pInheritanceInfo = _inheritance_info;
        
        return bi;
    }

private:
    const void* _p_next = nullptr;
    VkCommandBufferUsageFlags _flags = 0;
    const VkCommandBufferInheritanceInfo* _inheritance_info = nullptr;
};

class ClearValue {
public:
    ClearValue& set_color(float r, float g, float b, float a = 1.0f) {
        _value.color = {{ r, g, b, a }};
        return *this;
    }
    ClearValue& set_depth_stencil(float depth, uint32_t stencil = 0) {
        _value.depthStencil = { depth, stencil };
        return *this;
    }
    
    VkClearValue to_vk() const {
        return _value;
    }

private:
    VkClearValue _value{};
};

class RenderPassBeginInfo {
public:
    RenderPassBeginInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    RenderPassBeginInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    RenderPassBeginInfo& set_framebuffer(VkFramebuffer framebuffer) { _framebuffer = framebuffer; return *this; }
    RenderPassBeginInfo& set_render_area(VkRect2D render_area) { _render_area = render_area; return *this; }
    RenderPassBeginInfo& set_clear_values(uint32_t count, const VkClearValue* clear_values) {
        _p_clear_values = clear_values; 
        _clear_value_count = count; 
        return *this; 
    }
    
    VkRenderPassBeginInfo to_vk() const {
        VkRenderPassBeginInfo bi{};
        bi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        bi.pNext = _p_next;
        bi.renderPass = _render_pass;
        bi.framebuffer = _framebuffer;
        bi.renderArea = _render_area;
        bi.clearValueCount = _clear_value_count;
        bi.pClearValues = _p_clear_values;
        return bi;
    }

private:
    const void* _p_next = nullptr;
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    VkFramebuffer _framebuffer = VK_NULL_HANDLE;
    VkRect2D _render_area{};
    const VkClearValue* _p_clear_values = nullptr;
    uint32_t _clear_value_count = 0;
};

class CommandBufferAllocateInfo {
public:
    CommandBufferAllocateInfo& set_command_pool(VkCommandPool command_pool) { _command_pool = command_pool; return *this; }
    CommandBufferAllocateInfo& set_level(VkCommandBufferLevel level) { _level = level; return *this; }
    CommandBufferAllocateInfo& set_command_buffer_count(uint32_t count) { _command_buffer_count = count; return *this; }
    CommandBufferAllocateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }

    VkCommandBufferAllocateInfo to_vk() const {
        VkCommandBufferAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        ai.pNext = _p_next;
        ai.commandPool = _command_pool;
        ai.level = _level;
        ai.commandBufferCount = _command_buffer_count;
        return ai;
    }

private:
    const void* _p_next = nullptr;
    VkCommandPool _command_pool = VK_NULL_HANDLE;
    VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    uint32_t _command_buffer_count = 1;
};

}

#endif

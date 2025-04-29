#ifndef wulkan_wk_COMMAND_BUFFER_HPP
#define wulkan_wk_COMMAND_BUFFER_HPP

#include "wulkan_internal.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>

namespace wk {

class CommandBufferAllocateInfo {
public:
    CommandBufferAllocateInfo& set_command_pool(VkCommandPool command_pool) { 
        _command_pool = command_pool; 
        return *this;
    }

    VkCommandBufferAllocateInfo to_vk_command_buffer_allocate_info() {
        _allocate_info = {};
        _allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        _allocate_info.commandPool = _command_pool;
        _allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        _allocate_info.commandBufferCount = 1;
        return _allocate_info;
    }

private:
    VkCommandPool _command_pool = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo _allocate_info{};
};

class CommandBuffer {
public:
    CommandBuffer(VkDevice device, const VkCommandBufferAllocateInfo& ai)
        : _device(device), _command_pool(ai.commandPool)
    {
        if (vkAllocateCommandBuffers(_device, &ai, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to allocate command buffer" << std::endl;
            std::exit(-1);
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

    void Reset() {
        vkResetCommandBuffer(_handle, 0);
    }

    void BeginRecord(VkCommandBufferUsageFlags flags = 0) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = flags;

        if (vkBeginCommandBuffer(_handle, &begin_info) != VK_SUCCESS) {
            std::cerr << "failed to begin recording command buffer" << std::endl;
        }
    }

    void EndRecord() {
        if (vkEndCommandBuffer(_handle) != VK_SUCCESS) {
            std::cerr << "failed to record command buffer" << std::endl;
        }
    }

    void BeginRenderPass(VkFramebuffer framebuffer, VkRenderPass render_pass, VkExtent2D extent, VkClearValue clear_color) {
        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_pass;
        render_pass_info.framebuffer = framebuffer;
        render_pass_info.renderArea.offset = { 0, 0 };
        render_pass_info.renderArea.extent = extent;
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clear_color;
        
        vkCmdBeginRenderPass(_handle, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void EndRenderPass() {
        vkCmdEndRenderPass(_handle);
    }

    void BindPipeline(VkPipeline pipeline) {
        vkCmdBindPipeline(_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }

    void SetViewport(VkViewport viewport) {
        vkCmdSetViewport(_handle, 0, 1, &viewport);
    }

    void SetScissor(VkRect2D scissor) {
        vkCmdSetScissor(_handle, 0, 1, &scissor);
    }

    void CopyBuffer(VkBuffer source, VkBuffer dest, VkDeviceSize size) {
        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size = size;
        vkCmdCopyBuffer(_handle, source, dest, 1, &copy_region);
    }

    void PushConstants(VkPipelineLayout layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, void* values) {
        vkCmdPushConstants(_handle, layout, stage_flags, offset, size, values);
    }

    VkCommandBuffer handle() const { return _handle; }
private:
    VkCommandBuffer _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkCommandPool _command_pool = VK_NULL_HANDLE;
};

}

#endif

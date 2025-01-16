#ifndef wulkan_wk_COMMAND_BUFFER_HPP
#define wulkan_wk_COMMAND_BUFFER_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace wk {
class CommandBufferCreateInfo {
public:
    CommandBufferCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    CommandBufferCreateInfo& set_command_pool(VkCommandPool command_pool) { _command_pool = command_pool; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    VkCommandPool _command_pool = VK_NULL_HANDLE;

    friend class CommandBuffer;
};

class CommandBuffer {
public:
    CommandBuffer(const CommandBufferCreateInfo& ci) {
        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.commandPool = ci._command_pool;
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandBufferCount = 1;
        
        if(vkAllocateCommandBuffers(ci._device, &allocate_info, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to allocate command buffer" << std::endl;
        }

        _device = ci._device;
        _command_pool = ci._command_pool;

        std::clog << "allocated command buffer" << std::endl;
    }

    ~CommandBuffer() {
        vkFreeCommandBuffers(_device, _command_pool, 1, &_handle);
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
        if(vkEndCommandBuffer(_handle) != VK_SUCCESS) {
            std::cerr << "failed to record command buffer" << std::endl;
        }
    }

    void BeginRenderPass(VkFramebuffer framebuffer,  VkRenderPass render_pass, VkExtent2D extent, VkClearValue clear_color) {
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
        VkBufferCopy vertex_copy_region{};
        vertex_copy_region.srcOffset = 0;
        vertex_copy_region.dstOffset = 0;
        vertex_copy_region.size = size;
        vkCmdCopyBuffer(_handle, source, dest, 1, &vertex_copy_region);
    }

    VkCommandBuffer handle() const { return _handle; }
private:
    VkCommandBuffer _handle;
    VkDevice _device;
    VkCommandPool _command_pool;
};

}

#endif
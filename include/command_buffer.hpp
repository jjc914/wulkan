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

class ViewportInfo {
public:
    ViewportInfo& set_x(float x) { _x = x; return *this; }
    ViewportInfo& set_y(float y) { _y = y; return *this; }
    ViewportInfo& set_width(float width) { _width = width; return *this; }
    ViewportInfo& set_height(float height) { _height = height; return *this; }
    ViewportInfo& set_min_depth(float min_depth) { _min_depth = min_depth; return *this; }
    ViewportInfo& set_max_depth(float max_depth) { _max_depth = max_depth; return *this; }
private:
    float _x;
    float _y;
    float _width;
    float _height;
    float _min_depth;
    float _max_depth;

    friend class CommandBuffer;
};

class ScissorInfo {
public:
    ScissorInfo& set_offset(VkOffset2D offset) { _offset = offset; return *this; }
    ScissorInfo& set_extent(VkExtent2D extent) { _extent = extent; return *this; }
private:
    VkOffset2D _offset;
    VkExtent2D _extent;

    friend class CommandBuffer;
};

class CommandBuffer {
public:
    CommandBuffer(const CommandBufferCreateInfo& ci) {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        
        allocateInfo.commandPool = ci._command_pool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;
        
        if(vkAllocateCommandBuffers(ci._device, &allocateInfo, &_handle) != VK_SUCCESS) {
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

    void BeginRecord() {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

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

    void SetViewport(ViewportInfo i) {
        VkViewport viewport{};
        viewport.x = i._x;
        viewport.y = i._y;
        viewport.width = i._width;
        viewport.height = i._height;
        viewport.minDepth = i._min_depth;
        viewport.maxDepth = i._max_depth;
        vkCmdSetViewport(_handle, 0, 1, &viewport);
    }

    void SetScissor(ScissorInfo i) {
        VkRect2D scissor{};
        scissor.offset = i._offset;
        scissor.extent = i._extent;
        vkCmdSetScissor(_handle, 0, 1, &scissor);
    }

    VkCommandBuffer handle() const { return _handle; }
private:
    VkCommandBuffer _handle;
    VkDevice _device;
    VkCommandPool _command_pool;
};

}

#endif
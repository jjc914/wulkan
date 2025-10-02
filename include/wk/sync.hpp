#ifndef wulkan_WK_SYNC_HPP
#define wulkan_WK_SYNC_HPP

#include "wk/wulkan_internal.hpp"

namespace wk {

class MemoryBarrier {
public:
    MemoryBarrier& set_p_next(const void* p){ _p_next = p; return *this; }
    MemoryBarrier& set_src_access(VkAccessFlags v){ _src = v; return *this; }
    MemoryBarrier& set_dst_access(VkAccessFlags v){ _dst = v; return *this; }

    VkMemoryBarrier to_vk() const {
        VkMemoryBarrier b{};
        b.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        b.pNext = _p_next;
        b.srcAccessMask = _src;
        b.dstAccessMask = _dst;
        return b;
    }

private:
    const void* _p_next = nullptr;
    VkAccessFlags _src = 0, _dst = 0;
};

class BufferMemoryBarrier {
public:
    BufferMemoryBarrier& set_p_next(const void* p){ _p_next = p; return *this; }
    BufferMemoryBarrier& set_src_access(VkAccessFlags v){ _src = v; return *this; }
    BufferMemoryBarrier& set_dst_access(VkAccessFlags v){ _dst = v; return *this; }
    BufferMemoryBarrier& set_src_qfi(uint32_t i){ _src_qfi = i; return *this; }
    BufferMemoryBarrier& set_dst_qfi(uint32_t i){ _dst_qfi = i; return *this; }
    BufferMemoryBarrier& set_buffer(VkBuffer b){ _buf = b; return *this; }
    BufferMemoryBarrier& set_offset(VkDeviceSize o){ _offset = o; return *this; }
    BufferMemoryBarrier& set_size(VkDeviceSize s){ _size = s; return *this; }

    VkBufferMemoryBarrier to_vk() const {
        VkBufferMemoryBarrier b{};
        b.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        b.pNext = _p_next;
        b.srcAccessMask = _src;
        b.dstAccessMask = _dst;
        b.srcQueueFamilyIndex = _src_qfi;
        b.dstQueueFamilyIndex = _dst_qfi;
        b.buffer = _buf;
        b.offset = _offset;
        b.size = _size;
        return b;
    }

private:
    const void* _p_next = nullptr;
    VkAccessFlags _src = 0, _dst = 0;
    uint32_t _src_qfi = VK_QUEUE_FAMILY_IGNORED;
    uint32_t _dst_qfi = VK_QUEUE_FAMILY_IGNORED;
    VkBuffer _buf = VK_NULL_HANDLE;
    VkDeviceSize _offset = 0, _size = VK_WHOLE_SIZE;
};

class ImageMemoryBarrier {
public:
    ImageMemoryBarrier& set_p_next(const void* p){ _p_next = p; return *this; }
    ImageMemoryBarrier& set_src_access(VkAccessFlags v){ _src = v; return *this; }
    ImageMemoryBarrier& set_dst_access(VkAccessFlags v){ _dst = v; return *this; }
    ImageMemoryBarrier& set_old_layout(VkImageLayout v){ _old = v; return *this; }
    ImageMemoryBarrier& set_new_layout(VkImageLayout v){ _new = v; return *this; }
    ImageMemoryBarrier& set_src_qfi(uint32_t i){ _src_qfi = i; return *this; }
    ImageMemoryBarrier& set_dst_qfi(uint32_t i){ _dst_qfi = i; return *this; }
    ImageMemoryBarrier& set_image(VkImage img){ _img = img; return *this; }
    ImageMemoryBarrier& set_aspect(VkImageAspectFlags a){ _aspect = a; return *this; }
    ImageMemoryBarrier& set_levels(uint32_t base, uint32_t count){ _baseLevel = base; _levelCount = count; return *this; }
    ImageMemoryBarrier& set_layers(uint32_t base, uint32_t count){ _baseLayer = base; _layerCount = count; return *this; }

    VkImageMemoryBarrier to_vk() const {
        VkImageMemoryBarrier b{};
        b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        b.pNext = _p_next;
        b.srcAccessMask = _src;
        b.dstAccessMask = _dst;
        b.oldLayout = _old;
        b.newLayout = _new;
        b.srcQueueFamilyIndex = _src_qfi;
        b.dstQueueFamilyIndex = _dst_qfi;
        b.image = _img;
        b.subresourceRange.aspectMask = _aspect;
        b.subresourceRange.baseMipLevel = _baseLevel;
        b.subresourceRange.levelCount = _levelCount;
        b.subresourceRange.baseArrayLayer = _baseLayer;
        b.subresourceRange.layerCount = _layerCount;
        return b;
    }

private:
    const void* _p_next = nullptr;
    VkAccessFlags _src = 0, _dst = 0;
    VkImageLayout _old = VK_IMAGE_LAYOUT_UNDEFINED, _new = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t _src_qfi = VK_QUEUE_FAMILY_IGNORED, _dst_qfi = VK_QUEUE_FAMILY_IGNORED;
    VkImage _img = VK_NULL_HANDLE;
    VkImageAspectFlags _aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    uint32_t _baseLevel = 0, _levelCount = 1, _baseLayer = 0, _layerCount = 1;
};

class MemoryBarrier2 {
public:
    MemoryBarrier2& set_p_next(const void* p){ _p_next = p; return *this; }
    MemoryBarrier2& set_src_stage(VkPipelineStageFlags2 v){ _src_stage = v; return *this; }
    MemoryBarrier2& set_dst_stage(VkPipelineStageFlags2 v){ _dst_stage = v; return *this; }
    MemoryBarrier2& set_src_access(VkAccessFlags2 v){ _src_access = v; return *this; }
    MemoryBarrier2& set_dst_access(VkAccessFlags2 v){ _dst_access = v; return *this; }

    VkMemoryBarrier2 to_vk() const {
        VkMemoryBarrier2 b{};
        b.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
        b.pNext = _p_next;
        b.srcStageMask = _src_stage;
        b.dstStageMask = _dst_stage;
        b.srcAccessMask = _src_access;
        b.dstAccessMask = _dst_access;
        return b;
    }
private:
    const void* _p_next = nullptr;
    VkPipelineStageFlags2 _src_stage = 0, _dst_stage = 0;
    VkAccessFlags2 _src_access = 0, _dst_access = 0;
};

class BufferMemoryBarrier2 {
public:
    BufferMemoryBarrier2& set_p_next(const void* p){ _p_next = p; return *this; }
    BufferMemoryBarrier2& set_src_stage(VkPipelineStageFlags2 v){ _src_stage = v; return *this; }
    BufferMemoryBarrier2& set_dst_stage(VkPipelineStageFlags2 v){ _dst_stage = v; return *this; }
    BufferMemoryBarrier2& set_src_access(VkAccessFlags2 v){ _src_access = v; return *this; }
    BufferMemoryBarrier2& set_dst_access(VkAccessFlags2 v){ _dst_access = v; return *this; }
    BufferMemoryBarrier2& set_src_qfi(uint32_t i){ _src_qfi = i; return *this; }
    BufferMemoryBarrier2& set_dst_qfi(uint32_t i){ _dst_qfi = i; return *this; }
    BufferMemoryBarrier2& set_buffer(VkBuffer b){ _buf=b; return *this; }
    BufferMemoryBarrier2& set_offset(VkDeviceSize o){ _offset=o; return *this; }
    BufferMemoryBarrier2& set_size(VkDeviceSize s){ _size=s; return *this; }
 
    VkBufferMemoryBarrier2 to_vk() const {
        VkBufferMemoryBarrier2 b{};
        b.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
        b.pNext = _p_next;
        b.srcStageMask = _src_stage;
        b.dstStageMask = _dst_stage;
        b.srcAccessMask = _src_access;
        b.dstAccessMask = _dst_access;
        b.srcQueueFamilyIndex = _src_qfi;
        b.dstQueueFamilyIndex = _dst_qfi;
        b.buffer = _buf;
        b.offset = _offset;
        b.size = _size;
        return b;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineStageFlags2 _src_stage = 0, _dst_stage = 0;
    VkAccessFlags2 _src_access = 0, _dst_access = 0;
    uint32_t _src_qfi = VK_QUEUE_FAMILY_IGNORED, _dst_qfi = VK_QUEUE_FAMILY_IGNORED;
    VkBuffer _buf = VK_NULL_HANDLE; VkDeviceSize _offset = 0, _size = VK_WHOLE_SIZE;
};

class ImageMemoryBarrier2 {
public:
    ImageMemoryBarrier2& set_p_next(const void* p){ _p_next = p; return *this; }
    ImageMemoryBarrier2& set_src_stage(VkPipelineStageFlags2 v){ _src_stage = v; return *this; }
    ImageMemoryBarrier2& set_dst_stage(VkPipelineStageFlags2 v){ _dst_stage = v; return *this; }
    ImageMemoryBarrier2& set_src_access(VkAccessFlags2 v){ _src_access = v; return *this; }
    ImageMemoryBarrier2& set_dst_access(VkAccessFlags2 v){ _dst_access = v; return *this; }
    ImageMemoryBarrier2& set_old_layout(VkImageLayout v){ _old = v; return *this; }
    ImageMemoryBarrier2& set_new_layout(VkImageLayout v){ _new = v; return *this; }
    ImageMemoryBarrier2& set_src_qfi(uint32_t i){ _src_qfi = i; return *this; }
    ImageMemoryBarrier2& set_dst_qfi(uint32_t i){ _dst_qfi = i; return *this; }
    ImageMemoryBarrier2& set_image(VkImage img){ _img = img; return *this; }
    ImageMemoryBarrier2& set_aspect(VkImageAspectFlags a){ _aspect = a; return *this; }
    ImageMemoryBarrier2& set_levels(uint32_t base, uint32_t count){ _baseLevel=base; _levelCount=count; return *this; }
    ImageMemoryBarrier2& set_layers(uint32_t base, uint32_t count){ _baseLayer=base; _layerCount=count; return *this; }

    VkImageMemoryBarrier2 to_vk() const {
        VkImageMemoryBarrier2 b{};
        b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        b.pNext = _p_next;
        b.srcStageMask = _src_stage;
        b.dstStageMask = _dst_stage;
        b.srcAccessMask = _src_access;
        b.dstAccessMask = _dst_access;
        b.oldLayout = _old;
        b.newLayout = _new;
        b.srcQueueFamilyIndex = _src_qfi;
        b.dstQueueFamilyIndex = _dst_qfi;
        b.image = _img;
        b.subresourceRange.aspectMask = _aspect;
        b.subresourceRange.baseMipLevel = _baseLevel;
        b.subresourceRange.levelCount = _levelCount;
        b.subresourceRange.baseArrayLayer = _baseLayer;
        b.subresourceRange.layerCount = _layerCount;
        return b;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineStageFlags2 _src_stage = 0, _dst_stage = 0;
    VkAccessFlags2 _src_access = 0, _dst_access = 0;
    VkImageLayout _old = VK_IMAGE_LAYOUT_UNDEFINED, _new = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t _src_qfi = VK_QUEUE_FAMILY_IGNORED, _dst_qfi = VK_QUEUE_FAMILY_IGNORED;
    VkImage _img = VK_NULL_HANDLE;
    VkImageAspectFlags _aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    uint32_t _baseLevel = 0, _levelCount = 1, _baseLayer = 0, _layerCount = 1;
};

class DependencyInfo {
public:
    DependencyInfo& set_p_next(const void* p){ _p_next = p; return *this; }
    DependencyInfo& set_memory_barriers(uint32_t c, const VkMemoryBarrier2* p){ _mb_c=c; _mb_p=p; return *this; }
    DependencyInfo& set_buffer_barriers(uint32_t c, const VkBufferMemoryBarrier2* p){ _bb_c=c; _bb_p=p; return *this; }
    DependencyInfo& set_image_barriers(uint32_t c, const VkImageMemoryBarrier2* p){ _ib_c=c; _ib_p=p; return *this; }

    VkDependencyInfo to_vk() const {
        VkDependencyInfo d{};
        d.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        d.pNext = _p_next;
        d.memoryBarrierCount = _mb_c; d.pMemoryBarriers = _mb_p;
        d.bufferMemoryBarrierCount = _bb_c; d.pBufferMemoryBarriers = _bb_p;
        d.imageMemoryBarrierCount = _ib_c; d.pImageMemoryBarriers = _ib_p;
        return d;
    }

private:
    const void* _p_next = nullptr;
    uint32_t _mb_c=0,_bb_c=0,_ib_c=0;
    const VkMemoryBarrier2* _mb_p=nullptr;
    const VkBufferMemoryBarrier2* _bb_p=nullptr;
    const VkImageMemoryBarrier2* _ib_p=nullptr;
};

} // namespace wk

#endif // wulkan_WK_SYNC_HPP

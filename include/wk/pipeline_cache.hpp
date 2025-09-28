#ifndef wulkan_wk_PIPELINE_CACHE_HPP
#define wulkan_wk_PIPELINE_CACHE_HPP

#include "wulkan_internal.hpp"

namespace wk {

class PipelineCacheCreateInfo {
public:
    PipelineCacheCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineCacheCreateInfo& set_flags(VkPipelineCacheCreateFlags flags) { _flags = flags; return *this; }
    PipelineCacheCreateInfo& set_initial_data_size(size_t size) { _initial_data_size = size; return *this; }
    PipelineCacheCreateInfo& set_p_initial_data(const void* data) { _p_initial_data = data; return *this; }

    VkPipelineCacheCreateInfo to_vk() const {
        VkPipelineCacheCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.initialDataSize = _initial_data_size;
        ci.pInitialData = _p_initial_data;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineCacheCreateFlags _flags = 0;
    size_t _initial_data_size = 0;
    const void* _p_initial_data = nullptr;
};

class PipelineCache {
public:
    PipelineCache() noexcept = default;
    PipelineCache(VkDevice device, const VkPipelineCacheCreateInfo& ci) 
        : _device(device)
    {
        if (vkCreatePipelineCache(device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create pipeline cache" << std::endl;
        }
    }

    ~PipelineCache() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyPipelineCache(_device, _handle, nullptr);
        }
    }

    PipelineCache(const PipelineCache&) = delete;
    PipelineCache& operator=(const PipelineCache&) = delete;

    PipelineCache(PipelineCache&& other) noexcept : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    PipelineCache& operator=(PipelineCache&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyPipelineCache(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkPipelineCache& handle() const { return _handle; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkPipelineCache _handle = VK_NULL_HANDLE;
};

} // namespace wk

#endif

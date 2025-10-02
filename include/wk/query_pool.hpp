#ifndef wulkan_wk_QUERY_POOL_HPP
#define wulkan_wk_QUERY_POOL_HPP

#include "wulkan_internal.hpp"

namespace wk {

class QueryPool {
public:
    QueryPool() = default;
    QueryPool(VkDevice device, const VkQueryPoolCreateInfo& ci) :
        _device(device)
    {
        if (vkCreateQueryPool(device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create query pool" << std::endl;
        }
    }

    ~QueryPool() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyQueryPool(_device, _handle, nullptr);
        }
    }

    QueryPool(const QueryPool&) = delete;
    QueryPool& operator=(const QueryPool&) = delete;

    QueryPool(QueryPool&& other) noexcept : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    QueryPool& operator=(QueryPool&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyQueryPool(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkQueryPool& handle() const { return _handle; }

private:
    VkQueryPool _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

class QueryPoolCreateInfo {
public:
    QueryPoolCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    QueryPoolCreateInfo& set_flags(VkQueryPoolCreateFlags flags) { _flags = flags; return *this; }
    QueryPoolCreateInfo& set_query_type(VkQueryType type) { _query_type = type; return *this; }
    QueryPoolCreateInfo& set_query_count(uint32_t count) { _query_count = count; return *this; }
    QueryPoolCreateInfo& set_pipeline_statistics(VkQueryPipelineStatisticFlags stats) { _pipeline_statistics = stats; return *this; }

    VkQueryPoolCreateInfo to_vk() const {
        VkQueryPoolCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.queryType = _query_type;
        ci.queryCount = _query_count;
        ci.pipelineStatistics = _pipeline_statistics;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkQueryPoolCreateFlags _flags = 0;
    VkQueryType _query_type = VK_QUERY_TYPE_OCCLUSION;
    uint32_t _query_count = 1;
    VkQueryPipelineStatisticFlags _pipeline_statistics = 0;
};

} // namespace wk

#endif

#ifndef wk_ext_rt_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_HPP
#define wk_ext_rt_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class PhysicalDeviceRayTracingPipelineFeatures {
public:
    PhysicalDeviceRayTracingPipelineFeatures& set_p_next(void* p_next) { _p_next = p_next; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_ray_tracing_pipeline(VkBool32 enabled) { _ray_tracing_pipeline = enabled; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_ray_tracing_pipeline_shader_group_handle_capture_replay(VkBool32 enabled) { _ray_tracing_pipeline_shader_group_handle_capture_replay = enabled; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_ray_tracing_pipeline_shader_group_handle_capture_replay_mixed(VkBool32 enabled) { _ray_tracing_pipeline_shader_group_handle_capture_replay_mixed = enabled; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_ray_tracing_pipeline_trace_rays_indirect(VkBool32 enabled) { _ray_tracing_pipeline_trace_rays_indirect = enabled; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_ray_traversal_primitive_culling(VkBool32 enabled) { _ray_traversal_primitive_culling = enabled; return *this; }

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR to_vk() const {
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        features.pNext = _p_next;
        features.rayTracingPipeline = _ray_tracing_pipeline;
        features.rayTracingPipelineShaderGroupHandleCaptureReplay = _ray_tracing_pipeline_shader_group_handle_capture_replay;
        features.rayTracingPipelineShaderGroupHandleCaptureReplayMixed = _ray_tracing_pipeline_shader_group_handle_capture_replay_mixed;
        features.rayTracingPipelineTraceRaysIndirect = _ray_tracing_pipeline_trace_rays_indirect;
        features.rayTraversalPrimitiveCulling = _ray_traversal_primitive_culling;
        return features;
    }

private:
    void* _p_next = nullptr;
    VkBool32 _ray_tracing_pipeline = VK_FALSE;
    VkBool32 _ray_tracing_pipeline_shader_group_handle_capture_replay = VK_FALSE;
    VkBool32 _ray_tracing_pipeline_shader_group_handle_capture_replay_mixed = VK_FALSE;
    VkBool32 _ray_tracing_pipeline_trace_rays_indirect = VK_FALSE;
    VkBool32 _ray_traversal_primitive_culling = VK_FALSE;
};

}

#endif

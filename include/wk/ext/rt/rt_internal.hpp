#ifndef wk_ext_rt_RT_INTERNAL_HPP
#define wk_ext_rt_RT_INTERNAL_HPP

#include "../../wulkan_internal.hpp"

#include <vector>

namespace wk::ext::rt {

struct FeatureChain {
    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDeviceBufferDeviceAddressFeatures bda;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR asf;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtf;
};

struct DeviceFunctions {
    // Acceleration structures
    PFN_vkCreateAccelerationStructureKHR           vkCreateAccelerationStructureKHR = nullptr;
    PFN_vkDestroyAccelerationStructureKHR          vkDestroyAccelerationStructureKHR = nullptr;
    PFN_vkGetAccelerationStructureBuildSizesKHR    vkGetAccelerationStructureBuildSizesKHR = nullptr;
    PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = nullptr;
    PFN_vkCmdBuildAccelerationStructuresKHR        vkCmdBuildAccelerationStructuresKHR = nullptr;

    // Ray tracing pipelines
    PFN_vkCreateRayTracingPipelinesKHR             vkCreateRayTracingPipelinesKHR = nullptr;
    PFN_vkGetRayTracingShaderGroupHandlesKHR       vkGetRayTracingShaderGroupHandlesKHR = nullptr;
    PFN_vkCmdTraceRaysKHR                          vkCmdTraceRaysKHR = nullptr;

    // Deferred operations
    PFN_vkCreateDeferredOperationKHR               vkCreateDeferredOperationKHR = nullptr;
    PFN_vkDestroyDeferredOperationKHR              vkDestroyDeferredOperationKHR = nullptr;
    PFN_vkGetDeferredOperationResultKHR            vkGetDeferredOperationResultKHR = nullptr;
    PFN_vkDeferredOperationJoinKHR                 vkDeferredOperationJoinKHR = nullptr;
};

std::vector<const char*> GetRequiredDeviceExtensions();
FeatureChain MakeFeatureChain();
DeviceFunctions LoadFunctions(VkDevice device);

} // namespace wk::ext::rt

#endif // wk_ext_rt_RT_INTERNAL_HPP

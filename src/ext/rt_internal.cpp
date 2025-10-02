#include "../../include/wk/ext/rt/rt_internal.hpp"

namespace wk::ext::rt {

std::vector<const char*> GetRequiredDeviceExtensions() {
    std::vector<const char*> device_extensions;
#ifdef __APPLE__
    device_extensions.emplace_back("VK_KHR_portability_subset");
#endif
    device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);

    device_extensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);

    device_extensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);

    return device_extensions;
}

FeatureChain MakeFeatureChain() {
    FeatureChain chain{};

    chain.features2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    chain.bda = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES };
    chain.asf = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
    chain.rtf = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };

    chain.features2.pNext = &chain.bda;
    chain.bda.pNext = &chain.asf;
    chain.asf.pNext = &chain.rtf;

    chain.bda.bufferDeviceAddress = VK_TRUE;
    chain.asf.accelerationStructure  = VK_TRUE;
    chain.rtf.rayTracingPipeline = VK_TRUE;

    return chain;
}

DeviceFunctions LoadFunctions(VkDevice device) {\
    DeviceFunctions f{};
    f.vkCreateAccelerationStructureKHR =
        reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
    f.vkDestroyAccelerationStructureKHR =
        reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
    f.vkGetAccelerationStructureBuildSizesKHR =
        reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(
            vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));
    f.vkGetAccelerationStructureDeviceAddressKHR =
        reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(
            vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));
    f.vkCmdBuildAccelerationStructuresKHR =
        reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(
            vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));

    // Ray tracing pipelines
    f.vkCreateRayTracingPipelinesKHR =
        reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(
            vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));
    f.vkGetRayTracingShaderGroupHandlesKHR =
        reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(
            vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
    f.vkCmdTraceRaysKHR =
        reinterpret_cast<PFN_vkCmdTraceRaysKHR>(
            vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));

    // Deferred operations
    f.vkCreateDeferredOperationKHR =
        reinterpret_cast<PFN_vkCreateDeferredOperationKHR>(
            vkGetDeviceProcAddr(device, "vkCreateDeferredOperationKHR"));
    f.vkDestroyDeferredOperationKHR =
        reinterpret_cast<PFN_vkDestroyDeferredOperationKHR>(
            vkGetDeviceProcAddr(device, "vkDestroyDeferredOperationKHR"));
    f.vkGetDeferredOperationResultKHR =
        reinterpret_cast<PFN_vkGetDeferredOperationResultKHR>(
            vkGetDeviceProcAddr(device, "vkGetDeferredOperationResultKHR"));
    f.vkDeferredOperationJoinKHR =
        reinterpret_cast<PFN_vkDeferredOperationJoinKHR>(
            vkGetDeviceProcAddr(device, "vkDeferredOperationJoinKHR"));

    return f;
}
 
} // wk::ext::rt
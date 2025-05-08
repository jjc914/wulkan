#ifndef wk_ext_rt_RT_INTERNAL_HPP
#define wk_ext_rt_RT_INTERNAL_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

std::vector<const char*> GetRTRequiredDeviceExtensions() {
    std::vector<const char*> device_extensions;
    device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    device_extensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME); // rt extensions
    device_extensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
    // device_extensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
    // device_extensions.emplace_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
    // device_extensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);

    device_extensions.emplace_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME); // vma extensions
    device_extensions.emplace_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    return device_extensions;
}

}

#endif
#ifndef wk_ext_rt_RT_INTERNAL_HPP
#define wk_ext_rt_RT_INTERNAL_HPP

#include "../../wulkan_internal.hpp"

#include "physical_device_acceleration_structure_features.hpp"
#include "physical_device_buffer_device_address_features.hpp"
#include "physical_device_ray_tracing_pipeline_features.hpp"
#include "acceleration_structure_build_geometry_info.hpp"
#include "acceleration_structure_build_range_info.hpp"
#include "acceleration_structure_build_sizes_info.hpp"
#include "acceleration_structure_create_info.hpp"
#include "acceleration_structure_geometry.hpp"
#include "acceleration_structure_geometry_triangles_data.hpp"
#include "device_or_host_address_const.hpp"
#include "device_or_host_address.hpp"
#include "acceleration_structure.hpp"
#include "acceleration_structure_instance.hpp"

namespace wk::ext::rt {

std::vector<const char*> GetRTRequiredDeviceExtensions() {
    std::vector<const char*> device_extensions;
    device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    device_extensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME); // rt extensions
    device_extensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
    // device_extensions.emplace_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
    // device_extensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);

    device_extensions.emplace_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME); // vma extensions
    device_extensions.emplace_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    device_extensions.emplace_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    return device_extensions;
}

VkPhysicalDeviceAccelerationStructurePropertiesKHR GetPhysicalDeviceAccelerationStructureProperties(VkPhysicalDevice phys) {
    VkPhysicalDeviceAccelerationStructurePropertiesKHR props{};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    VkPhysicalDeviceProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
    props2.pNext = &props;
    vkGetPhysicalDeviceProperties2(phys, &props2);
    return props;
}

}

#endif
#ifndef wk_ext_rt_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_HPP
#define wk_ext_rt_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class PhysicalDeviceAccelerationStructureFeatures {
public:
    PhysicalDeviceAccelerationStructureFeatures& set_p_next(void* p_next) { _p_next = p_next; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_acceleration_structure(VkBool32 enabled) { _acceleration_structure = enabled; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_acceleration_structure_capture_replay(VkBool32 enabled) { _acceleration_structure_capture_replay = enabled; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_acceleration_structure_indirect_build(VkBool32 enabled) { _acceleration_structure_indirect_build = enabled; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_acceleration_structure_host_commands(VkBool32 enabled) { _acceleration_structure_host_commands = enabled; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_descriptor_binding_acceleration_structure_update_after_bind(VkBool32 enabled) { _descriptor_binding_acceleration_structure_update_after_bind = enabled; return *this; }

    VkPhysicalDeviceAccelerationStructureFeaturesKHR to_vk() const {
        VkPhysicalDeviceAccelerationStructureFeaturesKHR features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        features.pNext = _p_next;
        features.accelerationStructure = _acceleration_structure;
        features.accelerationStructureCaptureReplay = _acceleration_structure_capture_replay;
        features.accelerationStructureIndirectBuild = _acceleration_structure_indirect_build;
        features.accelerationStructureHostCommands = _acceleration_structure_host_commands;
        features.descriptorBindingAccelerationStructureUpdateAfterBind = _descriptor_binding_acceleration_structure_update_after_bind;
        return features;
    }

private:
    void* _p_next = nullptr;
    VkBool32 _acceleration_structure = VK_FALSE;
    VkBool32 _acceleration_structure_capture_replay = VK_FALSE;
    VkBool32 _acceleration_structure_indirect_build = VK_FALSE;
    VkBool32 _acceleration_structure_host_commands = VK_FALSE;
    VkBool32 _descriptor_binding_acceleration_structure_update_after_bind = VK_FALSE;
};

}

#endif

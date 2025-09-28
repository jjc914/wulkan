#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_INSTANCE_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_INSTANCE_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class AccelerationStructureGeometryInstancesData {
public:
    AccelerationStructureGeometryInstancesData& set_p_next(const void* p) { _p_next = p; return *this; }
    AccelerationStructureGeometryInstancesData& set_array_of_pointers(VkBool32 ap) { _array_of_pointers = ap; return *this; }
    AccelerationStructureGeometryInstancesData& set_data_device_address(VkDeviceAddress addr) { _data.deviceAddress = addr; return *this; }

    VkAccelerationStructureGeometryInstancesDataKHR to_vk() const {
        VkAccelerationStructureGeometryInstancesDataKHR v{};
        v.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        v.pNext = _p_next;
        v.arrayOfPointers = _array_of_pointers;
        v.data = _data;
        return v;
    }

private:
    const void* _p_next = nullptr;
    VkBool32 _array_of_pointers = VK_FALSE;
    VkDeviceOrHostAddressConstKHR _data{};
};

class AccelerationStructureInstance {
public:
    AccelerationStructureInstance& set_transform(const VkTransformMatrixKHR& m) { _transform = m; return *this; }
    AccelerationStructureInstance& set_instance_custom_index(uint32_t idx) { _instance_custom_index = idx; return *this; }
    AccelerationStructureInstance& set_mask(uint8_t m) { _mask = m; return *this; }
    AccelerationStructureInstance& set_sbt_record_offset(uint32_t o) { _sbt_record_offset = o; return *this; }
    AccelerationStructureInstance& set_flags(VkGeometryInstanceFlagsKHR f) { _flags = f; return *this; }
    AccelerationStructureInstance& set_acceleration_structure_device_address(VkDeviceAddress a) { _accel_device_addr = a; return *this; }

    VkAccelerationStructureInstanceKHR to_vk() const {
        VkAccelerationStructureInstanceKHR v{};
        v.transform = _transform;
        v.instanceCustomIndex = _instance_custom_index;
        v.mask = _mask;
        v.instanceShaderBindingTableRecordOffset = _sbt_record_offset;
        v.flags = _flags;
        v.accelerationStructureReference = _accel_device_addr;
        return v;
    }

private:
    VkTransformMatrixKHR _transform{};
    uint32_t _instance_custom_index = 0;
    uint8_t _mask = 0xFF;
    uint32_t _sbt_record_offset = 0;
    VkGeometryInstanceFlagsKHR _flags = 0;
    VkDeviceAddress _accel_device_addr = 0;
};

}

#endif

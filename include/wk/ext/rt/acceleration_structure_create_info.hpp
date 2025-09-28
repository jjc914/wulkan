#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_CREATE_INFO_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_CREATE_INFO_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class AccelerationStructureCreateInfo {
public:
    AccelerationStructureCreateInfo& set_p_next(const void* p) { _p_next = p; return *this; }
    AccelerationStructureCreateInfo& set_flags(VkAccelerationStructureCreateFlagsKHR f) { _flags = f; return *this; }
    AccelerationStructureCreateInfo& set_buffer(VkBuffer b) { _buffer = b; return *this; }
    AccelerationStructureCreateInfo& set_offset(VkDeviceSize o) { _offset = o; return *this; }
    AccelerationStructureCreateInfo& set_size(VkDeviceSize s) { _size = s; return *this; }
    AccelerationStructureCreateInfo& set_type(VkAccelerationStructureTypeKHR t) { _type = t; return *this; }
    AccelerationStructureCreateInfo& set_device_address(VkDeviceAddress a) { _device_address = a; return *this; }

    VkAccelerationStructureCreateInfoKHR to_vk() const {
        VkAccelerationStructureCreateInfoKHR v{};
        v.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        v.pNext = _p_next;
        v.createFlags = _flags;
        v.buffer = _buffer;
        v.offset = _offset;
        v.size = _size;
        v.type = _type;
        v.deviceAddress = _device_address;
        return v;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureCreateFlagsKHR _flags = 0;
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceSize _offset = 0;
    VkDeviceSize _size = 0;
    VkAccelerationStructureTypeKHR _type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    VkDeviceAddress _device_address = 0;
};

}

#endif

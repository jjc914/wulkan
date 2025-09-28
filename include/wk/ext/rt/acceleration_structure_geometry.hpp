#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_GEOMETRY_KHR_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_GEOMETRY_KHR_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class AccelerationStructureGeometry {
public:
    AccelerationStructureGeometry& set_p_next(const void* p) { _p_next = p; return *this; }
    AccelerationStructureGeometry& set_flags(VkGeometryFlagsKHR f) { _flags = f; return *this; }
    AccelerationStructureGeometry& set_triangles(const VkAccelerationStructureGeometryTrianglesDataKHR* t) {
        _geometry_type = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        _triangles = t;
        return *this;
    }
    AccelerationStructureGeometry& set_aabbs(const VkAccelerationStructureGeometryAabbsDataKHR* a) {
        _geometry_type = VK_GEOMETRY_TYPE_AABBS_KHR;
        _aabbs = a;
        return *this;
    }
    AccelerationStructureGeometry& set_instances(const VkAccelerationStructureGeometryInstancesDataKHR* i) {
        _geometry_type = VK_GEOMETRY_TYPE_INSTANCES_KHR;
        _instances = i;
        return *this;
    }

    VkAccelerationStructureGeometryKHR to_vk() const {
        VkAccelerationStructureGeometryKHR v{};
        v.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        v.pNext = _p_next;
        v.geometryType = _geometry_type;
        if (_geometry_type == VK_GEOMETRY_TYPE_TRIANGLES_KHR && _triangles)
            v.geometry.triangles = *_triangles;
        else if (_geometry_type == VK_GEOMETRY_TYPE_AABBS_KHR && _aabbs)
            v.geometry.aabbs = *_aabbs;
        else if (_geometry_type == VK_GEOMETRY_TYPE_INSTANCES_KHR && _instances)
            v.geometry.instances = *_instances;
        v.flags = _flags;
        return v;
    }

private:
    const void* _p_next = nullptr;
    VkGeometryTypeKHR _geometry_type = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    const VkAccelerationStructureGeometryTrianglesDataKHR* _triangles = nullptr;
    const VkAccelerationStructureGeometryAabbsDataKHR* _aabbs = nullptr;
    const VkAccelerationStructureGeometryInstancesDataKHR* _instances = nullptr;
    VkGeometryFlagsKHR _flags = 0;
};

}

#endif

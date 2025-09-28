#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class AccelerationStructureBuildGeometryInfo {
public:
    AccelerationStructureBuildGeometryInfo& set_p_next(const void* p) { _p_next = p; return *this; }
    AccelerationStructureBuildGeometryInfo& set_type(VkAccelerationStructureTypeKHR t) { _type = t; return *this; }
    AccelerationStructureBuildGeometryInfo& set_flags(VkBuildAccelerationStructureFlagsKHR f) { _flags = f; return *this; }
    AccelerationStructureBuildGeometryInfo& set_mode(VkBuildAccelerationStructureModeKHR m) { _mode = m; return *this; }
    AccelerationStructureBuildGeometryInfo& set_src_acceleration_structure(VkAccelerationStructureKHR h) { _src_as = h; return *this; }
    AccelerationStructureBuildGeometryInfo& set_dst_acceleration_structure(VkAccelerationStructureKHR h) { _dst_as = h; return *this; }
    AccelerationStructureBuildGeometryInfo& set_geometries(uint32_t c, const VkAccelerationStructureGeometryKHR* g) {
        _geometry_count = c; _geometries = g; return *this;
    }
    AccelerationStructureBuildGeometryInfo& set_scratch_data(VkDeviceOrHostAddressKHR d) { _scratch_data = d; return *this; }

    VkAccelerationStructureBuildGeometryInfoKHR to_vk() const {
        VkAccelerationStructureBuildGeometryInfoKHR v{};
        v.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        v.pNext = _p_next;
        v.type = _type;
        v.flags = _flags;
        v.mode = _mode;
        v.srcAccelerationStructure = _src_as;
        v.dstAccelerationStructure = _dst_as;
        v.geometryCount = _geometry_count;
        v.pGeometries = _geometries;
        v.scratchData = _scratch_data;
        return v;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureTypeKHR _type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    VkBuildAccelerationStructureFlagsKHR _flags = 0;
    VkBuildAccelerationStructureModeKHR _mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    VkAccelerationStructureKHR _src_as = VK_NULL_HANDLE;
    VkAccelerationStructureKHR _dst_as = VK_NULL_HANDLE;
    uint32_t _geometry_count = 0;
    const VkAccelerationStructureGeometryKHR* _geometries = nullptr;
    VkDeviceOrHostAddressKHR _scratch_data{};
};

}

#endif

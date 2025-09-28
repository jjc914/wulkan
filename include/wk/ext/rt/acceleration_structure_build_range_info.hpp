#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_BUILD_RANGE_INFO_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_BUILD_RANGE_INFO_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class AccelerationStructureBuildRangeInfo {
public:
    AccelerationStructureBuildRangeInfo& set_primitive_count(uint32_t c) { _primitive_count = c; return *this; }
    AccelerationStructureBuildRangeInfo& set_primitive_offset(uint32_t o) { _primitive_offset = o; return *this; }
    AccelerationStructureBuildRangeInfo& set_first_vertex(uint32_t v) { _first_vertex = v; return *this; }
    AccelerationStructureBuildRangeInfo& set_transform_offset(uint32_t o) { _transform_offset = o; return *this; }

    VkAccelerationStructureBuildRangeInfoKHR to_vk() const {
        VkAccelerationStructureBuildRangeInfoKHR v{};
        v.primitiveCount   = _primitive_count;
        v.primitiveOffset  = _primitive_offset;
        v.firstVertex      = _first_vertex;
        v.transformOffset  = _transform_offset;
        return v;
    }

private:
    uint32_t _primitive_count = 0;
    uint32_t _primitive_offset = 0;
    uint32_t _first_vertex = 0;
    uint32_t _transform_offset = 0;
};

}

#endif

#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class AccelerationStructureBuildSizesInfo {
public:
    AccelerationStructureBuildSizesInfo& set_p_next(const void* p) { _p_next = p; return *this; }
    AccelerationStructureBuildSizesInfo& set_accel_size(VkDeviceSize s) { _accel_size = s; return *this; }
    AccelerationStructureBuildSizesInfo& set_build_scratch_size(VkDeviceSize s) { _build_scratch_size = s; return *this; }
    AccelerationStructureBuildSizesInfo& set_update_scratch_size(VkDeviceSize s) { _update_scratch_size = s; return *this; }

    VkAccelerationStructureBuildSizesInfoKHR to_vk() const {
        VkAccelerationStructureBuildSizesInfoKHR v{};
        v.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        v.pNext = _p_next;
        v.accelerationStructureSize = _accel_size;
        v.buildScratchSize = _build_scratch_size;
        v.updateScratchSize = _update_scratch_size;
        return v;
    }

private:
    const void* _p_next = nullptr;
    VkDeviceSize _accel_size = 0;
    VkDeviceSize _build_scratch_size = 0;
    VkDeviceSize _update_scratch_size = 0;
};

}

#endif

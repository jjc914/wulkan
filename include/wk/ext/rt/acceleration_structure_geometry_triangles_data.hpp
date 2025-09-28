#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class AccelerationStructureGeometryTrianglesData {
public:
    AccelerationStructureGeometryTrianglesData& set_p_next(const void* p) { _p_next = p; return *this; }
    AccelerationStructureGeometryTrianglesData& set_vertex_format(VkFormat f) { _vertex_format = f; return *this; }
    AccelerationStructureGeometryTrianglesData& set_vertex_data(VkDeviceOrHostAddressConstKHR d) { _vertex_data = d; return *this; }
    AccelerationStructureGeometryTrianglesData& set_vertex_stride(VkDeviceSize s) { _vertex_stride = s; return *this; }
    AccelerationStructureGeometryTrianglesData& set_max_vertex(uint32_t m) { _max_vertex = m; return *this; }
    AccelerationStructureGeometryTrianglesData& set_index_type(VkIndexType t) { _index_type = t; return *this; }
    AccelerationStructureGeometryTrianglesData& set_index_data(VkDeviceOrHostAddressConstKHR d) { _index_data = d; return *this; }
    AccelerationStructureGeometryTrianglesData& set_transform_data(VkDeviceOrHostAddressConstKHR d) { _transform_data = d; return *this; }

    VkAccelerationStructureGeometryTrianglesDataKHR to_vk() const {
        VkAccelerationStructureGeometryTrianglesDataKHR v{};
        v.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        v.pNext = _p_next;
        v.vertexFormat = _vertex_format;
        v.vertexData = _vertex_data;
        v.vertexStride = _vertex_stride;
        v.maxVertex = _max_vertex;
        v.indexType = _index_type;
        v.indexData = _index_data;
        v.transformData = _transform_data;
        return v;
    }

private:
    const void* _p_next = nullptr;
    VkFormat _vertex_format = VK_FORMAT_UNDEFINED;
    VkDeviceOrHostAddressConstKHR _vertex_data{};
    VkDeviceSize _vertex_stride = 0;
    uint32_t _max_vertex = 0;
    VkIndexType _index_type = VK_INDEX_TYPE_NONE_KHR;
    VkDeviceOrHostAddressConstKHR _index_data{};
    VkDeviceOrHostAddressConstKHR _transform_data{};
};

}

#endif

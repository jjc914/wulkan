#ifndef wulkan_WK_EXT_RT_ACCELERATION_STRUCTURE_HPP
#define wulkan_WK_EXT_RT_ACCELERATION_STRUCTURE_HPP

#include "wk/ext/rt/rt_internal.hpp"
#include "wk/buffer.hpp"

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <iostream>

namespace wk::ext::rt {

class AccelerationStructure {
public:
    AccelerationStructure() = default;
    AccelerationStructure(VkDevice device, const DeviceFunctions& f, const VkAccelerationStructureCreateInfoKHR& ci)
        : _device(device),
          _vkCreateAccelerationStructureKHR(f.vkCreateAccelerationStructureKHR),
          _vkDestroyAccelerationStructureKHR(f.vkDestroyAccelerationStructureKHR)
    {
        if (_vkCreateAccelerationStructureKHR == VK_NULL_HANDLE) {
            std::cerr << "device function vkCreateAccelerationStructureKHR not set" << std::endl;
        } else if (_vkCreateAccelerationStructureKHR(_device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create acceleration structure" << std::endl;
        }
    }

    ~AccelerationStructure() {
        if (_handle != VK_NULL_HANDLE) {
            _vkDestroyAccelerationStructureKHR(_device, _handle, nullptr);
        }
        _handle = VK_NULL_HANDLE;
        _device = VK_NULL_HANDLE;
        _vkCreateAccelerationStructureKHR = VK_NULL_HANDLE;
        _vkDestroyAccelerationStructureKHR = VK_NULL_HANDLE;
    }

    AccelerationStructure(const AccelerationStructure&) = delete;
    AccelerationStructure& operator=(const AccelerationStructure&) = delete;

    AccelerationStructure(AccelerationStructure&& other) noexcept
        : _device(other._device), _handle(other._handle)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
        other._vkCreateAccelerationStructureKHR = VK_NULL_HANDLE;
        other._vkDestroyAccelerationStructureKHR = VK_NULL_HANDLE;
    }

    AccelerationStructure& operator=(AccelerationStructure&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE && _device != VK_NULL_HANDLE) {
                _vkDestroyAccelerationStructureKHR(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            _vkCreateAccelerationStructureKHR = other._vkCreateAccelerationStructureKHR;
            _vkDestroyAccelerationStructureKHR = other._vkDestroyAccelerationStructureKHR;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
            other._vkCreateAccelerationStructureKHR = VK_NULL_HANDLE;
            other._vkDestroyAccelerationStructureKHR = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkAccelerationStructureKHR& handle() const { return _handle; }

private:
    VkAccelerationStructureKHR _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    PFN_vkCreateAccelerationStructureKHR _vkCreateAccelerationStructureKHR = VK_NULL_HANDLE;
    PFN_vkDestroyAccelerationStructureKHR _vkDestroyAccelerationStructureKHR = VK_NULL_HANDLE;
};

class AccelerationStructureCreateInfo {
public:
    AccelerationStructureCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureCreateInfo& set_buffer(VkBuffer buffer) { _buffer = buffer; return *this; }
    AccelerationStructureCreateInfo& set_offset(VkDeviceSize offset) { _offset = offset; return *this; }
    AccelerationStructureCreateInfo& set_size(VkDeviceSize size) { _size = size; return *this; }
    AccelerationStructureCreateInfo& set_type(VkAccelerationStructureTypeKHR type) { _type = type; return *this; }
    AccelerationStructureCreateInfo& set_create_flags(VkAccelerationStructureCreateFlagsKHR flags) { _create_flags = flags; return *this; }
    AccelerationStructureCreateInfo& set_device_address(VkDeviceAddress addr) { _device_address = addr; return *this; }

    VkAccelerationStructureCreateInfoKHR to_vk() const {
        VkAccelerationStructureCreateInfoKHR ci{};
        ci.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        ci.pNext = _p_next;
        ci.createFlags = _create_flags;
        ci.buffer = _buffer;
        ci.offset = _offset;
        ci.size = _size;
        ci.type = _type;
        ci.deviceAddress = _device_address;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureCreateFlagsKHR _create_flags = 0;
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceSize _offset = 0;
    VkDeviceSize _size = 0;
    VkAccelerationStructureTypeKHR _type = VK_ACCELERATION_STRUCTURE_TYPE_MAX_ENUM_KHR;
    VkDeviceAddress _device_address = 0;
};

class AccelerationStructureBuildGeometryInfo {
public:
    AccelerationStructureBuildGeometryInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureBuildGeometryInfo& set_type(VkAccelerationStructureTypeKHR type) { _type = type; return *this; }
    AccelerationStructureBuildGeometryInfo& set_flags(VkBuildAccelerationStructureFlagsKHR flags) { _flags = flags; return *this; }
    AccelerationStructureBuildGeometryInfo& set_mode(VkBuildAccelerationStructureModeKHR mode) { _mode = mode; return *this; }
    AccelerationStructureBuildGeometryInfo& set_src_as(VkAccelerationStructureKHR src) { _src_as = src; return *this; }
    AccelerationStructureBuildGeometryInfo& set_dst_acceleration_structure(VkAccelerationStructureKHR dst) { _dst_as = dst; return *this; }
    AccelerationStructureBuildGeometryInfo& set_geometries(uint32_t count, const VkAccelerationStructureGeometryKHR* geoms) {
        _geom_count = count; _p_geometries = geoms; _pp_geometries = nullptr; return *this;
    }
    AccelerationStructureBuildGeometryInfo& set_geometries_ptrs(uint32_t count, const VkAccelerationStructureGeometryKHR* const* geoms_ptrs) {
        _geom_count = count; _pp_geometries = geoms_ptrs; _p_geometries = nullptr; return *this;
    }
    AccelerationStructureBuildGeometryInfo& set_scratch_data(VkDeviceOrHostAddressKHR addr) { _scratch_data = addr; return *this; }

    VkAccelerationStructureBuildGeometryInfoKHR to_vk() const {
        VkAccelerationStructureBuildGeometryInfoKHR bi{};
        bi.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        bi.pNext = _p_next;
        bi.type = _type;
        bi.flags = _flags;
        bi.mode = _mode;
        bi.srcAccelerationStructure = _src_as;
        bi.dstAccelerationStructure = _dst_as;
        bi.geometryCount = _geom_count;
        bi.pGeometries = _p_geometries;
        bi.ppGeometries = _pp_geometries;
        bi.scratchData = _scratch_data;
        return bi;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureTypeKHR _type = VK_ACCELERATION_STRUCTURE_TYPE_MAX_ENUM_KHR;
    VkBuildAccelerationStructureFlagsKHR _flags = 0;
    VkBuildAccelerationStructureModeKHR _mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR;
    VkAccelerationStructureKHR _src_as = VK_NULL_HANDLE;
    VkAccelerationStructureKHR _dst_as = VK_NULL_HANDLE;
    uint32_t _geom_count = 0;
    const VkAccelerationStructureGeometryKHR* _p_geometries = nullptr;
    const VkAccelerationStructureGeometryKHR* const* _pp_geometries = nullptr;
    VkDeviceOrHostAddressKHR _scratch_data{};
};

class AccelerationStructureBuildRangeInfo {
public:
    AccelerationStructureBuildRangeInfo& set_primitive_count(uint32_t count) { _primitive_count = count; return *this; }
    AccelerationStructureBuildRangeInfo& set_primitive_offset(uint32_t offset) { _primitive_offset = offset; return *this; }
    AccelerationStructureBuildRangeInfo& set_first_vertex(uint32_t v) { _first_vertex = v; return *this; }
    AccelerationStructureBuildRangeInfo& set_transform_offset(uint32_t offset) { _transform_offset = offset; return *this; }

    VkAccelerationStructureBuildRangeInfoKHR to_vk() const {
        VkAccelerationStructureBuildRangeInfoKHR ri{};
        ri.primitiveCount = _primitive_count;
        ri.primitiveOffset = _primitive_offset;
        ri.firstVertex = _first_vertex;
        ri.transformOffset = _transform_offset;
        return ri;
    }

private:
    uint32_t _primitive_count = 0;
    uint32_t _primitive_offset = 0;
    uint32_t _first_vertex = 0;
    uint32_t _transform_offset = 0;
};

class AccelerationStructureBuildSizesInfo {
public:
    AccelerationStructureBuildSizesInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureBuildSizesInfo& set_acceleration_structure_size(VkDeviceSize size) { _accel_size = size; return *this; }
    AccelerationStructureBuildSizesInfo& set_update_scratch_size(VkDeviceSize size) { _update_scratch = size; return *this; }
    AccelerationStructureBuildSizesInfo& set_build_scratch_size(VkDeviceSize size) { _build_scratch = size; return *this; }

    VkAccelerationStructureBuildSizesInfoKHR to_vk() const {
        VkAccelerationStructureBuildSizesInfoKHR si{};
        si.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        si.pNext = _p_next;
        si.accelerationStructureSize = _accel_size;
        si.updateScratchSize = _update_scratch;
        si.buildScratchSize = _build_scratch;
        return si;
    }

private:
    const void* _p_next = nullptr;
    VkDeviceSize _accel_size = 0;
    VkDeviceSize _update_scratch = 0;
    VkDeviceSize _build_scratch = 0;
};

class AccelerationStructureDeviceAddressInfo {
public:
    AccelerationStructureDeviceAddressInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureDeviceAddressInfo& set_acceleration_structure(VkAccelerationStructureKHR as) { _as = as; return *this; }

    VkAccelerationStructureDeviceAddressInfoKHR to_vk() const {
        VkAccelerationStructureDeviceAddressInfoKHR di{};
        di.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        di.pNext = _p_next;
        di.accelerationStructure = _as;
        return di;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureKHR _as = VK_NULL_HANDLE;
};

class AccelerationStructureGeometry {
public:
    AccelerationStructureGeometry& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureGeometry& set_geometry_type(VkGeometryTypeKHR type) { _type = type; return *this; }
    AccelerationStructureGeometry& set_flags(VkGeometryFlagsKHR flags) { _flags = flags; return *this; }
    AccelerationStructureGeometry& set_geometry(const VkAccelerationStructureGeometryDataKHR& data) { _data = data; return *this; }

    VkAccelerationStructureGeometryKHR to_vk() const {
        VkAccelerationStructureGeometryKHR g{};
        g.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        g.pNext = _p_next;
        g.geometryType = _type;
        g.geometry = _data;
        g.flags = _flags;
        return g;
    }

private:
    const void* _p_next = nullptr;
    VkGeometryTypeKHR _type = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    VkGeometryFlagsKHR _flags = 0;
    VkAccelerationStructureGeometryDataKHR _data{};
};

class AccelerationStructureGeometryTrianglesData {
public:
    AccelerationStructureGeometryTrianglesData& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureGeometryTrianglesData& set_vertex_format(VkFormat format) { _vertex_format = format; return *this; }
    AccelerationStructureGeometryTrianglesData& set_vertex_data(VkDeviceOrHostAddressConstKHR data) { _vertex_data = data; return *this; }
    AccelerationStructureGeometryTrianglesData& set_vertex_stride(VkDeviceSize stride) { _vertex_stride = stride; return *this; }
    AccelerationStructureGeometryTrianglesData& set_max_vertex(uint32_t max_v) { _max_vertex = max_v; return *this; }
    AccelerationStructureGeometryTrianglesData& set_index_type(VkIndexType type) { _index_type = type; return *this; }
    AccelerationStructureGeometryTrianglesData& set_index_data(VkDeviceOrHostAddressConstKHR data) { _index_data = data; return *this; }
    AccelerationStructureGeometryTrianglesData& set_transform_data(VkDeviceOrHostAddressConstKHR data) { _transform_data = data; return *this; }

    VkAccelerationStructureGeometryTrianglesDataKHR to_vk() const {
        VkAccelerationStructureGeometryTrianglesDataKHR t{};
        t.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        t.pNext = _p_next;
        t.vertexFormat = _vertex_format;
        t.vertexData = _vertex_data;
        t.vertexStride = _vertex_stride;
        t.maxVertex = _max_vertex;
        t.indexType = _index_type;
        t.indexData = _index_data;
        t.transformData = _transform_data;
        return t;
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

class AccelerationStructureGeometryAabbsData {
public:
    AccelerationStructureGeometryAabbsData& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureGeometryAabbsData& set_data(VkDeviceOrHostAddressConstKHR data) { _data = data; return *this; }
    AccelerationStructureGeometryAabbsData& set_stride(VkDeviceSize stride) { _stride = stride; return *this; }

    VkAccelerationStructureGeometryAabbsDataKHR to_vk() const {
        VkAccelerationStructureGeometryAabbsDataKHR a{};
        a.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
        a.pNext = _p_next;
        a.data = _data;
        a.stride = _stride;
        return a;
    }

private:
    const void* _p_next = nullptr;
    VkDeviceOrHostAddressConstKHR _data{};
    VkDeviceSize _stride = 0;
};

class AccelerationStructureGeometryInstancesData {
public:
    AccelerationStructureGeometryInstancesData& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureGeometryInstancesData& set_array_of_pointers(VkBool32 b) { _array_of_pointers = b; return *this; }
    AccelerationStructureGeometryInstancesData& set_data(VkDeviceOrHostAddressConstKHR data) { _data = data; return *this; }

    VkAccelerationStructureGeometryInstancesDataKHR to_vk() const {
        VkAccelerationStructureGeometryInstancesDataKHR i{};
        i.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        i.pNext = _p_next;
        i.arrayOfPointers = _array_of_pointers;
        i.data = _data;
        return i;
    }

private:
    const void* _p_next = nullptr;
    VkBool32 _array_of_pointers = VK_FALSE;
    VkDeviceOrHostAddressConstKHR _data{};
};

class AccelerationStructureGeometryData {
public:
    AccelerationStructureGeometryData& set_triangles(const VkAccelerationStructureGeometryTrianglesDataKHR& t) {
        _triangles = t;
        _type = Type::Triangles;
        return *this;
    }
    AccelerationStructureGeometryData& set_aabbs(const VkAccelerationStructureGeometryAabbsDataKHR& a) {
        _aabbs = a;
        _type = Type::Aabbs;
        return *this;
    }
    AccelerationStructureGeometryData& set_instances(const VkAccelerationStructureGeometryInstancesDataKHR& i) {
        _instances = i;
        _type = Type::Instances;
        return *this;
    }

    VkAccelerationStructureGeometryDataKHR to_vk() const {
        VkAccelerationStructureGeometryDataKHR data{};
        switch (_type) {
        case Type::Triangles:
            data.triangles = _triangles;
            break;
        case Type::Aabbs:
            data.aabbs = _aabbs;
            break;
        case Type::Instances:
            data.instances = _instances;
            break;
        case Type::None:
        default:
            break;
        }
        return data;
    }

private:
    enum class Type { None, Triangles, Aabbs, Instances };
    Type _type = Type::None;

    VkAccelerationStructureGeometryTrianglesDataKHR _triangles{};
    VkAccelerationStructureGeometryAabbsDataKHR _aabbs{};
    VkAccelerationStructureGeometryInstancesDataKHR _instances{};
};

class TransformMatrix {
public:
    // Set entire matrix (row-major). m[3][4].
    TransformMatrix& set_matrix(const float m[3][4]) {
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 4; ++c)
                _m[r][c] = m[r][c];
        return *this;
    }
    // Convenience setters
    TransformMatrix& set_row(int r, float x, float y, float z, float w) {
        _m[r][0]=x; _m[r][1]=y; _m[r][2]=z; _m[r][3]=w; return *this;
    }
    TransformMatrix& set_identity() {
        _m[0][0]=1.f; _m[0][1]=0.f; _m[0][2]=0.f; _m[0][3]=0.f;
        _m[1][0]=0.f; _m[1][1]=1.f; _m[1][2]=0.f; _m[1][3]=0.f;
        _m[2][0]=0.f; _m[2][1]=0.f; _m[2][2]=1.f; _m[2][3]=0.f; return *this;
    }

    VkTransformMatrixKHR to_vk() const {
        VkTransformMatrixKHR t{};
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 4; ++c)
                t.matrix[r][c] = _m[r][c];
        return t;
    }

private:
    float _m[3][4]{{1,0,0,0},{0,1,0,0},{0,0,1,0}};
};

class AccelerationStructureInstance {
public:
    AccelerationStructureInstance& set_transform(const VkTransformMatrixKHR& t) { _transform = t; return *this; }
    AccelerationStructureInstance& set_instance_custom_index(uint32_t idx24) { _instance_custom_index = idx24 & 0x00FFFFFFu; return *this; }
    AccelerationStructureInstance& set_mask(uint8_t mask8) { _mask = mask8; return *this; }
    AccelerationStructureInstance& set_instance_shader_binding_table_record_offset(uint32_t off24) { _sbt_record_offset = off24 & 0x00FFFFFFu; return *this; }
    AccelerationStructureInstance& set_flags(VkGeometryInstanceFlagsKHR flags8) { _flags = flags8; return *this; }
    AccelerationStructureInstance& set_acceleration_structure_reference(uint64_t ref) { _as_ref = ref; return *this; }

    VkAccelerationStructureInstanceKHR to_vk() const {
        VkAccelerationStructureInstanceKHR i{};
        i.transform = _transform;
        i.instanceCustomIndex = _instance_custom_index;
        i.mask = _mask;
        i.instanceShaderBindingTableRecordOffset = _sbt_record_offset;
        i.flags = _flags;
        i.accelerationStructureReference = _as_ref;
        return i;
    }

private:
    VkTransformMatrixKHR _transform{};
    uint32_t _instance_custom_index = 0;
    uint8_t  _mask = 0xFF;
    uint32_t _sbt_record_offset = 0;
    VkGeometryInstanceFlagsKHR _flags = 0;
    uint64_t _as_ref = 0;
};

class CopyAccelerationStructureInfo {
public:
    CopyAccelerationStructureInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    CopyAccelerationStructureInfo& set_src(VkAccelerationStructureKHR src) { _src = src; return *this; }
    CopyAccelerationStructureInfo& set_dst(VkAccelerationStructureKHR dst) { _dst = dst; return *this; }
    CopyAccelerationStructureInfo& set_mode(VkCopyAccelerationStructureModeKHR mode) { _mode = mode; return *this; }

    VkCopyAccelerationStructureInfoKHR to_vk() const {
        VkCopyAccelerationStructureInfoKHR ci{};
        ci.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
        ci.pNext = _p_next;
        ci.src = _src;
        ci.dst = _dst;
        ci.mode = _mode;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureKHR _src = VK_NULL_HANDLE;
    VkAccelerationStructureKHR _dst = VK_NULL_HANDLE;
    VkCopyAccelerationStructureModeKHR _mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_KHR;
};

class CopyAccelerationStructureToMemoryInfo {
public:
    CopyAccelerationStructureToMemoryInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    CopyAccelerationStructureToMemoryInfo& set_src(VkAccelerationStructureKHR src) { _src = src; return *this; }
    CopyAccelerationStructureToMemoryInfo& set_dst(VkDeviceOrHostAddressKHR dst) { _dst = dst; return *this; }
    CopyAccelerationStructureToMemoryInfo& set_mode(VkCopyAccelerationStructureModeKHR mode) { _mode = mode; return *this; }

    VkCopyAccelerationStructureToMemoryInfoKHR to_vk() const {
        VkCopyAccelerationStructureToMemoryInfoKHR ci{};
        ci.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
        ci.pNext = _p_next;
        ci.src = _src;
        ci.dst = _dst;
        ci.mode = _mode;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureKHR _src = VK_NULL_HANDLE;
    VkDeviceOrHostAddressKHR _dst{};
    VkCopyAccelerationStructureModeKHR _mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;
};

class CopyMemoryToAccelerationStructureInfo {
public:
    CopyMemoryToAccelerationStructureInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    CopyMemoryToAccelerationStructureInfo& set_src(VkDeviceOrHostAddressConstKHR src) { _src = src; return *this; }
    CopyMemoryToAccelerationStructureInfo& set_dst(VkAccelerationStructureKHR dst) { _dst = dst; return *this; }
    CopyMemoryToAccelerationStructureInfo& set_mode(VkCopyAccelerationStructureModeKHR mode) { _mode = mode; return *this; }

    VkCopyMemoryToAccelerationStructureInfoKHR to_vk() const {
        VkCopyMemoryToAccelerationStructureInfoKHR ci{};
        ci.sType = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
        ci.pNext = _p_next;
        ci.src = _src;
        ci.dst = _dst;
        ci.mode = _mode;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkDeviceOrHostAddressConstKHR _src{};
    VkAccelerationStructureKHR _dst = VK_NULL_HANDLE;
    VkCopyAccelerationStructureModeKHR _mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;
};

class AccelerationStructureVersionInfo {
public:
    AccelerationStructureVersionInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    AccelerationStructureVersionInfo& set_p_version_data(const uint8_t* data) { _p_version_data = data; return *this; }

    VkAccelerationStructureVersionInfoKHR to_vk() const {
        VkAccelerationStructureVersionInfoKHR vi{};
        vi.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_VERSION_INFO_KHR;
        vi.pNext = _p_next;
        vi.pVersionData = _p_version_data;
        return vi;
    }

private:
    const void* _p_next = nullptr;
    const uint8_t* _p_version_data = nullptr;
};

class WriteDescriptorSetAccelerationStructure {
public:
    WriteDescriptorSetAccelerationStructure& set_p_next(const void* p_next){ _p_next=p_next; return *this; }
    WriteDescriptorSetAccelerationStructure& set_acceleration_structures(uint32_t count, const VkAccelerationStructureKHR* ptr){
        _count=count; _p_accels=ptr; return *this;
    }

    VkWriteDescriptorSetAccelerationStructureKHR to_vk() const {
        VkWriteDescriptorSetAccelerationStructureKHR w{};
        w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        w.pNext = _p_next; w.accelerationStructureCount=_count; w.pAccelerationStructures=_p_accels; return w;
    }

private:
    const void* _p_next=nullptr; uint32_t _count=0; const VkAccelerationStructureKHR* _p_accels=nullptr;
};

} // namespace wk::ext::rt

#endif // wulkan_WK_EXT_RT_ACCELERATION_STRUCTURE_HPP

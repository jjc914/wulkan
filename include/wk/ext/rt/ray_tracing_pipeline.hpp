#ifndef wulkan_WK_EXT_RT_RAY_TRACING_PIPELINE_HPP
#define wulkan_WK_EXT_RT_RAY_TRACING_PIPELINE_HPP

#include "wk/ext/rt/rt_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk::ext::rt {

class RayTracingPipeline {
public:
    RayTracingPipeline() = default;

    RayTracingPipeline(VkDevice device, const DeviceFunctions& f, const VkRayTracingPipelineCreateInfoKHR& ci,
        VkDeferredOperationKHR deferred_op = VK_NULL_HANDLE,
        VkPipelineCache pipeline_cache = VK_NULL_HANDLE)
        : _device(device),
          _vkCreateRayTracingPipelinesKHR(f.vkCreateRayTracingPipelinesKHR)
    {
        if (_vkCreateRayTracingPipelinesKHR(_device,
                deferred_op,
                pipeline_cache,
                1,
                &ci,
                nullptr,
                &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create ray tracing pipeline");
        }
    }

    ~RayTracingPipeline() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyPipeline(_device, _handle, nullptr);
        }
        _handle = VK_NULL_HANDLE;
        _device = VK_NULL_HANDLE;
    }

    RayTracingPipeline(const RayTracingPipeline&) = delete;
    RayTracingPipeline& operator=(const RayTracingPipeline&) = delete;

    RayTracingPipeline(RayTracingPipeline&& other) noexcept
        : _device(other._device), _handle(other._handle), _vkCreateRayTracingPipelinesKHR(other._vkCreateRayTracingPipelinesKHR)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
        other._vkCreateRayTracingPipelinesKHR = VK_NULL_HANDLE;
    }

    RayTracingPipeline& operator=(RayTracingPipeline&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyPipeline(_device, _handle, nullptr);
            }
            _handle = other._handle;
            _device = other._device;
            _vkCreateRayTracingPipelinesKHR = other._vkCreateRayTracingPipelinesKHR;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
            other._vkCreateRayTracingPipelinesKHR = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkPipeline& handle() const { return _handle; }

private:
    VkPipeline _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    PFN_vkCreateRayTracingPipelinesKHR _vkCreateRayTracingPipelinesKHR = VK_NULL_HANDLE;
};

class RayTracingShaderGroupCreateInfo {
public:
    RayTracingShaderGroupCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    RayTracingShaderGroupCreateInfo& set_type(VkRayTracingShaderGroupTypeKHR type) { _type = type; return *this; }
    RayTracingShaderGroupCreateInfo& set_general_shader(uint32_t idx) { _general_shader = idx; return *this; }
    RayTracingShaderGroupCreateInfo& set_closest_hit_shader(uint32_t idx) { _closest_hit_shader = idx; return *this; }
    RayTracingShaderGroupCreateInfo& set_any_hit_shader(uint32_t idx) { _any_hit_shader = idx; return *this; }
    RayTracingShaderGroupCreateInfo& set_intersection_shader(uint32_t idx) { _intersection_shader = idx; return *this; }
    RayTracingShaderGroupCreateInfo& set_p_shader_group_capture_replay_handle(const void* ptr) { _p_capture_replay = ptr; return *this; }

    VkRayTracingShaderGroupCreateInfoKHR to_vk() const {
        VkRayTracingShaderGroupCreateInfoKHR g{};
        g.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        g.pNext = _p_next;
        g.type = _type;
        g.generalShader = _general_shader;
        g.closestHitShader = _closest_hit_shader;
        g.anyHitShader = _any_hit_shader;
        g.intersectionShader = _intersection_shader;
        g.pShaderGroupCaptureReplayHandle = _p_capture_replay;
        return g;
    }

private:
    const void* _p_next = nullptr;
    VkRayTracingShaderGroupTypeKHR _type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    uint32_t _general_shader = VK_SHADER_UNUSED_KHR;       // Vulkan defines VK_SHADER_UNUSED_KHR
    uint32_t _closest_hit_shader = VK_SHADER_UNUSED_KHR;
    uint32_t _any_hit_shader = VK_SHADER_UNUSED_KHR;
    uint32_t _intersection_shader = VK_SHADER_UNUSED_KHR;
    const void* _p_capture_replay = nullptr;
};

class RayTracingPipelineInterfaceCreateInfo {
public:
    RayTracingPipelineInterfaceCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    RayTracingPipelineInterfaceCreateInfo& set_max_pipeline_ray_payload_size(uint32_t sz) { _max_payload_size = sz; return *this; }
    RayTracingPipelineInterfaceCreateInfo& set_max_pipeline_ray_hit_attribute_size(uint32_t sz) { _max_hit_attr_size = sz; return *this; }

    VkRayTracingPipelineInterfaceCreateInfoKHR to_vk() const {
        VkRayTracingPipelineInterfaceCreateInfoKHR i{};
        i.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR;
        i.pNext = _p_next;
        i.maxPipelineRayPayloadSize = _max_payload_size;
        i.maxPipelineRayHitAttributeSize = _max_hit_attr_size;
        return i;
    }

private:
    const void* _p_next = nullptr;
    uint32_t _max_payload_size = 0;
    uint32_t _max_hit_attr_size = 0;
};

class PipelineLibraryCreateInfo {
public:
    PipelineLibraryCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineLibraryCreateInfo& set_libraries(uint32_t count, const VkPipeline* libs) { _library_count = count; _p_libraries = libs; return *this; }

    VkPipelineLibraryCreateInfoKHR to_vk() const {
        VkPipelineLibraryCreateInfoKHR li{};
        li.sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;
        li.pNext = _p_next;
        li.libraryCount = _library_count;
        li.pLibraries = _p_libraries;
        return li;
    }

private:
    const void* _p_next = nullptr;
    uint32_t _library_count = 0;
    const VkPipeline* _p_libraries = nullptr;
};

class RayTracingPipelineCreateInfo {
public:
    RayTracingPipelineCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    RayTracingPipelineCreateInfo& set_flags(VkPipelineCreateFlags flags) { _flags = flags; return *this; }
    RayTracingPipelineCreateInfo& set_stages(uint32_t count, const VkPipelineShaderStageCreateInfo* stages) {
        _stage_count = count; _p_stages = stages; return *this;
    }
    RayTracingPipelineCreateInfo& set_groups(uint32_t count, const VkRayTracingShaderGroupCreateInfoKHR* groups) {
        _group_count = count; _p_groups = groups; return *this;
    }
    RayTracingPipelineCreateInfo& set_max_pipeline_ray_recursion_depth(uint32_t depth) {
        _max_recursion_depth = depth; return *this;
    }
    RayTracingPipelineCreateInfo& set_p_library_info(const VkPipelineLibraryCreateInfoKHR* info) {
        _p_library_info = info; return *this;
    }
    RayTracingPipelineCreateInfo& set_p_library_interface(const VkRayTracingPipelineInterfaceCreateInfoKHR* iface) {
        _p_library_interface = iface; return *this;
    }
    RayTracingPipelineCreateInfo& set_p_dynamic_state(const VkPipelineDynamicStateCreateInfo* dyn) {
        _p_dynamic_state = dyn; return *this;
    }
    RayTracingPipelineCreateInfo& set_layout(VkPipelineLayout layout) { _layout = layout; return *this; }
    RayTracingPipelineCreateInfo& set_base_pipeline_handle(VkPipeline base) { _base_pipeline_handle = base; return *this; }
    RayTracingPipelineCreateInfo& set_base_pipeline_index(int32_t index) { _base_pipeline_index = index; return *this; }

    VkRayTracingPipelineCreateInfoKHR to_vk() const {
        VkRayTracingPipelineCreateInfoKHR ci{};
        ci.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.stageCount = _stage_count;
        ci.pStages = _p_stages;
        ci.groupCount = _group_count;
        ci.pGroups = _p_groups;
        ci.maxPipelineRayRecursionDepth = _max_recursion_depth;
        ci.pLibraryInfo = _p_library_info;
        ci.pLibraryInterface = _p_library_interface;
        ci.pDynamicState = _p_dynamic_state;
        ci.layout = _layout;
        ci.basePipelineHandle = _base_pipeline_handle;
        ci.basePipelineIndex = _base_pipeline_index;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineCreateFlags _flags = 0;
    uint32_t _stage_count = 0;
    const VkPipelineShaderStageCreateInfo* _p_stages = nullptr;
    uint32_t _group_count = 0;
    const VkRayTracingShaderGroupCreateInfoKHR* _p_groups = nullptr;
    uint32_t _max_recursion_depth = 1;
    const VkPipelineLibraryCreateInfoKHR* _p_library_info = nullptr;
    const VkRayTracingPipelineInterfaceCreateInfoKHR* _p_library_interface = nullptr;
    const VkPipelineDynamicStateCreateInfo* _p_dynamic_state = nullptr;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    VkPipeline _base_pipeline_handle = VK_NULL_HANDLE;
    int32_t _base_pipeline_index = -1;
};

class StridedDeviceAddressRegion {
public:
    StridedDeviceAddressRegion& set_device_address(VkDeviceAddress addr) { _device_address = addr; return *this; }
    StridedDeviceAddressRegion& set_stride(VkDeviceSize stride)          { _stride = stride; return *this; }
    StridedDeviceAddressRegion& set_size(VkDeviceSize size)              { _size = size; return *this; }

    VkStridedDeviceAddressRegionKHR to_vk() const {
        VkStridedDeviceAddressRegionKHR r{};
        r.deviceAddress = _device_address;
        r.stride        = _stride;
        r.size          = _size;
        return r;
    }

private:
    VkDeviceAddress _device_address = 0;
    VkDeviceSize _stride = 0;
    VkDeviceSize _size   = 0;
};

class DeviceAddress {
public:
    DeviceAddress& set(VkDeviceAddress addr) { _addr = addr; return *this; }
    VkDeviceAddress value() const { return _addr; }
private:
    VkDeviceAddress _addr = 0;
};

class TraceRaysIndirectCommand {
public:
    TraceRaysIndirectCommand& set_width(uint32_t w)  { _w = w; return *this; }
    TraceRaysIndirectCommand& set_height(uint32_t h) { _h = h; return *this; }
    TraceRaysIndirectCommand& set_depth(uint32_t d)  { _d = d; return *this; }

    VkTraceRaysIndirectCommandKHR to_vk() const {
        VkTraceRaysIndirectCommandKHR c{};
        c.width = _w; c.height = _h; c.depth = _d;
        return c;
    }

private:
    uint32_t _w = 0, _h = 0, _d = 0;
};

class TraceRaysIndirectCommand2 {
public:
    TraceRaysIndirectCommand2& set_raygen(VkDeviceAddress addr, VkDeviceSize size) { _rgen_addr = addr; _rgen_size = size; return *this; }
    TraceRaysIndirectCommand2& set_miss(VkDeviceAddress addr, VkDeviceSize size, VkDeviceSize stride) { _miss_addr = addr; _miss_size = size; _miss_stride = stride; return *this; }
    TraceRaysIndirectCommand2& set_hit(VkDeviceAddress addr, VkDeviceSize size, VkDeviceSize stride) { _hit_addr = addr; _hit_size = size; _hit_stride = stride; return *this; }
    TraceRaysIndirectCommand2& set_callable(VkDeviceAddress addr, VkDeviceSize size, VkDeviceSize stride) { _call_addr = addr; _call_size = size; _call_stride = stride; return *this; }
    TraceRaysIndirectCommand2& set_extent(uint32_t w, uint32_t h, uint32_t d) { _w=w; _h=h; _d=d; return *this; }

    VkTraceRaysIndirectCommand2KHR to_vk() const {
        VkTraceRaysIndirectCommand2KHR c{};
        c.raygenShaderRecordAddress       = _rgen_addr;
        c.raygenShaderRecordSize          = _rgen_size;
        c.missShaderBindingTableAddress   = _miss_addr;
        c.missShaderBindingTableSize      = _miss_size;
        c.missShaderBindingTableStride    = _miss_stride;
        c.hitShaderBindingTableAddress    = _hit_addr;
        c.hitShaderBindingTableSize       = _hit_size;
        c.hitShaderBindingTableStride     = _hit_stride;
        c.callableShaderBindingTableAddress = _call_addr;
        c.callableShaderBindingTableSize    = _call_size;
        c.callableShaderBindingTableStride  = _call_stride;
        c.width  = _w; c.height = _h; c.depth = _d;
        return c;
    }

private:
    VkDeviceAddress _rgen_addr = 0;
    VkDeviceSize    _rgen_size = 0;
    VkDeviceAddress _miss_addr = 0;
    VkDeviceSize    _miss_size = 0, _miss_stride = 0;
    VkDeviceAddress _hit_addr = 0;
    VkDeviceSize    _hit_size = 0, _hit_stride = 0;
    VkDeviceAddress _call_addr = 0;
    VkDeviceSize    _call_size = 0, _call_stride = 0;
    uint32_t _w = 0, _h = 0, _d = 0;
};

class PhysicalDeviceRayTracingPipelineFeatures {
public:
    PhysicalDeviceRayTracingPipelineFeatures& set_p_next(void* p_next) { _p_next = p_next; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_ray_tracing_pipeline(VkBool32 b){ _rt_pipeline=b; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_handle_capture_replay(VkBool32 b){ _handle_capture_replay=b; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_handle_capture_replay_mixed(VkBool32 b){ _handle_capture_replay_mixed=b; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_trace_rays_indirect(VkBool32 b){ _trace_rays_indirect=b; return *this; }
    PhysicalDeviceRayTracingPipelineFeatures& set_traversal_primitive_culling(VkBool32 b){ _primitive_culling=b; return *this; }

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR to_vk() const {
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR f{};
        f.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        f.pNext = _p_next;
        f.rayTracingPipeline = _rt_pipeline;
        f.rayTracingPipelineShaderGroupHandleCaptureReplay = _handle_capture_replay;
        f.rayTracingPipelineShaderGroupHandleCaptureReplayMixed = _handle_capture_replay_mixed;
        f.rayTracingPipelineTraceRaysIndirect = _trace_rays_indirect;
        f.rayTraversalPrimitiveCulling = _primitive_culling;
        return f;
    }

private:
    void* _p_next = nullptr;
    VkBool32 _rt_pipeline = VK_FALSE;
    VkBool32 _handle_capture_replay = VK_FALSE;
    VkBool32 _handle_capture_replay_mixed = VK_FALSE;
    VkBool32 _trace_rays_indirect = VK_FALSE;
    VkBool32 _primitive_culling = VK_FALSE;
};

class PhysicalDeviceAccelerationStructureFeatures {
public:
    PhysicalDeviceAccelerationStructureFeatures& set_p_next(void* p_next){ _p_next=p_next; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_acceleration_structure(VkBool32 b){ _accel=b; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_capture_replay(VkBool32 b){ _capture_replay=b; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_indirect_build(VkBool32 b){ _indirect_build=b; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_host_commands(VkBool32 b){ _host_cmds=b; return *this; }
    PhysicalDeviceAccelerationStructureFeatures& set_update_after_bind(VkBool32 b){ _update_after_bind=b; return *this; }

    VkPhysicalDeviceAccelerationStructureFeaturesKHR to_vk() const {
        VkPhysicalDeviceAccelerationStructureFeaturesKHR f{};
        f.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        f.pNext = _p_next;
        f.accelerationStructure = _accel;
        f.accelerationStructureCaptureReplay = _capture_replay;
        f.accelerationStructureIndirectBuild = _indirect_build;
        f.accelerationStructureHostCommands = _host_cmds;
        f.descriptorBindingAccelerationStructureUpdateAfterBind = _update_after_bind;
        return f;
    }

private:
    void* _p_next = nullptr;
    VkBool32 _accel = VK_FALSE;
    VkBool32 _capture_replay = VK_FALSE;
    VkBool32 _indirect_build = VK_FALSE;
    VkBool32 _host_cmds = VK_FALSE;
    VkBool32 _update_after_bind = VK_FALSE;
};

class PhysicalDeviceBufferDeviceAddressFeatures {
public:
    PhysicalDeviceBufferDeviceAddressFeatures& set_p_next(void* p_next){ _p_next=p_next; return *this; }
    PhysicalDeviceBufferDeviceAddressFeatures& set_buffer_device_address(VkBool32 b){ _bda=b; return *this; }
    PhysicalDeviceBufferDeviceAddressFeatures& set_capture_replay(VkBool32 b){ _bda_cr=b; return *this; }
    PhysicalDeviceBufferDeviceAddressFeatures& set_multi_device(VkBool32 b){ _bda_md=b; return *this; }

    VkPhysicalDeviceBufferDeviceAddressFeatures to_vk() const {
        VkPhysicalDeviceBufferDeviceAddressFeatures f{};
        f.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        f.pNext = _p_next;
        f.bufferDeviceAddress = _bda;
        f.bufferDeviceAddressCaptureReplay = _bda_cr;
        f.bufferDeviceAddressMultiDevice = _bda_md;
        return f;
    }

private:
    void* _p_next = nullptr;
    VkBool32 _bda = VK_FALSE;
    VkBool32 _bda_cr = VK_FALSE;
    VkBool32 _bda_md = VK_FALSE;
};

class PhysicalDeviceRayTracingPipelineProperties {
public:
    PhysicalDeviceRayTracingPipelineProperties& set_p_next(void* p_next){ _p_next=p_next; return *this; }
    PhysicalDeviceRayTracingPipelineProperties& set_shader_group_handle_size(uint32_t v){ _shaderGroupHandleSize=v; return *this; }
    PhysicalDeviceRayTracingPipelineProperties& set_max_ray_recursion_depth(uint32_t v){ _maxRayRecursionDepth=v; return *this; }
    PhysicalDeviceRayTracingPipelineProperties& set_max_shader_group_stride(uint32_t v){ _maxShaderGroupStride=v; return *this; }
    PhysicalDeviceRayTracingPipelineProperties& set_shader_group_base_alignment(uint32_t v){ _shaderGroupBaseAlignment=v; return * this; }
    PhysicalDeviceRayTracingPipelineProperties& set_shader_group_handle_capture_replay_size(uint32_t v){ _shaderGroupHandleCaptureReplaySize=v; return * this; }
    PhysicalDeviceRayTracingPipelineProperties& set_max_ray_dispatch_invocation_count(uint32_t v){ _maxRayDispatchInvocationCount=v; return * this; }
    PhysicalDeviceRayTracingPipelineProperties& set_shader_group_handle_alignment(uint32_t v){ _shaderGroupHandleAlignment=v; return * this; }
    PhysicalDeviceRayTracingPipelineProperties& set_max_ray_hit_attribute_size(uint32_t v){ _maxRayHitAttributeSize=v; return * this; }

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR to_vk() const {
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR p{};
        p.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        p.pNext = _p_next;
        p.shaderGroupHandleSize = _shaderGroupHandleSize;
        p.maxRayRecursionDepth = _maxRayRecursionDepth;
        p.maxShaderGroupStride = _maxShaderGroupStride;
        p.shaderGroupBaseAlignment = _shaderGroupBaseAlignment;
        p.shaderGroupHandleCaptureReplaySize = _shaderGroupHandleCaptureReplaySize;
        p.maxRayDispatchInvocationCount = _maxRayDispatchInvocationCount;
        p.shaderGroupHandleAlignment = _shaderGroupHandleAlignment;
        p.maxRayHitAttributeSize = _maxRayHitAttributeSize;
        return p;
    }

private:
    void* _p_next = nullptr;
    uint32_t _shaderGroupHandleSize = 0;
    uint32_t _maxRayRecursionDepth = 0;
    uint32_t _maxShaderGroupStride = 0;
    uint32_t _shaderGroupBaseAlignment = 0;
    uint32_t _shaderGroupHandleCaptureReplaySize = 0;
    uint32_t _maxRayDispatchInvocationCount = 0;
    uint32_t _shaderGroupHandleAlignment = 0;
    uint32_t _maxRayHitAttributeSize = 0;
};

class PhysicalDeviceAccelerationStructureProperties {
public:
    PhysicalDeviceAccelerationStructureProperties& set_p_next(void* p_next){ _p_next=p_next; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_max_geometry_count(uint64_t v){ _maxGeometryCount=v; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_max_instance_count(uint64_t v){ _maxInstanceCount=v; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_max_primitive_count(uint64_t v){ _maxPrimitiveCount=v; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_max_per_stage_desc_as(uint32_t v){ _maxPerStageDescriptorAccelerationStructures=v; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_max_per_stage_desc_uab_as(uint32_t v){ _maxPerStageDescriptorUpdateAfterBindAccelerationStructures=v; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_max_desc_set_as(uint32_t v){ _maxDescriptorSetAccelerationStructures=v; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_max_desc_set_uab_as(uint32_t v){ _maxDescriptorSetUpdateAfterBindAccelerationStructures=v; return *this; }
    PhysicalDeviceAccelerationStructureProperties& set_min_scratch_offset_alignment(uint32_t v){ _minAccelerationStructureScratchOffsetAlignment=v; return *this; }

    VkPhysicalDeviceAccelerationStructurePropertiesKHR to_vk() const {
        VkPhysicalDeviceAccelerationStructurePropertiesKHR p{};
        p.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
        p.pNext = _p_next;
        p.maxGeometryCount = _maxGeometryCount;
        p.maxInstanceCount = _maxInstanceCount;
        p.maxPrimitiveCount = _maxPrimitiveCount;
        p.maxPerStageDescriptorAccelerationStructures = _maxPerStageDescriptorAccelerationStructures;
        p.maxPerStageDescriptorUpdateAfterBindAccelerationStructures = _maxPerStageDescriptorUpdateAfterBindAccelerationStructures;
        p.maxDescriptorSetAccelerationStructures = _maxDescriptorSetAccelerationStructures;
        p.maxDescriptorSetUpdateAfterBindAccelerationStructures = _maxDescriptorSetUpdateAfterBindAccelerationStructures;
        p.minAccelerationStructureScratchOffsetAlignment = _minAccelerationStructureScratchOffsetAlignment;
        return p;
    }

private:
    void* _p_next = nullptr;
    uint64_t _maxGeometryCount = 0;
    uint64_t _maxInstanceCount = 0;
    uint64_t _maxPrimitiveCount = 0;
    uint32_t _maxPerStageDescriptorAccelerationStructures = 0;
    uint32_t _maxPerStageDescriptorUpdateAfterBindAccelerationStructures = 0;
    uint32_t _maxDescriptorSetAccelerationStructures = 0;
    uint32_t _maxDescriptorSetUpdateAfterBindAccelerationStructures = 0;
    uint32_t _minAccelerationStructureScratchOffsetAlignment = 1;
};

} // namespace wk::ext::rt

#endif // wulkan_WK_EXT_RT_RAY_TRACING_PIPELINE_HPP

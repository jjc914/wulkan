#ifndef wulkan_wk_PIPELINE_HPP
#define wulkan_wk_PIPELINE_HPP

#include "wulkan_internal.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class Viewport {
public:
    Viewport& set_x(float x) { _x = x; return *this; }
    Viewport& set_y(float y) { _y = y; return *this; }
    Viewport& set_width(float width) { _width = width; return *this; }
    Viewport& set_height(float height) { _height = height; return *this; }
    Viewport& set_min_depth(float min_depth) { _min_depth = min_depth; return *this; }
    Viewport& set_max_depth(float max_depth) { _max_depth = max_depth; return *this; }

    VkViewport to_vk() {
        VkViewport viewport{};
        viewport.x = _x;
        viewport.y = _y;
        viewport.width = _width;
        viewport.height = _height;
        viewport.minDepth = _min_depth;
        viewport.maxDepth = _max_depth;
        return viewport;
    }
private:
    float _x = 0;
    float _y = 0;
    float _width = 0;
    float _height = 0;
    float _min_depth = 0;
    float _max_depth = 0;
};

class Rect2D {
public:
    Rect2D& set_offset(VkOffset2D offset) { _offset = offset; return *this; }
    Rect2D& set_extent(VkExtent2D extent) { _extent = extent; return *this; }

    VkRect2D to_vk() {
        VkRect2D scissor{};
        scissor.offset = _offset;
        scissor.extent = _extent;
        return scissor;
    }
private:
    VkOffset2D _offset{};
    VkExtent2D _extent{};
};

class PipelineShaderStageCreateInfo {
public:
    PipelineShaderStageCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineShaderStageCreateInfo& set_flags(VkPipelineShaderStageCreateFlags flags) { _flags = flags; return *this; }
    PipelineShaderStageCreateInfo& set_stage(VkShaderStageFlagBits stage) { _stage = stage; return *this; }
    PipelineShaderStageCreateInfo& set_module(VkShaderModule module) { _module = module; return *this; }
    PipelineShaderStageCreateInfo& set_p_name(const char* p_name) { _p_name = p_name; return *this; }
    PipelineShaderStageCreateInfo& set_p_specialization_info(const VkSpecializationInfo* p_specialization_info) {
        _p_specialization_info = p_specialization_info; return *this;
    }

    VkPipelineShaderStageCreateInfo to_vk() const {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.stage = _stage;
        ci.module = _module;
        ci.pName = _p_name;
        ci.pSpecializationInfo = _p_specialization_info;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineShaderStageCreateFlags _flags = 0;
    VkShaderStageFlagBits _stage = VK_SHADER_STAGE_VERTEX_BIT;
    VkShaderModule _module = VK_NULL_HANDLE;
    const char* _p_name = "main";
    const VkSpecializationInfo* _p_specialization_info = nullptr;
};

class VertexInputBindingDescription {
public:
    VertexInputBindingDescription& set_binding(uint32_t binding) { _binding = binding; return *this; }
    VertexInputBindingDescription& set_stride(uint32_t stride) { _stride = stride; return *this; }
    VertexInputBindingDescription& set_input_rate(VkVertexInputRate input_rate) { _input_rate = input_rate; return *this; }

    VkVertexInputBindingDescription to_vk() const {
        VkVertexInputBindingDescription desc{};
        desc.binding = _binding;
        desc.stride = _stride;
        desc.inputRate = _input_rate;
        return desc;
    }

private:
    uint32_t _binding = 0;
    uint32_t _stride = 0;
    VkVertexInputRate _input_rate = VK_VERTEX_INPUT_RATE_VERTEX;
};    


class VertexBindingDescription {
public:
    VertexBindingDescription& set_binding(uint32_t binding) { _binding = binding; return *this; }
    VertexBindingDescription& set_stride(uint32_t stride) { _stride = stride; return *this; }
    VertexBindingDescription& set_input_rate(VkVertexInputRate input_rate) { _input_rate = input_rate; return *this; }

    VkVertexInputBindingDescription to_vk_vertex_input_binding_description() {
        VkVertexInputBindingDescription vkbd{};
        vkbd.binding = _binding;
        vkbd.stride = _stride;
        vkbd.inputRate = _input_rate;
        return vkbd;
    }
private:
    uint32_t _binding = 0;
    uint32_t _stride = 0;
    VkVertexInputRate _input_rate = VK_VERTEX_INPUT_RATE_MAX_ENUM;
};

class VertexInputAttributeDescription {
public:
    VertexInputAttributeDescription& set_location(uint32_t location) { _location = location; return *this; }
    VertexInputAttributeDescription& set_binding(uint32_t binding) { _binding = binding; return *this; }
    VertexInputAttributeDescription& set_format(VkFormat format) { _format = format; return *this; }
    VertexInputAttributeDescription& set_offset(uint32_t offset) { _offset = offset; return *this; }

    VkVertexInputAttributeDescription to_vk() {
        VkVertexInputAttributeDescription vkad{};
        vkad.location = _location;
        vkad.binding = _binding;
        vkad.format = _format;
        vkad.offset = _offset;
        return vkad;
    }
private:
    uint32_t _binding = 0;
    uint32_t _location = 0;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    uint32_t _offset = 0;
};

class PipelineDynamicStateCreateInfo {
public:
    PipelineDynamicStateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineDynamicStateCreateInfo& set_flags(VkPipelineDynamicStateCreateFlags flags) { _flags = flags; return *this; }
    PipelineDynamicStateCreateInfo& set_dynamic_states(uint32_t count, const VkDynamicState* p_states) {
        _dynamic_state_count = count;
        _p_dynamic_states = p_states;
        return *this;
    }

    VkPipelineDynamicStateCreateInfo to_vk() const {
        VkPipelineDynamicStateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.dynamicStateCount = _dynamic_state_count;
        ci.pDynamicStates = _p_dynamic_states;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineDynamicStateCreateFlags _flags = 0;
    uint32_t _dynamic_state_count = 0;
    const VkDynamicState* _p_dynamic_states = nullptr;
};

class PipelineVertexInputStateCreateInfo {
public:
    PipelineVertexInputStateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineVertexInputStateCreateInfo& set_flags(VkPipelineVertexInputStateCreateFlags flags) { _flags = flags; return *this; }
    PipelineVertexInputStateCreateInfo& set_vertex_binding_descriptions(uint32_t count, const VkVertexInputBindingDescription* p_bindings) {
        _binding_count = count;
        _p_bindings = p_bindings;
        return *this;
    }
    PipelineVertexInputStateCreateInfo& set_vertex_attribute_descriptions(uint32_t count, const VkVertexInputAttributeDescription* p_attributes) {
        _attribute_count = count;
        _p_attributes = p_attributes;
        return *this;
    }

    VkPipelineVertexInputStateCreateInfo to_vk() const {
        VkPipelineVertexInputStateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.vertexBindingDescriptionCount = _binding_count;
        ci.pVertexBindingDescriptions = _p_bindings;
        ci.vertexAttributeDescriptionCount = _attribute_count;
        ci.pVertexAttributeDescriptions = _p_attributes;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineVertexInputStateCreateFlags _flags = 0;
    uint32_t _binding_count = 0;
    const VkVertexInputBindingDescription* _p_bindings = nullptr;
    uint32_t _attribute_count = 0;
    const VkVertexInputAttributeDescription* _p_attributes = nullptr;
};

class PipelineInputAssemblyStateCreateInfo {
public:
    PipelineInputAssemblyStateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineInputAssemblyStateCreateInfo& set_flags(VkPipelineInputAssemblyStateCreateFlags flags) { _flags = flags; return *this; }
    PipelineInputAssemblyStateCreateInfo& set_topology(VkPrimitiveTopology topology) { _topology = topology; return *this; }
    PipelineInputAssemblyStateCreateInfo& set_primitive_restart_enable(VkBool32 enable) { _primitive_restart_enable = enable; return *this; }

    VkPipelineInputAssemblyStateCreateInfo to_vk() const {
        VkPipelineInputAssemblyStateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.topology = _topology;
        ci.primitiveRestartEnable = _primitive_restart_enable;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineInputAssemblyStateCreateFlags _flags = 0;
    VkPrimitiveTopology _topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32 _primitive_restart_enable = VK_FALSE;
};

class PipelineViewportStateCreateInfo {
public:
    PipelineViewportStateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineViewportStateCreateInfo& set_flags(VkPipelineViewportStateCreateFlags flags) { _flags = flags; return *this; }
    PipelineViewportStateCreateInfo& set_viewports(uint32_t count, const VkViewport* p_viewports) { _viewport_count = count; _p_viewports = p_viewports; return *this; }
    PipelineViewportStateCreateInfo& set_scissors(uint32_t count, const VkRect2D* p_scissors) { _scissor_count = count; _p_scissors = p_scissors; return *this; }

    VkPipelineViewportStateCreateInfo to_vk() const {
        VkPipelineViewportStateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.viewportCount = _viewport_count;
        ci.pViewports = _p_viewports;
        ci.scissorCount = _scissor_count;
        ci.pScissors = _p_scissors;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineViewportStateCreateFlags _flags = 0;
    uint32_t _viewport_count = 0;
    const VkViewport* _p_viewports = nullptr;
    uint32_t _scissor_count = 0;
    const VkRect2D* _p_scissors = nullptr;
};

class PipelineRasterizationStateCreateInfo {
public:
    PipelineRasterizationStateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineRasterizationStateCreateInfo& set_flags(VkPipelineRasterizationStateCreateFlags flags) { _flags = flags; return *this; }
    PipelineRasterizationStateCreateInfo& set_depth_clamp_enable(VkBool32 enable) { _depth_clamp_enable = enable; return *this; }
    PipelineRasterizationStateCreateInfo& set_rasterizer_discard_enable(VkBool32 enable) { _rasterizer_discard_enable = enable; return *this; }
    PipelineRasterizationStateCreateInfo& set_polygon_mode(VkPolygonMode mode) { _polygon_mode = mode; return *this; }
    PipelineRasterizationStateCreateInfo& set_cull_mode(VkCullModeFlags mode) { _cull_mode = mode; return *this; }
    PipelineRasterizationStateCreateInfo& set_front_face(VkFrontFace face) { _front_face = face; return *this; }
    PipelineRasterizationStateCreateInfo& set_depth_bias_enable(VkBool32 enable) { _depth_bias_enable = enable; return *this; }
    PipelineRasterizationStateCreateInfo& set_depth_bias_constant_factor(float factor) { _depth_bias_constant_factor = factor; return *this; }
    PipelineRasterizationStateCreateInfo& set_depth_bias_clamp(float clamp) { _depth_bias_clamp = clamp; return *this; }
    PipelineRasterizationStateCreateInfo& set_depth_bias_slope_factor(float factor) { _depth_bias_slope_factor = factor; return *this; }
    PipelineRasterizationStateCreateInfo& set_line_width(float width) { _line_width = width; return *this; }

    VkPipelineRasterizationStateCreateInfo to_vk() const {
        VkPipelineRasterizationStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = _p_next;
        info.flags = _flags;
        info.depthClampEnable = _depth_clamp_enable;
        info.rasterizerDiscardEnable = _rasterizer_discard_enable;
        info.polygonMode = _polygon_mode;
        info.cullMode = _cull_mode;
        info.frontFace = _front_face;
        info.depthBiasEnable = _depth_bias_enable;
        info.depthBiasConstantFactor = _depth_bias_constant_factor;
        info.depthBiasClamp = _depth_bias_clamp;
        info.depthBiasSlopeFactor = _depth_bias_slope_factor;
        info.lineWidth = _line_width;
        return info;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineRasterizationStateCreateFlags _flags = 0;
    VkBool32 _depth_clamp_enable = VK_FALSE;
    VkBool32 _rasterizer_discard_enable = VK_FALSE;
    VkPolygonMode _polygon_mode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags _cull_mode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace _front_face = VK_FRONT_FACE_CLOCKWISE;
    VkBool32 _depth_bias_enable = VK_FALSE;
    float _depth_bias_constant_factor = 0.0f;
    float _depth_bias_clamp = 0.0f;
    float _depth_bias_slope_factor = 0.0f;
    float _line_width = 1.0f;
};

class PipelineMultisampleStateCreateInfo {
public:
    PipelineMultisampleStateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineMultisampleStateCreateInfo& set_flags(VkPipelineMultisampleStateCreateFlags flags) { _flags = flags; return *this; }
    PipelineMultisampleStateCreateInfo& set_rasterization_samples(VkSampleCountFlagBits samples) { _rasterization_samples = samples; return *this; }
    PipelineMultisampleStateCreateInfo& set_sample_shading_enable(VkBool32 enable) { _sample_shading_enable = enable; return *this; }
    PipelineMultisampleStateCreateInfo& set_min_sample_shading(float min_sample_shading) { _min_sample_shading = min_sample_shading; return *this; }
    PipelineMultisampleStateCreateInfo& set_p_sample_mask(const VkSampleMask* p_sample_mask) { _p_sample_mask = p_sample_mask; return *this; }
    PipelineMultisampleStateCreateInfo& set_alpha_to_coverage_enable(VkBool32 enable) { _alpha_to_coverage_enable = enable; return *this; }
    PipelineMultisampleStateCreateInfo& set_alpha_to_one_enable(VkBool32 enable) { _alpha_to_one_enable = enable; return *this; }

    VkPipelineMultisampleStateCreateInfo to_vk() const {
        VkPipelineMultisampleStateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.rasterizationSamples = _rasterization_samples;
        ci.sampleShadingEnable = _sample_shading_enable;
        ci.minSampleShading = _min_sample_shading;
        ci.pSampleMask = _p_sample_mask;
        ci.alphaToCoverageEnable = _alpha_to_coverage_enable;
        ci.alphaToOneEnable = _alpha_to_one_enable;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineMultisampleStateCreateFlags _flags = 0;
    VkSampleCountFlagBits _rasterization_samples = VK_SAMPLE_COUNT_1_BIT;
    VkBool32 _sample_shading_enable = VK_FALSE;
    float _min_sample_shading = 0.0f;
    const VkSampleMask* _p_sample_mask = nullptr;
    VkBool32 _alpha_to_coverage_enable = VK_FALSE;
    VkBool32 _alpha_to_one_enable = VK_FALSE;
};

class PipelineDepthStencilStateCreateInfo {
    public:
        PipelineDepthStencilStateCreateInfo& set_p_next(void* p_next) { _p_next = p_next; return *this; }
        PipelineDepthStencilStateCreateInfo& set_flags(VkPipelineDepthStencilStateCreateFlags flags) { _flags = flags; return *this; }
        PipelineDepthStencilStateCreateInfo& set_depth_test_enable(VkBool32 enable) { _depth_test_enable = enable; return *this; }
        PipelineDepthStencilStateCreateInfo& set_depth_write_enable(VkBool32 enable) { _depth_write_enable = enable; return *this; }
        PipelineDepthStencilStateCreateInfo& set_depth_compare_op(VkCompareOp op) { _depth_compare_op = op; return *this; }
        PipelineDepthStencilStateCreateInfo& set_depth_bounds_test_enable(VkBool32 enable) { _depth_bounds_test_enable = enable; return *this; }
        PipelineDepthStencilStateCreateInfo& set_stencil_test_enable(VkBool32 enable) { _stencil_test_enable = enable; return *this; }
        PipelineDepthStencilStateCreateInfo& set_front(const VkStencilOpState& front) { _front = front; return *this; }
        PipelineDepthStencilStateCreateInfo& set_back(const VkStencilOpState& back) { _back = back; return *this; }
        PipelineDepthStencilStateCreateInfo& set_min_depth_bounds(float min_bounds) { _min_depth_bounds = min_bounds; return *this; }
        PipelineDepthStencilStateCreateInfo& set_max_depth_bounds(float max_bounds) { _max_depth_bounds = max_bounds; return *this; }
    
        VkPipelineDepthStencilStateCreateInfo to_vk() const {
            VkPipelineDepthStencilStateCreateInfo ci{};
            ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            ci.pNext = _p_next;
            ci.flags = _flags;
            ci.depthTestEnable = _depth_test_enable;
            ci.depthWriteEnable = _depth_write_enable;
            ci.depthCompareOp = _depth_compare_op;
            ci.depthBoundsTestEnable = _depth_bounds_test_enable;
            ci.stencilTestEnable = _stencil_test_enable;
            ci.front = _front;
            ci.back = _back;
            ci.minDepthBounds = _min_depth_bounds;
            ci.maxDepthBounds = _max_depth_bounds;
            return ci;
        }
    
    private:
        void* _p_next = nullptr;
        VkPipelineDepthStencilStateCreateFlags _flags = 0;
        VkBool32 _depth_test_enable = VK_TRUE;
        VkBool32 _depth_write_enable = VK_TRUE;
        VkCompareOp _depth_compare_op = VK_COMPARE_OP_LESS;
        VkBool32 _depth_bounds_test_enable = VK_FALSE;
        VkBool32 _stencil_test_enable = VK_FALSE;
        VkStencilOpState _front = {};
        VkStencilOpState _back = {};
        float _min_depth_bounds = 0.0f;
        float _max_depth_bounds = 1.0f;
    };

class PipelineColorBlendAttachmentState {
public:
    PipelineColorBlendAttachmentState& set_blend_enable(VkBool32 enable) { _blend_enable = enable; return *this; }
    PipelineColorBlendAttachmentState& set_src_color_blend_factor(VkBlendFactor factor) { _src_color_blend_factor = factor; return *this; }
    PipelineColorBlendAttachmentState& set_dst_color_blend_factor(VkBlendFactor factor) { _dst_color_blend_factor = factor; return *this; }
    PipelineColorBlendAttachmentState& set_color_blend_op(VkBlendOp op) { _color_blend_op = op; return *this; }
    PipelineColorBlendAttachmentState& set_src_alpha_blend_factor(VkBlendFactor factor) { _src_alpha_blend_factor = factor; return *this; }
    PipelineColorBlendAttachmentState& set_dst_alpha_blend_factor(VkBlendFactor factor) { _dst_alpha_blend_factor = factor; return *this; }
    PipelineColorBlendAttachmentState& set_alpha_blend_op(VkBlendOp op) { _alpha_blend_op = op; return *this; }
    PipelineColorBlendAttachmentState& set_color_write_mask(VkColorComponentFlags mask) { _color_write_mask = mask; return *this; }

    VkPipelineColorBlendAttachmentState to_vk() const {
        VkPipelineColorBlendAttachmentState state{};
        state.blendEnable = _blend_enable;
        state.srcColorBlendFactor = _src_color_blend_factor;
        state.dstColorBlendFactor = _dst_color_blend_factor;
        state.colorBlendOp = _color_blend_op;
        state.srcAlphaBlendFactor = _src_alpha_blend_factor;
        state.dstAlphaBlendFactor = _dst_alpha_blend_factor;
        state.alphaBlendOp = _alpha_blend_op;
        state.colorWriteMask = _color_write_mask;
        return state;
    }

private:
    VkBool32 _blend_enable = VK_FALSE;
    VkBlendFactor _src_color_blend_factor = VK_BLEND_FACTOR_ONE;
    VkBlendFactor _dst_color_blend_factor = VK_BLEND_FACTOR_ZERO;
    VkBlendOp _color_blend_op = VK_BLEND_OP_ADD;
    VkBlendFactor _src_alpha_blend_factor = VK_BLEND_FACTOR_ONE;
    VkBlendFactor _dst_alpha_blend_factor = VK_BLEND_FACTOR_ZERO;
    VkBlendOp _alpha_blend_op = VK_BLEND_OP_ADD;
    VkColorComponentFlags _color_write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
};

class PipelineColorBlendStateCreateInfo {
public:
    PipelineColorBlendStateCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineColorBlendStateCreateInfo& set_flags(VkPipelineColorBlendStateCreateFlags flags) { _flags = flags; return *this; }
    PipelineColorBlendStateCreateInfo& set_logic_op_enable(VkBool32 enable) { _logic_op_enable = enable; return *this; }
    PipelineColorBlendStateCreateInfo& set_logic_op(VkLogicOp op) { _logic_op = op; return *this; }

    PipelineColorBlendStateCreateInfo& set_attachments(uint32_t count, const VkPipelineColorBlendAttachmentState* attachments) {
        _attachment_count = count;
        _p_attachments = attachments;
        return *this;
    }

    PipelineColorBlendStateCreateInfo& set_blend_constants(const float constants[4]) {
        _p_blend_constants = constants;
        return *this;
    }

    VkPipelineColorBlendStateCreateInfo to_vk() const {
        VkPipelineColorBlendStateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.logicOpEnable = _logic_op_enable;
        ci.logicOp = _logic_op;
        ci.attachmentCount = _attachment_count;
        ci.pAttachments = _p_attachments;
        if (_p_blend_constants) {
            memcpy(ci.blendConstants, _p_blend_constants, sizeof(float) * 4);
        }
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineColorBlendStateCreateFlags _flags = 0;
    VkBool32 _logic_op_enable = VK_FALSE;
    VkLogicOp _logic_op = VK_LOGIC_OP_COPY;
    uint32_t _attachment_count = 0;
    const VkPipelineColorBlendAttachmentState* _p_attachments = nullptr;
    const float* _p_blend_constants = nullptr;
};

class PipelineCreateInfo {
public:
    PipelineCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    PipelineCreateInfo& set_flags(VkPipelineCreateFlags flags) { _flags = flags; return *this; }
    PipelineCreateInfo& set_stages(uint32_t count, const VkPipelineShaderStageCreateInfo* p_stages) { 
        _stage_count = count;
        _p_stages = p_stages; 
        return *this; 
    }
    PipelineCreateInfo& set_p_vertex_input_state(const VkPipelineVertexInputStateCreateInfo* p_vertex_input_state) { _p_vertex_input_state = p_vertex_input_state; return *this; }
    PipelineCreateInfo& set_p_input_assembly_state(const VkPipelineInputAssemblyStateCreateInfo* p_input_assembly_state) { _p_input_assembly_state = p_input_assembly_state; return *this; }
    PipelineCreateInfo& set_p_tessellation_state(const VkPipelineTessellationStateCreateInfo* p_tessellation_state) { _p_tessellation_state = p_tessellation_state; return *this; }
    PipelineCreateInfo& set_p_viewport_state(const VkPipelineViewportStateCreateInfo* p_viewport_state) { _p_viewport_state = p_viewport_state; return *this; }
    PipelineCreateInfo& set_p_rasterization_state(const VkPipelineRasterizationStateCreateInfo* p_rasterization_state) { _p_rasterization_state = p_rasterization_state; return *this; }
    PipelineCreateInfo& set_p_multisample_state(const VkPipelineMultisampleStateCreateInfo* p_multisample_state) { _p_multisample_state = p_multisample_state; return *this; }
    PipelineCreateInfo& set_p_depth_stencil_state(const VkPipelineDepthStencilStateCreateInfo* p_depth_stencil_state) { _p_depth_stencil_state = p_depth_stencil_state; return *this; }
    PipelineCreateInfo& set_p_color_blend_state(const VkPipelineColorBlendStateCreateInfo* p_color_blend_state) { _p_color_blend_state = p_color_blend_state; return *this; }
    PipelineCreateInfo& set_p_dynamic_state(const VkPipelineDynamicStateCreateInfo* p_dynamic_state) { _p_dynamic_state = p_dynamic_state; return *this; }
    PipelineCreateInfo& set_layout(VkPipelineLayout layout) { _layout = layout; return *this; }
    PipelineCreateInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    PipelineCreateInfo& set_subpass(uint32_t subpass) { _subpass = subpass; return *this; }
    PipelineCreateInfo& set_base_pipeline_handle(VkPipeline base_pipeline_handle) { _base_pipeline_handle = base_pipeline_handle; return *this; }
    PipelineCreateInfo& set_base_pipeline_index(int32_t base_pipeline_index) { _base_pipeline_index = base_pipeline_index; return *this; }

    VkGraphicsPipelineCreateInfo to_vk() const {
        VkGraphicsPipelineCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.stageCount = _stage_count;
        ci.pStages = _p_stages;
        ci.pVertexInputState = _p_vertex_input_state;
        ci.pInputAssemblyState = _p_input_assembly_state;
        ci.pTessellationState = _p_tessellation_state;
        ci.pViewportState = _p_viewport_state;
        ci.pRasterizationState = _p_rasterization_state;
        ci.pMultisampleState = _p_multisample_state;
        ci.pDepthStencilState = _p_depth_stencil_state;
        ci.pColorBlendState = _p_color_blend_state;
        ci.pDynamicState = _p_dynamic_state;
        ci.layout = _layout;
        ci.renderPass = _render_pass;
        ci.subpass = _subpass;
        ci.basePipelineHandle = _base_pipeline_handle;
        ci.basePipelineIndex = _base_pipeline_index;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkPipelineCreateFlags _flags = 0;
    uint32_t _stage_count = 0;
    const VkPipelineShaderStageCreateInfo* _p_stages = nullptr;
    const VkPipelineVertexInputStateCreateInfo* _p_vertex_input_state = nullptr;
    const VkPipelineInputAssemblyStateCreateInfo* _p_input_assembly_state = nullptr;
    const VkPipelineTessellationStateCreateInfo* _p_tessellation_state = nullptr;
    const VkPipelineViewportStateCreateInfo* _p_viewport_state = nullptr;
    const VkPipelineRasterizationStateCreateInfo* _p_rasterization_state = nullptr;
    const VkPipelineMultisampleStateCreateInfo* _p_multisample_state = nullptr;
    const VkPipelineDepthStencilStateCreateInfo* _p_depth_stencil_state = nullptr;
    const VkPipelineColorBlendStateCreateInfo* _p_color_blend_state = nullptr;
    const VkPipelineDynamicStateCreateInfo* _p_dynamic_state = nullptr;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    uint32_t _subpass = 0;
    VkPipeline _base_pipeline_handle = VK_NULL_HANDLE;
    int32_t _base_pipeline_index = -1;
};
    

class Pipeline {
public:
    Pipeline() noexcept = default;
    Pipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& ci)
        : _device(device) 
    {
        if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create graphics pipeline" << std::endl;
        }
    }

    ~Pipeline() {
        Destroy();
    }

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    Pipeline(Pipeline&& other) noexcept
        : _handle(other._handle),
          _device(other._device)
    {
        other._handle = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
    }

    Pipeline& operator=(Pipeline&& other) noexcept {
        if (this != &other) {
            Destroy();
            _handle = other._handle;
            _device = other._device;
            other._handle = VK_NULL_HANDLE;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkPipeline& handle() const { return _handle; }
private:
    void Destroy() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyPipeline(_device, _handle, nullptr);
            _handle = VK_NULL_HANDLE;
        }
    }

    VkPipeline _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
};

}

#endif

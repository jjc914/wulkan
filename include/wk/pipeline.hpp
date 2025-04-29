#ifndef wulkan_wk_PIPELINE_HPP
#define wulkan_wk_PIPELINE_HPP

#include "wulkan_internal.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>

namespace wk {

class ViewportInfo {
public:
    ViewportInfo& set_x(float x) { _x = x; return *this; }
    ViewportInfo& set_y(float y) { _y = y; return *this; }
    ViewportInfo& set_width(float width) { _width = width; return *this; }
    ViewportInfo& set_height(float height) { _height = height; return *this; }
    ViewportInfo& set_min_depth(float min_depth) { _min_depth = min_depth; return *this; }
    ViewportInfo& set_max_depth(float max_depth) { _max_depth = max_depth; return *this; }

    VkViewport to_vk_viewport() {
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

class ScissorInfo {
public:
    ScissorInfo& set_offset(VkOffset2D offset) { _offset = offset; return *this; }
    ScissorInfo& set_extent(VkExtent2D extent) { _extent = extent; return *this; }

    VkRect2D to_vk_rect_2D() {
        VkRect2D scissor{};
        scissor.offset = _offset;
        scissor.extent = _extent;
        return scissor;
    }
private:
    VkOffset2D _offset{};
    VkExtent2D _extent{};
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

class VertexAttributeDescription {
public:
    VertexAttributeDescription& set_binding(uint32_t binding) { _binding = binding; return *this; }
    VertexAttributeDescription& set_location(uint32_t location) { _location = location; return *this; }
    VertexAttributeDescription& set_format(VkFormat format) { _format = format; return *this; }
    VertexAttributeDescription& set_offset(uint32_t offset) { _offset = offset; return *this; }

    VkVertexInputAttributeDescription to_vk_vertex_input_attribute_description() {
        VkVertexInputAttributeDescription vkad{};
        vkad.binding = _binding;
        vkad.location = _location;
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

class PipelineCreateInfo {
public:
    PipelineCreateInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    PipelineCreateInfo& set_layout(VkPipelineLayout layout) { _layout = layout; return *this; }
    PipelineCreateInfo& set_vert_shader(VkShaderModule vert) { _vert_shader = vert; return *this; }
    PipelineCreateInfo& set_frag_shader(VkShaderModule frag) { _frag_shader = frag; return *this; }
    PipelineCreateInfo& set_viewport(VkViewport viewport) { _viewport = viewport; return *this; }
    PipelineCreateInfo& set_scissor(VkRect2D scissor) { _scissor = scissor; return *this; }
    PipelineCreateInfo& set_vertex_binding_descriptions(std::vector<VkVertexInputBindingDescription> bindings) { _vertex_bindings = bindings; return *this; }
    PipelineCreateInfo& set_vertex_attribute_descriptions(std::vector<VkVertexInputAttributeDescription> attributes) { _vertex_attributes = attributes; return *this; }

    VkGraphicsPipelineCreateInfo to_vk_graphics_pipeline_create_info() {
        // shader stages
        _shader_stages.resize(2);
        _shader_stages[0] = {};
        _shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        _shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        _shader_stages[0].module = _vert_shader;
        _shader_stages[0].pName = "main";
        _shader_stages[1] = {};
        _shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        _shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        _shader_stages[1].module = _frag_shader;
        _shader_stages[1].pName = "main";

        // dynamic states
        _dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        _dynamic_state_create_info = {};
        _dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        _dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(_dynamic_states.size());
        _dynamic_state_create_info.pDynamicStates = _dynamic_states.data();

        // vertex input
        _vertex_input_create_info = {};
        _vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        _vertex_input_create_info.vertexBindingDescriptionCount = static_cast<uint32_t>(_vertex_bindings.size());
        _vertex_input_create_info.pVertexBindingDescriptions = _vertex_bindings.data();
        _vertex_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(_vertex_attributes.size());
        _vertex_input_create_info.pVertexAttributeDescriptions = _vertex_attributes.data();

        // input assembly
        _input_assembly_create_info = {};
        _input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        _input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        _input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

        // viewport and scissor
        _viewport_state_create_info = {};
        _viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        _viewport_state_create_info.viewportCount = 1;
        _viewport_state_create_info.pViewports = &_viewport;
        _viewport_state_create_info.scissorCount = 1;
        _viewport_state_create_info.pScissors = &_scissor;

        // rasterization
        _rasterization_create_info = {};
        _rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        _rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        _rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        _rasterization_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        _rasterization_create_info.lineWidth = 1.0f;

        // multisample
        _multisample_create_info = {};
        _multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        _multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // color blending
        _color_blend_attachment = {};
        _color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                 VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        _color_blend_attachment.blendEnable = VK_TRUE;
        _color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        _color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        _color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        _color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        _color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        _color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        _color_blend_state_create_info = {};
        _color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        _color_blend_state_create_info.attachmentCount = 1;
        _color_blend_state_create_info.pAttachments = &_color_blend_attachment;

        // pipeline
        VkGraphicsPipelineCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        ci.stageCount = static_cast<uint32_t>(_shader_stages.size());
        ci.pStages = _shader_stages.data();
        ci.pVertexInputState = &_vertex_input_create_info;
        ci.pInputAssemblyState = &_input_assembly_create_info;
        ci.pViewportState = &_viewport_state_create_info;
        ci.pRasterizationState = &_rasterization_create_info;
        ci.pMultisampleState = &_multisample_create_info;
        ci.pColorBlendState = &_color_blend_state_create_info;
        ci.pDynamicState = &_dynamic_state_create_info;
        ci.layout = _layout;
        ci.renderPass = _render_pass;
        ci.subpass = 0;

        return ci;
    }
private:
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    VkShaderModule _vert_shader = VK_NULL_HANDLE;
    VkShaderModule _frag_shader = VK_NULL_HANDLE;
    VkViewport _viewport{};
    VkRect2D _scissor{};
    std::vector<VkVertexInputBindingDescription> _vertex_bindings{};
    std::vector<VkVertexInputAttributeDescription> _vertex_attributes{};

    std::vector<VkPipelineShaderStageCreateInfo> _shader_stages{};
    std::vector<VkDynamicState> _dynamic_states{};
    VkPipelineDynamicStateCreateInfo _dynamic_state_create_info{};
    VkPipelineVertexInputStateCreateInfo _vertex_input_create_info{};
    VkPipelineInputAssemblyStateCreateInfo _input_assembly_create_info{};
    VkPipelineViewportStateCreateInfo _viewport_state_create_info{};
    VkPipelineRasterizationStateCreateInfo _rasterization_create_info{};
    VkPipelineMultisampleStateCreateInfo _multisample_create_info{};
    VkPipelineColorBlendAttachmentState _color_blend_attachment{};
    VkPipelineColorBlendStateCreateInfo _color_blend_state_create_info{};
};

class Pipeline {
public:
    Pipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& ci)
        : _device(device), 
          _viewport(*ci.pViewportState->pViewports),
          _scissor(*ci.pViewportState->pScissors)
    {
        if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create graphics pipeline" << std::endl;
            std::exit(-1);
        }
    }

    ~Pipeline() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyPipeline(_device, _handle, nullptr);
        }
    }

    VkPipeline handle() const { return _handle; }
    VkViewport viewport() const { return _viewport; }
    VkRect2D scissor() const { return _scissor; }
private:
    VkPipeline _handle = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkViewport _viewport{};
    VkRect2D _scissor{};
};

}

#endif

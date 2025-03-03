#ifndef wulkan_wk_PIPELINE_HPP
#define wulkan_wk_PIPELINE_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

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
    float _x;
    float _y;
    float _width;
    float _height;
    float _min_depth;
    float _max_depth;

    friend class Pipeline;
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
    VkOffset2D _offset;
    VkExtent2D _extent;

    friend class Pipeline;
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
    uint32_t _binding;
    uint32_t _stride;
    VkVertexInputRate _input_rate;

    friend class Pipeline;
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

    friend class Pipeline;
};

class PushConstantRange {
public:
    PushConstantRange& set_stage_flags(VkShaderStageFlags flags) { _stage_flags = flags; return *this; }
    PushConstantRange& set_offset(uint32_t offset) { _offset = offset; return *this; }
    PushConstantRange& set_size(uint32_t size) { _size = size; return *this; }

    VkPushConstantRange to_vk_push_constant_range() {
        VkPushConstantRange pcr{};
        pcr.stageFlags = _stage_flags;
        pcr.offset = _offset;
        pcr.size = _size;
        return pcr;
    }
private:
    VkShaderStageFlags _stage_flags = 0;
    uint32_t _offset = 0;
    uint32_t _size = 0;

    friend class Pipeline;
};

class PipelineCreateInfo {
public:
    PipelineCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    PipelineCreateInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    PipelineCreateInfo& set_vert(VkShaderModule vert) { _vert = vert; return *this; }
    PipelineCreateInfo& set_frag(VkShaderModule frag) { _frag = frag; return *this; }
    PipelineCreateInfo& set_extent(VkExtent2D extent) { _extent = extent; return *this; }
    PipelineCreateInfo& set_viewport(VkViewport viewport) { _viewport = viewport; return *this; }
    PipelineCreateInfo& set_scissor(VkRect2D scissor) { _scissor = scissor; return *this; }
    PipelineCreateInfo& set_vertex_binding_descriptions(std::vector<VkVertexInputBindingDescription> descriptions) { _vertex_binding_descriptions = descriptions; return *this; }
    PipelineCreateInfo& set_vertex_attribute_descriptions(std::vector<VkVertexInputAttributeDescription> descriptions) { _vertex_attribute_descriptions = descriptions; return *this; }
    PipelineCreateInfo& set_push_constant_ranges(std::vector<VkPushConstantRange> ranges) { _push_constant_ranges = ranges; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    VkShaderModule _vert = VK_NULL_HANDLE;
    VkShaderModule _frag = VK_NULL_HANDLE;
    VkExtent2D _extent{};
    std::vector<VkVertexInputBindingDescription> _vertex_binding_descriptions{};
    std::vector<VkVertexInputAttributeDescription> _vertex_attribute_descriptions{};
    std::vector<VkPushConstantRange> _push_constant_ranges{};
    VkViewport _viewport{};
    VkRect2D _scissor{};

    friend class Pipeline;
};

class Pipeline {
public:
    Pipeline(const PipelineCreateInfo& ci) {
        std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos(2);

        shader_stage_create_infos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_create_infos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stage_create_infos[0].module = ci._vert;
        shader_stage_create_infos[0].pName = "main";
        shader_stage_create_infos[0].pSpecializationInfo = nullptr;

        shader_stage_create_infos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_create_infos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stage_create_infos[1].module = ci._frag;
        shader_stage_create_infos[1].pName = "main";
        shader_stage_create_infos[1].pSpecializationInfo = nullptr;

        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
        dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_create_info.dynamicStateCount =(uint32_t)dynamic_states.size();
        dynamic_state_create_info.pDynamicStates = dynamic_states.data();

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
        vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state_create_info.vertexBindingDescriptionCount = ci._vertex_binding_descriptions.size();
        vertex_input_state_create_info.pVertexBindingDescriptions = ci._vertex_binding_descriptions.data();
        vertex_input_state_create_info.vertexAttributeDescriptionCount = ci._vertex_attribute_descriptions.size();
        vertex_input_state_create_info.pVertexAttributeDescriptions = ci._vertex_attribute_descriptions.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{};
        input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state_create_info{};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.pViewports = &ci._viewport;
        viewport_state_create_info.scissorCount = 1;
        viewport_state_create_info.pScissors = &ci._scissor;
            
        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{};
        rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_state_create_info.depthClampEnable = VK_FALSE;
        rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_state_create_info.lineWidth = 1.0f;
        rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterization_state_create_info.depthBiasEnable = VK_FALSE;
        
        VkPipelineMultisampleStateCreateInfo multisample_state_create_info{};
        multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_state_create_info.sampleShadingEnable = VK_FALSE;
        multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{};
        
        VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
        color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment_state.blendEnable = VK_TRUE;
        color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
        
        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{};
        color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state_create_info.logicOpEnable = VK_FALSE;
        color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_state_create_info.attachmentCount = 1;
        color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
        color_blend_state_create_info.blendConstants[0] = 0.0f;
        color_blend_state_create_info.blendConstants[1] = 0.0f;
        color_blend_state_create_info.blendConstants[2] = 0.0f;
        color_blend_state_create_info.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.setLayoutCount = 0;
        pipeline_layout_create_info.pushConstantRangeCount = ci._push_constant_ranges.size();
        pipeline_layout_create_info.pPushConstantRanges = ci._push_constant_ranges.data();

        if (vkCreatePipelineLayout(ci._device, &pipeline_layout_create_info, nullptr, &_layout) != VK_SUCCESS) {
            std::cerr << "failed to create pipeline layout" << std::endl;
        }
        std::clog << "created pipeline layout" << std::endl;

        VkGraphicsPipelineCreateInfo pipeline_create_info{};
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.stageCount = shader_stage_create_infos.size();
        pipeline_create_info.pStages = shader_stage_create_infos.data();
        pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
        pipeline_create_info.pViewportState = &viewport_state_create_info;
        pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
        pipeline_create_info.pMultisampleState = &multisample_state_create_info;
        pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
        pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
        pipeline_create_info.pDynamicState = &dynamic_state_create_info;
        
        pipeline_create_info.layout = _layout;
        pipeline_create_info.renderPass = ci._render_pass;
        pipeline_create_info.subpass = 0;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
        
        if (vkCreateGraphicsPipelines(ci._device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create graphics pipeline" << std::endl;
        }

        _device = ci._device;
        _scissor = ci._scissor;
        _viewport = ci._viewport;

        std::clog << "created graphics pipeline" << std::endl;
    }

    ~Pipeline() {
        vkDestroyPipeline(_device, _handle, nullptr);
        vkDestroyPipelineLayout(_device, _layout, nullptr);
    }

    VkPipeline handle() const { return _handle; }
    VkViewport viewport() const { return _viewport; }
    VkRect2D scissor() const { return _scissor; }
    VkPipelineLayout layout() const { return _layout; }
private:
    VkPipeline _handle;
    VkPipelineLayout _layout;
    VkDevice _device;
    VkViewport _viewport;
    VkRect2D _scissor;
};

}

#endif
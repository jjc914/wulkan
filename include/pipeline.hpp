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

class PipelineCreateInfo {
public:
    PipelineCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
    PipelineCreateInfo& set_render_pass(VkRenderPass render_pass) { _render_pass = render_pass; return *this; }
    PipelineCreateInfo& set_vert(VkShaderModule vert) { _vert = vert; return *this; }
    PipelineCreateInfo& set_frag(VkShaderModule frag) { _frag = frag; return *this; }
    PipelineCreateInfo& set_extent(VkExtent2D extent) { _extent = extent; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    VkShaderModule _vert = VK_NULL_HANDLE;
    VkShaderModule _frag = VK_NULL_HANDLE;
    VkExtent2D _extent{};

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
        vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
        vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
        vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
        vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{};
        input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = ci._extent.width;
        viewport.height = ci._extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = ci._extent;

        VkPipelineViewportStateCreateInfo viewport_state_create_info{};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.scissorCount = 1;
            
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
        pipeline_layout_create_info.pushConstantRangeCount = 0;

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

        std::clog << "created graphics pipeline" << std::endl;
    }

    ~Pipeline() {
        vkDestroyPipeline(_device, _handle, nullptr);
        vkDestroyPipelineLayout(_device, _layout, nullptr);
    }

    VkPipeline handle() const { return _handle; }
private:
    VkPipeline _handle;
    VkPipelineLayout _layout;
    VkDevice _device;
};

}

#endif
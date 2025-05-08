#define WLK_ENABLE_VALIDATION_LAYERS
#include <wk/wulkan.hpp>
#include <wk/ext/glfw/glfw_internal.hpp>
#include <wk/ext/glfw/surface.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <algorithm>
#include <cstring>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

int main() {
    std::vector<Vertex> vertices = {
        {{-1.0f, -1.0f,  1.0f}, {1, 0, 0}}, // front
        {{ 1.0f, -1.0f,  1.0f}, {0, 1, 0}},
        {{ 1.0f,  1.0f,  1.0f}, {0, 0, 1}},
        {{-1.0f,  1.0f,  1.0f}, {1, 1, 0}},

        {{-1.0f, -1.0f, -1.0f}, {1, 0, 1}}, // back
        {{ 1.0f, -1.0f, -1.0f}, {0, 1, 1}},
        {{ 1.0f,  1.0f, -1.0f}, {1, 1, 1}},
        {{-1.0f,  1.0f, -1.0f}, {0, 0, 0}},
    };

    std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0, // front
        1, 5, 6, 6, 2, 1, // right
        7, 6, 5, 5, 4, 7, // back
        4, 0, 3, 3, 7, 4, // left
        3, 2, 6, 6, 7, 3, // top
        4, 5, 1, 1, 0, 4, // bottom
    };

    int width = 900;
    int height = 600;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(width, height, "example1_basic", nullptr, nullptr);

#ifdef WLK_ENABLE_VALIDATION_LAYERS
    VkDebugUtilsMessengerCreateInfoEXT debug_ci = wk::DebugMessengerCreateInfo{}
        .set_message_severity(
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        .set_message_type(
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        .set_user_callback(wk::DefaultDebugMessengerCallback)
        .to_vk_debug_messenger_create_info();
#endif

    VkResult result;
    std::vector<const char*> instance_extensions = wk::ext::glfw::GetDefaultGlfwRequiredInstanceExtensions();
    std::vector<const char*> instance_layers;
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    VkApplicationInfo application_info = wk::ApplicationInfo{}
        .set_application_name("vulkantest")
        .set_application_version(VK_MAKE_VERSION(1, 0, 0))
        .set_engine_name("No Engine")
        .set_engine_version(VK_MAKE_VERSION(1, 0, 0))
        .set_api_version(VK_API_VERSION_1_3)
        .to_vk_application_info();
    wk::Instance instance(
        wk::InstanceCreateInfo{}
            .set_flags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR)
            .set_extensions(instance_extensions.size(), instance_extensions.data())
            .set_layers(instance_layers.size(), instance_layers.data())
            .set_application_info(&application_info)
#ifdef WLK_ENABLE_VALIDATION_LAYERS
            .set_p_next(&debug_ci)
#endif
            .to_vk_instance_create_info()
    );
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    wk::DebugMessenger debugMessenger(instance.handle(), debug_ci);
#endif
    wk::ext::glfw::Surface surface(instance.handle(), window);
    wk::PhysicalDevice physical_device(instance.handle(), surface.handle(), wk::GetDefaultRequiredDeviceExtensions());
    wk::PhysicalDeviceSurfaceSupport physical_device_support = wk::GetPhysicalDeviceSurfaceSupport(physical_device.handle(), surface.handle());
    wk::DeviceQueueFamilyIndices queue_family_indices = physical_device.queue_family_indices();

    const float queue_priorities = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    if (queue_family_indices.is_unique()) {
        queue_create_infos = {
            wk::DeviceQueueCreateInfo{}
                .set_queue_family_index(queue_family_indices.graphics_family.value())
                .set_queue_count(1)
                .set_p_queue_priorities(&queue_priorities)
                .to_vk_device_queue_create_info(),
            wk::DeviceQueueCreateInfo{}
                .set_queue_family_index(queue_family_indices.present_family.value())
                .set_queue_count(1)
                .set_p_queue_priorities(&queue_priorities)
                .to_vk_device_queue_create_info()
        };
    } else {
        queue_create_infos = {
            wk::DeviceQueueCreateInfo{}
                .set_queue_family_index(queue_family_indices.graphics_family.value())
                .set_queue_count(1)
                .set_p_queue_priorities(&queue_priorities)
                .to_vk_device_queue_create_info()
        };
    }
    wk::Device device(physical_device.handle(), queue_family_indices, surface.handle(), 
        wk::DeviceCreateInfo{}
            .set_p_enabled_features(&physical_device.features())
            .set_enabled_extensions(physical_device.extensions().size(), physical_device.extensions().data())
            .set_queue_create_infos(queue_create_infos.size(), queue_create_infos.data())
            .to_vk_device_create_info()
    );

    VmaVulkanFunctions vulkan_functions{};
    wk::Allocator allocator(
        wk::AllocatorCreateInfo{}
            .set_vulkan_api_version(VK_API_VERSION_1_3)
            .set_instance(instance.handle())
            .set_physical_device(physical_device.handle())
            .set_device(device.handle())
            .set_p_vulkan_functions(&vulkan_functions)
            .to_vk_allocator_create_info()
    );

    std::vector<uint32_t> queue_family_indices_vec = queue_family_indices.to_vec();
    VkSurfaceFormatKHR surface_format = wk::ChooseSurfaceFormat(physical_device_support.formats);
    const size_t max_frames_in_flight = 2;
    wk::Swapchain swapchain(device.handle(), 
        wk::SwapchainCreateInfo{}
            .set_surface(surface.handle())
            .set_present_mode(wk::ChooseSurfacePresentationMode(physical_device_support.present_modes))
            .set_min_image_count(std::clamp(
                physical_device_support.capabilities.minImageCount + 1, 
                physical_device_support.capabilities.minImageCount, 
                physical_device_support.capabilities.maxImageCount))
            .set_image_extent(wk::ChooseSurfaceExtent(width, height, physical_device_support.capabilities))
            .set_image_format(surface_format.format)
            .set_image_color_space(surface_format.colorSpace)
            .set_image_array_layers(1)
            .set_image_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .set_queue_family_indices(2, queue_family_indices_vec.data())
            .to_vk_swapchain_create_info()
    );

    VkFormat depth_format = wk::ChooseDepthFormat(physical_device.handle(), { 
        VK_FORMAT_D32_SFLOAT, 
        VK_FORMAT_D32_SFLOAT_S8_UINT, 
        VK_FORMAT_D24_UNORM_S8_UINT
    });
    VkAttachmentDescription attachments[2] = {
        wk::AttachmentDescription{}
            .set_flags(0)
            .set_format(swapchain.image_format())
            .set_samples(VK_SAMPLE_COUNT_1_BIT)
            .set_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .set_store_op(VK_ATTACHMENT_STORE_OP_STORE)
            .set_stencil_load_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .set_stencil_store_op(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
            .set_final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
            .to_vk_attachment_description(),
        wk::AttachmentDescription{}
            .set_flags(0)
            .set_format(depth_format)
            .set_samples(VK_SAMPLE_COUNT_1_BIT)
            .set_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .set_store_op(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .set_stencil_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .set_stencil_store_op(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
            .set_final_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .to_vk_attachment_description()
    };
    
    VkAttachmentReference color_attachment_ref = wk::AttachmentReference{}
        .set_attachment(0)
        .set_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        .to_vk_attachment_reference();

    VkAttachmentReference depth_attachment_ref = wk::AttachmentReference{}
        .set_attachment(1)
        .set_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        .to_vk_attachment_reference();

    VkSubpassDescription subpass = wk::SubpassDescription{}
        .set_pipeline_bind_point(VK_PIPELINE_BIND_POINT_GRAPHICS)
        .set_color_attachments(1, &color_attachment_ref)
        .set_depth_stencil_attachment(&depth_attachment_ref)
        .to_vk_subpass_description();

    VkSubpassDependency dependency = wk::SubpassDependency{}
        .set_src_subpass(VK_SUBPASS_EXTERNAL)
        .set_dst_subpass(0)
        .set_src_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        .set_dst_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        .set_src_access_mask(0)
        .set_dst_access_mask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
        .to_vk_subpass_dependency();

    wk::RenderPass render_pass(device.handle(),
        wk::RenderPassCreateInfo{}
            .set_attachments(2, attachments)
            .set_subpasses(1, &subpass)
            .set_dependencies(1, &dependency)
            .to_vk_render_pass_create_info()
    );

    wk::CommandPool command_pool(device.handle(),
        wk::CommandPoolCreateInfo{}
            .set_queue_family_index(queue_family_indices.graphics_family.value())
            .to_vk_command_pool_create_info()
    );

    std::vector<wk::Framebuffer> framebuffers;
    std::vector<wk::ImageView> depth_image_views;
    std::vector<wk::Image> depth_images;
    framebuffers.reserve(swapchain.image_views().size());
    depth_images.reserve(swapchain.image_views().size());
    depth_image_views.reserve(swapchain.image_views().size());
    for (int i = 0; i < swapchain.image_views().size(); ++i) {
        depth_images.emplace_back(allocator.handle(), 
            wk::ImageCreateInfo{}
                .set_image_type(VK_IMAGE_TYPE_2D)
                .set_format(depth_format)
                .set_extent(wk::Extent(swapchain.extent()).to_vk_extent_3d())
                .set_mip_levels(1)
                .set_array_layers(1)
                .set_samples(VK_SAMPLE_COUNT_1_BIT)
                .set_tiling(VK_IMAGE_TILING_OPTIMAL)
                .set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
                .to_vk_image_create_info(),
            wk::AllocationCreateInfo{}
                .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
                .to_vma_allocation_create_info()
        );
        depth_image_views.emplace_back(device.handle(),
            wk::ImageViewCreateInfo{}
                .set_image(depth_images[i].handle())
                .set_view_type(VK_IMAGE_VIEW_TYPE_2D)
                .set_format(depth_format)
                .set_components(wk::ComponentMapping::identity().to_vk_component_mapping())
                .set_subresource_range(
                    wk::ImageSubresourceRange{}
                        .set_aspect_mask(wk::GetAspectFlags(depth_format))
                        .set_base_mip_level(0)
                        .set_level_count(1)
                        .set_base_array_layer(0)
                        .set_layer_count(1)
                        .to_vk_image_subresource_range()
                )
                .to_vk_image_view_create_info()
        );
        std::vector<VkImageView> attachments = { swapchain.image_views()[i], depth_image_views[i].handle() };
        framebuffers.emplace_back(device.handle(), 
            wk::FramebufferCreateInfo{}
                .set_render_pass(render_pass.handle())
                .set_attachments(attachments.size(), attachments.data())
                .set_extent(swapchain.extent())
                .set_layers(1)
                .to_vk_framebuffer_create_info()
        );
    }

    std::vector<wk::CommandBuffer> command_buffers{};
    std::vector<wk::Semaphore> image_available_semaphores{};
    std::vector<wk::Semaphore> render_finished_semaphores{};
    command_buffers.reserve(max_frames_in_flight);
    image_available_semaphores.reserve(max_frames_in_flight);
    render_finished_semaphores.reserve(max_frames_in_flight);
    for (int i = 0; i < max_frames_in_flight; ++i) {
        command_buffers.emplace_back(device.handle(),
            wk::CommandBufferAllocateInfo{}
                .set_command_pool(command_pool.handle())
                .to_vk_command_buffer_allocate_info()
        );
        image_available_semaphores.emplace_back(device.handle(),
            wk::SemaphoreCreateInfo{}.to_vk_semaphore_create_info()
        );
        render_finished_semaphores.emplace_back(device.handle(),
            wk::SemaphoreCreateInfo{}.to_vk_semaphore_create_info()
        );
    }

    std::vector<wk::Fence> frame_in_flight_fences{};
    frame_in_flight_fences.reserve(max_frames_in_flight);
    for (int i = 0; i < max_frames_in_flight; ++i) {
        frame_in_flight_fences.emplace_back(device.handle(),
            wk::FenceCreateInfo{}
                .set_flags(VK_FENCE_CREATE_SIGNALED_BIT)
                .to_vk_fence_create_info()
        );
    }

    std::vector<uint8_t> vert_byte_code = wk::ReadSpirvShader("build/bin/example1_basic/shaders/triangle.vert.spv");
    std::vector<uint8_t> frag_byte_code = wk::ReadSpirvShader("build/bin/example1_basic/shaders/triangle.frag.spv");
    wk::Shader vert(device.handle(),
        wk::ShaderCreateInfo{}
            .set_byte_code(vert_byte_code.size(), vert_byte_code.data())
            .to_vk_shader_module_create_info()
    );
    wk::Shader frag(device.handle(),
        wk::ShaderCreateInfo{}
            .set_byte_code(frag_byte_code.size(), frag_byte_code.data())
            .to_vk_shader_module_create_info()
    );
    
    VkDescriptorSetLayoutBinding bindings[] = {
        wk::DescriptorSetLayoutBinding{}
            .set_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(1)
            .set_stage_flags(VK_SHADER_STAGE_VERTEX_BIT)
            .to_vk_descriptor_set_layout_binding()
    };
    wk::DescriptorSetLayout descriptor_set_layout(device.handle(),
        wk::DescriptorSetLayoutCreateInfo{}
            .set_bindings(1, bindings)
            .to_vk_descriptor_set_layout_create_info()
    );

    VkDescriptorPoolSize pool_sizes[] = {
        wk::DescriptorPoolSize{}
            .set_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(1)
            .to_vk_descriptor_pool_size()
    };
    wk::DescriptorPool descriptor_pool(device.handle(),
        wk::DescriptorPoolCreateInfo{}
            .set_max_sets(1)
            .set_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .set_pool_sizes(1, pool_sizes)
            .to_vk_descriptor_pool_create_info()
    );

    VkDescriptorSetLayout layouts[] = { descriptor_set_layout.handle() };
    wk::DescriptorSet descriptor_set(device.handle(),
        wk::DescriptorSetAllocateInfo{}
            .set_descriptor_pool(descriptor_pool.handle())
            .set_set_layouts(1, layouts)
            .to_vk_descriptor_set_allocate_info()
    );

    wk::PipelineLayout pipeline_layout(device.handle(), 
        wk::PipelineLayoutCreateInfo{}
            .set_set_layouts(1, layouts)
            .set_push_constant_ranges(0, nullptr)
            .to_vk_pipeline_layout_create_info()
    );

    VkPipelineShaderStageCreateInfo shader_stages[2] = {
        wk::PipelineShaderStageCreateInfo{}
            .set_stage(VK_SHADER_STAGE_VERTEX_BIT)
            .set_module(vert.handle())
            .set_p_name("main")
            .to_vk_pipeline_shader_stage_create_info(),
        wk::PipelineShaderStageCreateInfo{}
            .set_stage(VK_SHADER_STAGE_FRAGMENT_BIT)
            .set_module(frag.handle())
            .set_p_name("main")
            .to_vk_pipeline_shader_stage_create_info()
    };

    VkVertexInputBindingDescription vertex_binding = wk::VertexInputBindingDescription{}
        .set_binding(0)
        .set_stride(sizeof(Vertex))
        .set_input_rate(VK_VERTEX_INPUT_RATE_VERTEX)
        .to_vk_vertex_input_binding_description();

    VkVertexInputAttributeDescription vertex_attributes[2] = {
        wk::VertexInputAttributeDescription{}
            .set_binding(0)
            .set_location(0)
            .set_format(VK_FORMAT_R32G32B32_SFLOAT)
            .set_offset(offsetof(Vertex, position))
            .to_vk_vertex_input_attribute_description(),
        wk::VertexInputAttributeDescription{}
            .set_binding(0)
            .set_location(1)
            .set_format(VK_FORMAT_R32G32B32_SFLOAT)
            .set_offset(offsetof(Vertex, color))
            .to_vk_vertex_input_attribute_description(),
    };

    VkViewport viewport = wk::Viewport{}
        .set_x(0.0f)
        .set_y(0.0f)
        .set_width(static_cast<float>(swapchain.extent().width))
        .set_height(static_cast<float>(swapchain.extent().height))
        .set_min_depth(0.0f)
        .set_max_depth(1.0f)
        .to_vk_viewport();

    VkRect2D scissor = wk::Rect2D{}
        .set_offset({ 0, 0 })
        .set_extent(swapchain.extent())
        .to_vk_rect_2D();

    VkPipelineViewportStateCreateInfo viewport_state_ci = wk::PipelineViewportStateCreateInfo{}
        .set_viewports(1, &viewport)
        .set_scissors(1, &scissor)
        .to_vk_pipeline_viewport_state_create_info();

    VkPipelineRasterizationStateCreateInfo raster_ci = wk::PipelineRasterizationStateCreateInfo{}
        .set_polygon_mode(VK_POLYGON_MODE_FILL)
        .set_cull_mode(VK_CULL_MODE_BACK_BIT)
        .set_front_face(VK_FRONT_FACE_COUNTER_CLOCKWISE)
        .set_line_width(1.0f)
        .to_vk_pipeline_rasterization_state_create_info();

    VkPipelineMultisampleStateCreateInfo multisample_ci = wk::PipelineMultisampleStateCreateInfo{}
        .set_rasterization_samples(VK_SAMPLE_COUNT_1_BIT)
        .to_vk_pipeline_multisample_state_create_info();
        
    VkPipelineColorBlendAttachmentState color_blend_attachment = wk::PipelineColorBlendAttachmentState{}
        .set_blend_enable(VK_TRUE)
        .set_color_write_mask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
        .set_src_color_blend_factor(VK_BLEND_FACTOR_SRC_ALPHA)
        .set_dst_color_blend_factor(VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
        .set_color_blend_op(VK_BLEND_OP_ADD)
        .set_src_alpha_blend_factor(VK_BLEND_FACTOR_ONE)
        .set_dst_alpha_blend_factor(VK_BLEND_FACTOR_ZERO)
        .set_alpha_blend_op(VK_BLEND_OP_ADD)
        .to_vk_pipeline_color_blend_attachment_state();

    VkPipelineColorBlendStateCreateInfo color_blend_state_ci = wk::PipelineColorBlendStateCreateInfo{}
        .set_attachments(1, &color_blend_attachment)
        .to_vk_pipeline_color_blend_state_create_info();

    VkDynamicState dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamic_ci = wk::PipelineDynamicStateCreateInfo{}
        .set_dynamic_states(2, dynamic_states)
        .to_vk_pipeline_dynamic_state_create_info();
    
    VkPipelineVertexInputStateCreateInfo vertex_input_ci = wk::PipelineVertexInputStateCreateInfo{}
        .set_vertex_binding_descriptions(1, &vertex_binding)
        .set_vertex_attribute_descriptions(2, vertex_attributes)
        .to_vk_pipeline_vertex_input_state_create_info();

    VkPipelineInputAssemblyStateCreateInfo input_assembly_ci = wk::PipelineInputAssemblyStateCreateInfo{}
        .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .set_primitive_restart_enable(VK_FALSE)
        .to_vk_pipeline_input_assembly_state_create_info();
    
    VkPipelineDepthStencilStateCreateInfo depth_stencil_ci = wk::PipelineDepthStencilStateCreateInfo{}
        .set_depth_test_enable(VK_TRUE)
        .set_depth_write_enable(VK_TRUE)
        .set_depth_compare_op(VK_COMPARE_OP_LESS)
        .set_depth_bounds_test_enable(VK_FALSE)
        .set_stencil_test_enable(VK_FALSE)
        .to_vk_pipeline_depth_stencil_state_create_info();

    wk::Pipeline pipeline(device.handle(), 
        wk::PipelineCreateInfo{}
            .set_stages(2, shader_stages)
            .set_p_vertex_input_state(&vertex_input_ci)
            .set_p_input_assembly_state(&input_assembly_ci)
            .set_p_viewport_state(&viewport_state_ci)
            .set_p_rasterization_state(&raster_ci)
            .set_p_multisample_state(&multisample_ci)
            .set_p_color_blend_state(&color_blend_state_ci)
            .set_p_dynamic_state(&dynamic_ci)
            .set_p_depth_stencil_state(&depth_stencil_ci)
            .set_layout(pipeline_layout.handle())
            .set_render_pass(render_pass.handle())
            .set_subpass(0)
            .to_vk_graphics_pipeline_create_info()
    );

    wk::Buffer vertex_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(vertices.size() * sizeof(Vertex))
            .set_usage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk_buffer_create_info(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vma_allocation_create_info()
    );

    wk::Buffer index_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(indices.size() * sizeof(uint16_t))
            .set_usage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
            .to_vk_buffer_create_info(),
        wk::AllocationCreateInfo{}
            .to_vma_allocation_create_info()
    );

    // staging
    wk::Buffer vertex_staging_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(vertices.size() * sizeof(Vertex))
            .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk_buffer_create_info(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
            .to_vma_allocation_create_info()
    );

    wk::Buffer index_staging_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(indices.size() * sizeof(uint16_t))
            .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk_buffer_create_info(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
            .to_vma_allocation_create_info()
    );

    void* vertex_data;
    vmaMapMemory(allocator.handle(), vertex_staging_buffer.allocation(), &vertex_data);
    memcpy(vertex_data, vertices.data(), vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(allocator.handle(), vertex_staging_buffer.allocation());

    void* index_data;
    vmaMapMemory(allocator.handle(), index_staging_buffer.allocation(), &index_data);
    memcpy(index_data, indices.data(), indices.size() * sizeof(uint16_t));
    vmaUnmapMemory(allocator.handle(), index_staging_buffer.allocation());

    wk::CommandBuffer upload_command_buffer(device.handle(),
        wk::CommandBufferAllocateInfo{}
                .set_command_pool(command_pool.handle())
                .to_vk_command_buffer_allocate_info()
    );
    
    VkCommandBufferBeginInfo upload_begin_info = wk::CommandBufferBeginInfo{}
        .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
        .to_vk_command_buffer_begin_info();
    result = vkBeginCommandBuffer(upload_command_buffer.handle(), &upload_begin_info);
    if (result != VK_SUCCESS) {
        std::cerr << "could not begin command buffer" << std::endl;
        return -1;
    }
    
    VkBufferCopy vertex_buffer_copy = wk::BufferCopy{}
        .set_size(vertices.size() * sizeof(Vertex))
        .to_vk_buffer_copy();
    vkCmdCopyBuffer(upload_command_buffer.handle(), 
        vertex_staging_buffer.handle(), 
        vertex_buffer.handle(), 
        1, 
        &vertex_buffer_copy
    );
    VkBufferCopy index_buffer_copy = wk::BufferCopy{}
        .set_size(indices.size() * sizeof(uint16_t))
        .to_vk_buffer_copy();
    vkCmdCopyBuffer(upload_command_buffer.handle(), 
        index_staging_buffer.handle(), 
        index_buffer.handle(), 
        1, 
        &index_buffer_copy
    );

    result = vkEndCommandBuffer(upload_command_buffer.handle());
    if (result != VK_SUCCESS) {
        std::cerr << "failed to end command buffer" << std::endl;
        return -1;
    }

    std::vector<VkCommandBuffer> upload_command_buffers = { upload_command_buffer.handle() };
    VkSubmitInfo gq_submit_info = wk::GraphicsQueueSubmitInfo{}
        .set_command_buffers(upload_command_buffers.size(), upload_command_buffers.data())
        .to_vk_submit_info();
    result = vkQueueSubmit(device.graphics_queue(), 1, &gq_submit_info, VK_NULL_HANDLE);
    if (result != VK_SUCCESS) {
        std::cerr << "failed to submit queue" << std::endl;
        return -1;
    }
    vkDeviceWaitIdle(device.handle());

    wk::Buffer uniform_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(sizeof(UniformBufferObject))
            .set_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk_buffer_create_info(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
            .to_vma_allocation_create_info()
    );

    VkDescriptorBufferInfo db_info = wk::DescriptorBufferInfo{}
        .set_buffer(uniform_buffer.handle())
        .set_offset(0)
        .set_range(sizeof(UniformBufferObject))
        .to_vk_descriptor_buffer_info();
    VkWriteDescriptorSet write_descriptor_set = wk::WriteDescriptorSet{}
        .set_dst_set(descriptor_set.handle())
        .set_dst_binding(0)
        .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        .set_descriptor_count(1)
        .set_p_buffer_info(&db_info)
        .to_vk_write_descriptor_set();
    vkUpdateDescriptorSets(device.handle(), 1, &write_descriptor_set, 0, nullptr);

    bool is_swapchain_outdated = false;
    size_t current_frame_in_flight = 0;
    while (!glfwWindowShouldClose(window)) {
        vkWaitForFences(device.handle(), 1, &frame_in_flight_fences[current_frame_in_flight].handle(), VK_TRUE, UINT64_MAX);
        glfwPollEvents();

        uint32_t available_image_index = 0;
        result = vkAcquireNextImageKHR(device.handle(), swapchain.handle(), UINT32_MAX, 
            image_available_semaphores[current_frame_in_flight].handle(), VK_NULL_HANDLE, &available_image_index
        );
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            is_swapchain_outdated = true;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            std::cerr << "failed to acquire swapchain image" << std::endl;
            return -1;
        }
        result = vkGetFenceStatus(device.handle(), frame_in_flight_fences[current_frame_in_flight].handle());
        vkResetFences(device.handle(), 1, &frame_in_flight_fences[current_frame_in_flight].handle());
        vkResetCommandBuffer(command_buffers[current_frame_in_flight].handle(), 0);

        VkCommandBufferBeginInfo cb_begin_info = wk::CommandBufferBeginInfo{}.to_vk_command_buffer_begin_info();
        result = vkBeginCommandBuffer(command_buffers[current_frame_in_flight].handle(), &cb_begin_info);
        if (result != VK_SUCCESS) {
            std::cerr << "could not begin command buffer" << std::endl;
            return -1;
        }

        std::vector<VkClearValue> clear_values = {
            { 0.0f, 0.0f, 0.0f, 1.0f }, 
            wk::ClearValue{}
                .set_color(0.0f, 0.0f, 0.0f, 1.0f)
                .set_depth_stencil(1.0f, 0)
                .to_vk_clear_value()};
        VkRenderPassBeginInfo rp_begin_info = wk::RenderPassBeginInfo{}
            .set_render_pass(render_pass.handle())
            .set_framebuffer(framebuffers[available_image_index].handle())
            .set_render_area({ { 0, 0 }, swapchain.extent() })
            .set_clear_values(clear_values.size(), clear_values.data())
            .to_vk_render_pass_begin_info();
        vkCmdBeginRenderPass(command_buffers[current_frame_in_flight].handle(), 
            &rp_begin_info,
            VK_SUBPASS_CONTENTS_INLINE
        );

        vkCmdBindPipeline(command_buffers[current_frame_in_flight].handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle());
        vkCmdSetViewport(command_buffers[current_frame_in_flight].handle(), 0, 1, &pipeline.viewport());
        vkCmdSetScissor(command_buffers[current_frame_in_flight].handle(), 0, 1, &pipeline.scissor());

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        ubo.proj = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 100.0f);
        ubo.proj[1][1] *= -1; // flip y for vulkan

        void* data;
        vmaMapMemory(allocator.handle(), uniform_buffer.allocation(), &data);
        memcpy(data, &ubo, sizeof(ubo));
        vmaUnmapMemory(allocator.handle(), uniform_buffer.allocation());

        vkCmdBindDescriptorSets(
            command_buffers[current_frame_in_flight].handle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout.handle(),
            0, 1, &descriptor_set.handle(),
            0, nullptr
        );

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(command_buffers[current_frame_in_flight].handle(), 0, 1, &vertex_buffer.handle(), &offset);
        vkCmdBindIndexBuffer(command_buffers[current_frame_in_flight].handle(), index_buffer.handle(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffers[current_frame_in_flight].handle(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        
        vkCmdEndRenderPass(command_buffers[current_frame_in_flight].handle());
        result = vkEndCommandBuffer(command_buffers[current_frame_in_flight].handle());
        if (result != VK_SUCCESS) {
            std::cerr << "failed to end command buffer" << std::endl;
            return -1;
        }

        std::vector<VkCommandBuffer> gq_command_buffers = { command_buffers[current_frame_in_flight].handle() };
        std::vector<VkSemaphore> gq_wait_semaphores = { image_available_semaphores[current_frame_in_flight].handle() };
        std::vector<VkPipelineStageFlags> gq_wait_stage_flags = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        std::vector<VkSemaphore> gq_signal_semaphores = { render_finished_semaphores[current_frame_in_flight].handle() };
        gq_submit_info = wk::GraphicsQueueSubmitInfo{}
            .set_command_buffers(gq_command_buffers.size(), gq_command_buffers.data())
            .set_wait_semaphores(gq_wait_semaphores.size(), gq_wait_semaphores.data())
            .set_wait_dst_stage_masks(gq_wait_stage_flags.size(), gq_wait_stage_flags.data())
            .set_signal_semaphores(gq_signal_semaphores.size(), gq_signal_semaphores.data())
            .to_vk_submit_info();
        result = vkQueueSubmit(device.graphics_queue(), 1, &gq_submit_info, frame_in_flight_fences[current_frame_in_flight].handle());
        if (result != VK_SUCCESS) {
            std::cerr << "failed to submit queue" << std::endl;
            return -1;
        }

        std::vector<VkSwapchainKHR> pq_swapchains = { swapchain.handle() };
        std::vector<uint32_t> pq_image_indices = { available_image_index };
        VkPresentInfoKHR present_info = wk::PresentInfo{}
            .set_swapchains(pq_swapchains.size(), pq_swapchains.data())
            .set_image_indices(pq_image_indices.data())
            .set_wait_semaphores(gq_signal_semaphores.size(), gq_signal_semaphores.data())
            .to_vk_present_info();
        result = vkQueuePresentKHR(device.present_queue(), &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            is_swapchain_outdated = true;
            return -1;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            std::cerr << "failed to present" << std::endl;
            return -1;
        }
        
        current_frame_in_flight = (current_frame_in_flight + 1) % max_frames_in_flight;
    }

    vkDeviceWaitIdle(device.handle());
    glfwTerminate();
}
#include "app.hpp"

#include <wk/wulkan_internal.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>

App::App() {

}

App::~App() {

}

// ------------------------- public -------------------------

int App::run() {
    if (_init_window()) return 1;
    if (_init_vulkan()) return 1;
    if (_main_loop()) return 1;
    _cleanup();
    return 0;
}

// ------------------------- window -------------------------

int App::_init_window() {
    glfwSetErrorCallback(wk::ext::glfw::DefaultGlfwErrorCallback);
    if (!glfwInit()) {
        std::cerr << "glfw failed to init" << std::endl;
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(_WIDTH, _HEIGHT, "example1_basic", nullptr, nullptr);
    if (!glfwVulkanSupported()) {
        std::cerr << "glfw vulkan not supported" << std::endl;
        return 1;
    }
    return 0;
}

// ------------------------- vulkan -------------------------

int App::_init_vulkan() {
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
        .to_vk();
#endif

    std::vector<const char*> instance_extensions = wk::ext::glfw::GetDefaultGlfwRequiredInstanceExtensions();
    std::vector<const char*> instance_layers;
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    VkApplicationInfo application_info = wk::ApplicationInfo{}
        .set_application_name("1_basic")
        .set_application_version(VK_MAKE_VERSION(1, 0, 0))
        .set_engine_name("No Engine")
        .set_engine_version(VK_MAKE_VERSION(1, 0, 0))
        .set_api_version(VK_API_VERSION_1_3)
        .to_vk();

    _instance = wk::Instance(
        wk::InstanceCreateInfo{}
            .set_flags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR)
            .set_extensions(instance_extensions.size(), instance_extensions.data())
            .set_layers(instance_layers.size(), instance_layers.data())
            .set_p_application_info(&application_info)
#ifdef WLK_ENABLE_VALIDATION_LAYERS
            .set_p_next(&debug_ci)
#endif
            .to_vk()
    );
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    _debug_messenger = wk::DebugMessenger(_instance.handle(), debug_ci);
#endif
    _surface = wk::ext::glfw::Surface(_instance.handle(), _window);

    // ---------- device ----------
    VkPhysicalDeviceFeatures2 physical_device_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    _physical_device = wk::PhysicalDevice(_instance.handle(), _surface.handle(),
        wk::GetRequiredDeviceExtensions(), &physical_device_features,
        &wk::DefaultPhysicalDeviceFeatureScorer
    );
    wk::PhysicalDeviceSurfaceSupport physical_device_support = wk::GetPhysicalDeviceSurfaceSupport(_physical_device.handle(), _surface.handle());
    wk::DeviceQueueFamilyIndices queue_family_indices = _physical_device.queue_family_indices();

    const float QUEUE_PRIORITY = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos = {
        wk::DeviceQueueCreateInfo{}
            .set_queue_family_index(queue_family_indices.graphics_family.value())
            .set_queue_count(1)
            .set_p_queue_priorities(&QUEUE_PRIORITY)
            .to_vk()
    };
    if (queue_family_indices.is_unique()) {
        queue_create_infos.push_back(
            wk::DeviceQueueCreateInfo{}
                .set_queue_family_index(queue_family_indices.present_family.value())
                .set_queue_count(1)
                .set_p_queue_priorities(&QUEUE_PRIORITY)
                .to_vk());
    }

    _device = wk::Device(_physical_device.handle(), queue_family_indices,
        wk::DeviceCreateInfo{}
            .set_p_enabled_features(&_physical_device.features())
            .set_enabled_extensions(_physical_device.extensions().size(),
                                    _physical_device.extensions().data())
            .set_queue_create_infos(queue_create_infos.size(), queue_create_infos.data())
            .to_vk());

    // ---------- Command pool ----------
    _command_pool = wk::CommandPool(_device.handle(),
        wk::CommandPoolCreateInfo{}
            .set_flags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                    VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)
            .set_queue_family_index(queue_family_indices.graphics_family.value())
            .to_vk()
    );

    // ---------- Allocator ----------
    VmaVulkanFunctions vulkan_functions{};
    _allocator = wk::Allocator(
        wk::AllocatorCreateInfo{}
            .set_vulkan_api_version(VK_API_VERSION_1_3)
            .set_instance(_instance.handle())
            .set_physical_device(_physical_device.handle())
            .set_device(_device.handle())
            .set_p_vulkan_functions(&vulkan_functions)
            .to_vk()
    );

    // ---------- Surface & Image formats ----------
    VkSurfaceFormatKHR surface_format = wk::ChooseSurfaceFormat(physical_device_support.formats);
    VkFormat image_format = surface_format.format;

    // ---------- Swapchain ----------
    std::vector<uint32_t> queue_family_indices_vec;
    VkSharingMode image_sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    if (queue_family_indices.is_unique()) {
        queue_family_indices_vec = queue_family_indices.to_vec(); // {graphics, present}
        image_sharing_mode = VK_SHARING_MODE_CONCURRENT;
    }

    _swapchain = wk::Swapchain(_device.handle(),
        wk::SwapchainCreateInfo{}
            .set_surface(_surface.handle())
            .set_present_mode(wk::ChooseSurfacePresentationMode(physical_device_support.present_modes))
            .set_min_image_count(std::clamp(
                physical_device_support.capabilities.minImageCount + 1,
                physical_device_support.capabilities.minImageCount,
                physical_device_support.capabilities.maxImageCount))
            .set_image_extent(wk::ChooseSurfaceExtent(_WIDTH, _HEIGHT, physical_device_support.capabilities))
            .set_image_format(surface_format.format)
            .set_image_color_space(surface_format.colorSpace)
            .set_image_array_layers(1)
            .set_image_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .set_image_sharing_mode(image_sharing_mode)
            .set_queue_family_indices(static_cast<uint32_t>(queue_family_indices_vec.size()),
                                      queue_family_indices_vec.data())
            .to_vk()
    );

    // ---------- Render pass ----------
    VkFormat depth_format = wk::ChooseDepthFormat(_physical_device.handle(), _DEPTH_FORMATS);

    VkAttachmentDescription attachments[2] = {
        wk::AttachmentDescription{}
            .set_flags(0)
            .set_format(image_format)
            .set_samples(VK_SAMPLE_COUNT_1_BIT)
            .set_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .set_store_op(VK_ATTACHMENT_STORE_OP_STORE)
            .set_stencil_load_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .set_stencil_store_op(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
            .set_final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
            .to_vk(),
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
            .to_vk()
    };
    
    VkAttachmentReference color_attachment_ref = wk::AttachmentReference{}
        .set_attachment(0)
        .set_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        .to_vk();

    VkAttachmentReference depth_attachment_ref = wk::AttachmentReference{}
        .set_attachment(1)
        .set_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        .to_vk();

    VkSubpassDescription subpass = wk::SubpassDescription{}
        .set_pipeline_bind_point(VK_PIPELINE_BIND_POINT_GRAPHICS)
        .set_color_attachments(1, &color_attachment_ref)
        .set_depth_stencil_attachment(&depth_attachment_ref)
        .to_vk();

    VkSubpassDependency dependency = wk::SubpassDependency{}
        .set_src_subpass(VK_SUBPASS_EXTERNAL)
        .set_dst_subpass(0)
        .set_src_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        .set_dst_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        .set_src_access_mask(0)
        .set_dst_access_mask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
        .to_vk();

    _render_pass = wk::RenderPass(_device.handle(),
        wk::RenderPassCreateInfo{}
            .set_attachments(2, attachments)
            .set_subpasses(1, &subpass)
            .set_dependencies(1, &dependency)
            .to_vk()
    );

    _depth_images.clear();
    _depth_image_views.clear();
    _framebuffers.clear();
    _depth_images.reserve(_swapchain.image_views().size());
    _depth_image_views.reserve(_swapchain.image_views().size());
    _framebuffers.reserve(_swapchain.image_views().size());

    for (size_t i = 0; i < _swapchain.image_views().size(); ++i) {
        // Depth image per framebuffer
        _depth_images.emplace_back(
            _allocator.handle(),
            wk::ImageCreateInfo{}
                .set_image_type(VK_IMAGE_TYPE_2D)
                .set_format(depth_format)
                .set_extent(wk::Extent(_swapchain.extent()).to_vk())
                .set_mip_levels(1)
                .set_array_layers(1)
                .set_samples(VK_SAMPLE_COUNT_1_BIT)
                .set_tiling(VK_IMAGE_TILING_OPTIMAL)
                .set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
                .to_vk(),
            wk::AllocationCreateInfo{}
                .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
                .to_vk()
        );

        // Depth view
        _depth_image_views.emplace_back(
            _device.handle(),
            wk::ImageViewCreateInfo{}
                .set_image(_depth_images.back().handle())
                .set_view_type(VK_IMAGE_VIEW_TYPE_2D)
                .set_format(depth_format)
                .set_components(wk::ComponentMapping::identity().to_vk())
                .set_subresource_range(
                    wk::ImageSubresourceRange{}
                        .set_aspect_mask(wk::GetAspectFlags(depth_format))
                        .set_base_mip_level(0)
                        .set_level_count(1)
                        .set_base_array_layer(0)
                        .set_layer_count(1)
                        .to_vk()
                )
                .to_vk()
        );

        // Framebuffer (color + depth)
        std::vector<VkImageView> attachments = {
            _swapchain.image_views()[i],
            _depth_image_views.back().handle()
        };

        _framebuffers.emplace_back(
            _device.handle(),
            wk::FramebufferCreateInfo{}
                .set_render_pass(_render_pass.handle())
                .set_attachments(static_cast<uint32_t>(attachments.size()), attachments.data())
                .set_extent(_swapchain.extent())
                .set_layers(1)
                .to_vk()
        );
    }

    // ---------- Graphics pipeline ----------
    VkDescriptorSetLayoutBinding bindings[] = {
        wk::DescriptorSetLayoutBinding{}
            .set_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(1)
            .set_stage_flags(VK_SHADER_STAGE_VERTEX_BIT)
            .to_vk()
    };
    
    wk::DescriptorSetLayout descriptor_set_layout(_device.handle(),
        wk::DescriptorSetLayoutCreateInfo{}
            .set_bindings(1, bindings)
            .to_vk()
    );
    
    VkDescriptorSetLayout layouts[] = { descriptor_set_layout.handle() };

    _pipeline_layout = wk::PipelineLayout(_device.handle(), 
        wk::PipelineLayoutCreateInfo{}
            .set_set_layouts(1, layouts)
            .set_push_constant_ranges(0, nullptr)
            .to_vk()
    );

    std::vector<uint8_t> vert_byte_code = wk::ReadSpirvShader("build/bin/example1_basic/Debug/shaders/triangle.vert.spv");
    std::vector<uint8_t> frag_byte_code = wk::ReadSpirvShader("build/bin/example1_basic/Debug/shaders/triangle.frag.spv");

    wk::Shader vert(_device.handle(),
        wk::ShaderCreateInfo{}
            .set_byte_code(vert_byte_code.size(), vert_byte_code.data())
            .to_vk()
    );
    wk::Shader frag(_device.handle(),
        wk::ShaderCreateInfo{}
            .set_byte_code(frag_byte_code.size(), frag_byte_code.data())
            .to_vk()
    );

    VkPipelineShaderStageCreateInfo shader_stages[2] = {
        wk::PipelineShaderStageCreateInfo{}
            .set_stage(VK_SHADER_STAGE_VERTEX_BIT)
            .set_module(vert.handle())
            .set_p_name("main")
            .to_vk(),
        wk::PipelineShaderStageCreateInfo{}
            .set_stage(VK_SHADER_STAGE_FRAGMENT_BIT)
            .set_module(frag.handle())
            .set_p_name("main")
            .to_vk()
    };
    
    VkViewport viewport = wk::Viewport{}
        .set_x(0.0f)
        .set_y(0.0f)
        .set_width(static_cast<float>(_swapchain.extent().width))
        .set_height(static_cast<float>(_swapchain.extent().height))
        .set_min_depth(0.0f)
        .set_max_depth(1.0f)
        .to_vk();

    VkRect2D scissor = wk::Rect2D{}
        .set_offset({ 0, 0 })
        .set_extent(_swapchain.extent())
        .to_vk();

    VkVertexInputBindingDescription vertex_binding = wk::VertexInputBindingDescription{}
        .set_binding(0)
        .set_stride(sizeof(Vertex))
        .set_input_rate(VK_VERTEX_INPUT_RATE_VERTEX)
        .to_vk();

    VkVertexInputAttributeDescription vertex_attributes[2] = {
        wk::VertexInputAttributeDescription{}
            .set_binding(0)
            .set_location(0)
            .set_format(VK_FORMAT_R32G32B32_SFLOAT)
            .set_offset(offsetof(Vertex, position))
            .to_vk(),
        wk::VertexInputAttributeDescription{}
            .set_binding(0)
            .set_location(1)
            .set_format(VK_FORMAT_R32G32B32_SFLOAT)
            .set_offset(offsetof(Vertex, color))
            .to_vk(),
    };
    
    VkPipelineVertexInputStateCreateInfo vertex_input_ci = wk::PipelineVertexInputStateCreateInfo{}
        .set_vertex_binding_descriptions(1, &vertex_binding)
        .set_vertex_attribute_descriptions(2, vertex_attributes)
        .to_vk();

    VkPipelineInputAssemblyStateCreateInfo input_assembly_ci = wk::PipelineInputAssemblyStateCreateInfo{}
        .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .set_primitive_restart_enable(VK_FALSE)
        .to_vk();
        
    VkPipelineViewportStateCreateInfo viewport_state_ci = wk::PipelineViewportStateCreateInfo{}
        .set_viewports(1, &viewport)
        .set_scissors(1, &scissor)
        .to_vk();

    VkPipelineRasterizationStateCreateInfo raster_ci = wk::PipelineRasterizationStateCreateInfo{}
        .set_polygon_mode(VK_POLYGON_MODE_FILL)
        .set_cull_mode(VK_CULL_MODE_BACK_BIT)
        .set_front_face(VK_FRONT_FACE_COUNTER_CLOCKWISE)
        .set_line_width(1.0f)
        .to_vk();

    VkPipelineMultisampleStateCreateInfo multisample_ci = wk::PipelineMultisampleStateCreateInfo{}
        .set_rasterization_samples(VK_SAMPLE_COUNT_1_BIT)
        .to_vk();
    
    VkPipelineDepthStencilStateCreateInfo depth_stencil_ci = wk::PipelineDepthStencilStateCreateInfo{}
        .set_depth_test_enable(VK_TRUE)
        .set_depth_write_enable(VK_TRUE)
        .set_depth_compare_op(VK_COMPARE_OP_LESS)
        .set_depth_bounds_test_enable(VK_FALSE)
        .set_stencil_test_enable(VK_FALSE)
        .to_vk();

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
        .to_vk();

    VkPipelineColorBlendStateCreateInfo color_blend_state_ci = wk::PipelineColorBlendStateCreateInfo{}
        .set_attachments(1, &color_blend_attachment)
        .to_vk();
    
    VkDynamicState dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic_ci = wk::PipelineDynamicStateCreateInfo{}
        .set_dynamic_states(2, dynamic_states)
        .to_vk();

    _pipeline = wk::Pipeline(_device.handle(), 
        wk::PipelineCreateInfo{}
            .set_stages(2, shader_stages)
            .set_p_vertex_input_state(&vertex_input_ci)
            .set_p_input_assembly_state(&input_assembly_ci)
            .set_p_viewport_state(&viewport_state_ci)
            .set_p_rasterization_state(&raster_ci)
            .set_p_multisample_state(&multisample_ci)
            .set_p_depth_stencil_state(&depth_stencil_ci)
            .set_p_color_blend_state(&color_blend_state_ci)
            .set_p_dynamic_state(&dynamic_ci)
            .set_layout(_pipeline_layout.handle())
            .set_render_pass(_render_pass.handle())
            .set_subpass(0)
            .to_vk()
    );

    // ---------- Geometry buffers ----------
    _vertex_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(_VERTICES.size() * sizeof(Vertex))
            .set_usage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    _index_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(_INDICES.size() * sizeof(uint16_t))
            .set_usage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    // ---------- Staging buffers ----------
    wk::Buffer vertex_staging_buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(_VERTICES.size() * sizeof(Vertex))
            .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
            .to_vk()
    );

    wk::Buffer index_staging_buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(_INDICES.size() * sizeof(uint16_t))
            .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
            .to_vk()
    );

    // ---------- Upload ----------
    {
        void* vertex_data;
        vmaMapMemory(_allocator.handle(), vertex_staging_buffer.allocation(), &vertex_data);
        memcpy(vertex_data, _VERTICES.data(), _VERTICES.size() * sizeof(Vertex));
        vmaUnmapMemory(_allocator.handle(), vertex_staging_buffer.allocation());

        void* index_data;
        vmaMapMemory(_allocator.handle(), index_staging_buffer.allocation(), &index_data);
        memcpy(index_data, _INDICES.data(), _INDICES.size() * sizeof(uint16_t));
        vmaUnmapMemory(_allocator.handle(), index_staging_buffer.allocation());

        // ---------- Record copy command ----------
        wk::CommandBuffer upload_command_buffer(_device.handle(),
            wk::CommandBufferAllocateInfo{}
                .set_command_pool(_command_pool.handle())
                .to_vk()
        );

        VkCommandBufferBeginInfo begin_info = wk::CommandBufferBeginInfo{}
            .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
            .to_vk();
        vkBeginCommandBuffer(upload_command_buffer.handle(), &begin_info);

        VkBufferCopy vertex_copy = wk::BufferCopy{}
            .set_size(_VERTICES.size() * sizeof(Vertex))
            .to_vk();
        vkCmdCopyBuffer(upload_command_buffer.handle(),
            vertex_staging_buffer.handle(), _vertex_buffer.handle(), 1, &vertex_copy);

        VkBufferCopy index_copy = wk::BufferCopy{}
            .set_size(_INDICES.size() * sizeof(uint16_t))
            .to_vk();
        vkCmdCopyBuffer(upload_command_buffer.handle(),
            index_staging_buffer.handle(), _index_buffer.handle(), 1, &index_copy);

        vkEndCommandBuffer(upload_command_buffer.handle());

        // ---------- Submit & Wait ----------
        VkSubmitInfo submit_info = wk::GraphicsQueueSubmitInfo{}
            .set_command_buffers(1, &upload_command_buffer.handle())
            .to_vk();

        vkQueueSubmit(_device.graphics_queue().handle(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(_device.graphics_queue().handle());
    }

    // ---------- Uniform buffers ----------
    _uniform_buffers.reserve(_MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; ++i) {
        _uniform_buffers.emplace_back(_allocator.handle(),
            wk::BufferCreateInfo{}
                .set_size(sizeof(UniformBufferObject))
                .set_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                .to_vk(),
            wk::AllocationCreateInfo{}
                .set_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
                .to_vk()
        );
    }

    // ---------- Descriptor pool ----------
    VkDescriptorPoolSize pool_sizes[] = {
        wk::DescriptorPoolSize{}
            .set_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT))
            .to_vk()
    };

    _descriptor_pool = wk::DescriptorPool(_device.handle(),
        wk::DescriptorPoolCreateInfo{}
            .set_max_sets(_MAX_FRAMES_IN_FLIGHT)
            .set_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .set_pool_sizes(1, pool_sizes)
            .to_vk()
    );

    // ---------- Descriptor sets ----------
    _descriptor_sets.clear();
    _descriptor_sets.reserve(_MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; ++i) {
        _descriptor_sets.emplace_back(_device.handle(),
            wk::DescriptorSetAllocateInfo{}
                .set_descriptor_pool(_descriptor_pool.handle())
                .set_set_layouts(1, layouts)
                .to_vk()
        );

        VkDescriptorBufferInfo db_info = wk::DescriptorBufferInfo{}
            .set_buffer(_uniform_buffers[i].handle())
            .set_offset(0)
            .set_range(sizeof(UniformBufferObject))
            .to_vk();

        VkWriteDescriptorSet write_descriptor_set = wk::WriteDescriptorSet{}
            .set_dst_set(_descriptor_sets[i].handle())
            .set_dst_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(1)
            .set_p_buffer_info(&db_info)
            .to_vk();

        vkUpdateDescriptorSets(_device.handle(), 1, &write_descriptor_set, 0, nullptr);
    }

    // ---------- Sync & Command buffers ----------
    _command_buffers.clear();
    _image_available_semaphores.clear();
    _render_finished_semaphores.clear();
    _frame_in_flight_fences.clear();
    _command_buffers.reserve(_MAX_FRAMES_IN_FLIGHT);
    _image_available_semaphores.reserve(_MAX_FRAMES_IN_FLIGHT);
    _render_finished_semaphores.reserve(_MAX_FRAMES_IN_FLIGHT);
    _frame_in_flight_fences.reserve(_MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; ++i) {
        _command_buffers.emplace_back(_device.handle(),
            wk::CommandBufferAllocateInfo{}
                .set_command_pool(_command_pool.handle())
                .to_vk());
        _image_available_semaphores.emplace_back(_device.handle(), 
            wk::SemaphoreCreateInfo{}
                .to_vk());
        _render_finished_semaphores.emplace_back(_device.handle(), 
            wk::SemaphoreCreateInfo{}
                .to_vk());
        _frame_in_flight_fences.emplace_back(_device.handle(),
            wk::FenceCreateInfo{}
                .set_flags(VK_FENCE_CREATE_SIGNALED_BIT)
                .to_vk());
    }
    return 0;
}

int App::_init_geometry() {
    return 0;
}

int App::_main_loop() {
    size_t current_frame_in_flight = 0;
    while (!glfwWindowShouldClose(_window)) {
        vkWaitForFences(_device.handle(), 1, &_frame_in_flight_fences[current_frame_in_flight].handle(), VK_TRUE, UINT64_MAX);
        glfwPollEvents();

        // Acquire next image
        VkResult result;
        uint32_t available_image_index = 0;
        result = vkAcquireNextImageKHR(_device.handle(), _swapchain.handle(), UINT32_MAX, 
            _image_available_semaphores[current_frame_in_flight].handle(), VK_NULL_HANDLE, &available_image_index
        );
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            _rebuild_swapchain();
            continue;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            std::cerr << "failed to acquire swapchain image" << std::endl;
            return 1;
        }

        vkResetFences(_device.handle(), 1, &_frame_in_flight_fences[current_frame_in_flight].handle());
        vkResetCommandBuffer(_command_buffers[current_frame_in_flight].handle(), 0);

        VkCommandBufferBeginInfo cb_begin_info = wk::CommandBufferBeginInfo{}.to_vk();
        result = vkBeginCommandBuffer(_command_buffers[current_frame_in_flight].handle(), &cb_begin_info);
        if (result != VK_SUCCESS) {
            std::cerr << "could not begin command buffer" << std::endl;
            return 1;
        }

        // Clear screen
        VkClearValue color_clear{};  color_clear.color        = {{0.0f, 0.0f, 0.0f, 1.0f}};
        VkClearValue depth_clear{};  depth_clear.depthStencil = {1.0f, 0};
        std::vector<VkClearValue> clear_values = { color_clear, depth_clear };

        VkRenderPassBeginInfo rp_begin_info = wk::RenderPassBeginInfo{}
            .set_render_pass(_render_pass.handle())
            .set_framebuffer(_framebuffers[available_image_index].handle())
            .set_render_area({ { 0, 0 }, _swapchain.extent() })
            .set_clear_values(static_cast<uint32_t>(clear_values.size()), clear_values.data())
            .to_vk();

        vkCmdBeginRenderPass(_command_buffers[current_frame_in_flight].handle(), 
            &rp_begin_info,
            VK_SUBPASS_CONTENTS_INLINE
        );

        vkCmdBindPipeline(_command_buffers[current_frame_in_flight].handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.handle());

        // Build viewport/scissor
        VkViewport viewport = wk::Viewport{}
            .set_x(0.0f).set_y(0.0f)
            .set_width(static_cast<float>(_swapchain.extent().width))
            .set_height(static_cast<float>(_swapchain.extent().height))
            .set_min_depth(0.0f).set_max_depth(1.0f)
            .to_vk();
        VkRect2D scissor = wk::Rect2D{}
            .set_offset({0,0})
            .set_extent(_swapchain.extent())
            .to_vk();
        vkCmdSetViewport(_command_buffers[current_frame_in_flight].handle(), 0, 1, &viewport);
        vkCmdSetScissor(_command_buffers[current_frame_in_flight].handle(), 0, 1, &scissor);

        // UBO update
        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        ubo.proj = glm::perspective(glm::radians(45.0f),
                                    _WIDTH / (float)_HEIGHT, 0.1f, 100.0f);
        ubo.proj[1][1] *= -1; // flip y for Vulkan

        void* data;
        vmaMapMemory(_allocator.handle(), _uniform_buffers[current_frame_in_flight].allocation(), &data);
        memcpy(data, &ubo, sizeof(ubo));
        vmaUnmapMemory(_allocator.handle(), _uniform_buffers[current_frame_in_flight].allocation());

        vkCmdBindDescriptorSets(
            _command_buffers[current_frame_in_flight].handle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
            _pipeline_layout.handle(),
            0, 1, &_descriptor_sets[current_frame_in_flight].handle(),
            0, nullptr
        );

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(_command_buffers[current_frame_in_flight].handle(), 0, 1, &_vertex_buffer.handle(), &offset);
        vkCmdBindIndexBuffer(_command_buffers[current_frame_in_flight].handle(), _index_buffer.handle(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(_command_buffers[current_frame_in_flight].handle(), static_cast<uint32_t>(_INDICES.size()), 1, 0, 0, 0);
        
        vkCmdEndRenderPass(_command_buffers[current_frame_in_flight].handle());
        result = vkEndCommandBuffer(_command_buffers[current_frame_in_flight].handle());
        if (result != VK_SUCCESS) {
            std::cerr << "failed to end command buffer" << std::endl;
            return 1;
        }

        std::vector<VkCommandBuffer> gq_command_buffers = { _command_buffers[current_frame_in_flight].handle() };
        std::vector<VkSemaphore> gq_wait_semaphores = { _image_available_semaphores[current_frame_in_flight].handle() };
        std::vector<VkPipelineStageFlags> gq_wait_stage_flags = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        std::vector<VkSemaphore> gq_signal_semaphores = { _render_finished_semaphores[current_frame_in_flight].handle() };
        VkSubmitInfo gq_submit_info = wk::GraphicsQueueSubmitInfo{}
            .set_command_buffers(static_cast<uint32_t>(gq_command_buffers.size()), gq_command_buffers.data())
            .set_wait_semaphores(static_cast<uint32_t>(gq_wait_semaphores.size()), gq_wait_semaphores.data())
            .set_wait_dst_stage_masks(static_cast<uint32_t>(gq_wait_stage_flags.size()), gq_wait_stage_flags.data())
            .set_signal_semaphores(static_cast<uint32_t>(gq_signal_semaphores.size()), gq_signal_semaphores.data())
            .to_vk();
        result = vkQueueSubmit(_device.graphics_queue().handle(), 1, &gq_submit_info, _frame_in_flight_fences[current_frame_in_flight].handle());
        if (result != VK_SUCCESS) {
            std::cerr << "failed to submit queue" << std::endl;
            return 1;
        }

        std::vector<VkSwapchainKHR> pq_swapchains = { _swapchain.handle() };
        std::vector<uint32_t> pq_image_indices = { available_image_index };
        VkPresentInfoKHR present_info = wk::PresentInfo{}
            .set_swapchains(static_cast<uint32_t>(pq_swapchains.size()), pq_swapchains.data())
            .set_image_indices(pq_image_indices.data())
            .set_wait_semaphores(static_cast<uint32_t>(gq_signal_semaphores.size()), gq_signal_semaphores.data())
            .to_vk();
        result = vkQueuePresentKHR(_device.present_queue().handle(), &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            _rebuild_swapchain();
            continue;
        } else if (result != VK_SUCCESS) {
            std::cerr << "failed to present" << std::endl;
            return 1;
        }
        
        current_frame_in_flight = (current_frame_in_flight + 1) % _MAX_FRAMES_IN_FLIGHT;
    }
    return 0;
}

void App::_rebuild_swapchain() {
    vkDeviceWaitIdle(_device.handle());
    
    wk::PhysicalDeviceSurfaceSupport physical_device_support = wk::GetPhysicalDeviceSurfaceSupport(_physical_device.handle(), _surface.handle());
    VkSurfaceFormatKHR surface_format = wk::ChooseSurfaceFormat(physical_device_support.formats);
    VkExtent2D surface_extent = wk::ChooseSurfaceExtent(_WIDTH, _HEIGHT, physical_device_support.capabilities);
    VkFormat depth_format = wk::ChooseDepthFormat(_physical_device.handle(), _DEPTH_FORMATS);

    wk::Swapchain new_swapchain = wk::Swapchain(_device.handle(),
        wk::SwapchainCreateInfo{}
            .set_surface(_surface.handle())
            .set_present_mode(wk::ChooseSurfacePresentationMode(physical_device_support.present_modes))
            .set_min_image_count(std::clamp(
                physical_device_support.capabilities.minImageCount + 1,
                physical_device_support.capabilities.minImageCount,
                physical_device_support.capabilities.maxImageCount))
            .set_image_extent(surface_extent)
            .set_image_format(surface_format.format)
            .set_image_color_space(surface_format.colorSpace)
            .set_image_array_layers(1)
            .set_image_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .set_image_sharing_mode(_swapchain.image_sharing_mode())
            .set_queue_family_indices(_swapchain.queue_family_indices().size(),
                                      _swapchain.queue_family_indices().data())
            .set_old_swapchain(_swapchain.handle())
            .to_vk()
    );
    _swapchain = std::move(new_swapchain);

    _depth_images.clear();
    _depth_image_views.clear();
    _framebuffers.clear();
    _depth_images.reserve(_swapchain.image_views().size());
    _depth_image_views.reserve(_swapchain.image_views().size());
    _framebuffers.reserve(_swapchain.image_views().size());

    for (size_t i = 0; i < _swapchain.image_views().size(); ++i) {
        // Depth image per framebuffer
        _depth_images.emplace_back(
            _allocator.handle(),
            wk::ImageCreateInfo{}
                .set_image_type(VK_IMAGE_TYPE_2D)
                .set_format(depth_format)
                .set_extent(wk::Extent(_swapchain.extent()).to_vk())
                .set_mip_levels(1)
                .set_array_layers(1)
                .set_samples(VK_SAMPLE_COUNT_1_BIT)
                .set_tiling(VK_IMAGE_TILING_OPTIMAL)
                .set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
                .to_vk(),
            wk::AllocationCreateInfo{}
                .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
                .to_vk()
        );

        // Depth view
        _depth_image_views.emplace_back(
            _device.handle(),
            wk::ImageViewCreateInfo{}
                .set_image(_depth_images.back().handle())
                .set_view_type(VK_IMAGE_VIEW_TYPE_2D)
                .set_format(depth_format)
                .set_components(wk::ComponentMapping::identity().to_vk())
                .set_subresource_range(
                    wk::ImageSubresourceRange{}
                        .set_aspect_mask(wk::GetAspectFlags(depth_format))
                        .set_base_mip_level(0)
                        .set_level_count(1)
                        .set_base_array_layer(0)
                        .set_layer_count(1)
                        .to_vk()
                )
                .to_vk()
        );

        // Framebuffer (color + depth)
        std::vector<VkImageView> attachments = {
            _swapchain.image_views()[i],
            _depth_image_views.back().handle()
        };

        _framebuffers.emplace_back(
            _device.handle(),
            wk::FramebufferCreateInfo{}
                .set_render_pass(_render_pass.handle())
                .set_attachments(static_cast<uint32_t>(attachments.size()), attachments.data())
                .set_extent(_swapchain.extent())
                .set_layers(1)
                .to_vk()
        );
    }
}

void App::_cleanup() {
    vkDeviceWaitIdle(_device.handle());
    glfwTerminate();
}
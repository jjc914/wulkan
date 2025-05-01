#define WLK_ENABLE_VALIDATION_LAYERS
#include <wk/wulkan.hpp>
#include <ext/glfw/glfw_internal.hpp>
#include <ext/glfw/surface.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>

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
    GLFWwindow* window = glfwCreateWindow(width, height, "vulkantest", nullptr, nullptr);

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
    wk::Instance instance(
        wk::InstanceCreateInfo{}
            .set_flags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR)
            .set_extensions(wk::ext::glfw::GetDefaultGlfwRequiredInstanceExtensions())
            .set_layers({ "VK_LAYER_KHRONOS_validation" })
            .set_application_info(
                wk::ApplicationInfo{}
                    .set_application_name("vulkantest")
                    .set_application_version(VK_MAKE_VERSION(1, 0, 0))
                    .set_engine_name("No Engine")
                    .set_engine_version(VK_MAKE_VERSION(1, 0, 0))
                    .set_api_version(VK_API_VERSION_1_3)
                    .to_vk_application_info()
            )
#ifdef WLK_ENABLE_VALIDATION_LAYERS
            .set_pnext(&debug_ci)
#endif
            .to_vk_instance_create_info()
    );

    wk::ext::glfw::Surface surface(instance.handle(), window);
    wk::PhysicalDevice physical_device(instance.handle(), surface.handle(), wk::GetDefaultRequiredDeviceExtensions());
    wk::Device device(physical_device.handle(), physical_device.queue_family_indices(), surface.handle(), 
        wk::DeviceCreateInfo{}
            .set_features(physical_device.features())
            .set_queue_family_indices(physical_device.queue_family_indices())
            .set_extensions(physical_device.extensions())
            .set_layers({})
            .to_vk_device_create_info()
    ); 

    const size_t max_frames_in_flight = 2;
    wk::Swapchain swapchain(device.handle(), 
        wk::SwapchainCreateInfo{}
            .set_surface(surface.handle())
            .set_physical_device(physical_device.handle())
            .set_width(900)
            .set_height(600)
            .set_queue_family_indices(physical_device.queue_family_indices())
            .to_vk_swapchain_create_info()
    );

    wk::RenderPass render_pass(device.handle(), 
        wk::RenderPassCreateInfo{}
            .set_image_format(swapchain.image_format())
            .set_depth_format(swapchain.depth_format())
            .set_samples(VK_SAMPLE_COUNT_1_BIT)
            .to_vk_render_pass_create_info()
    );

    wk::CommandPool command_pool(device.handle(),
        wk::CommandPoolCreateInfo{}
            .set_queue_family_indices(physical_device.queue_family_indices())
            .to_vk_command_pool_create_info()
    );

    std::vector<wk::Framebuffer> framebuffers{};
    framebuffers.reserve(swapchain.image_views().size());
    for (int i = 0; i < swapchain.image_views().size(); ++i) {
        framebuffers.emplace_back(device.handle(), 
            wk::FramebufferCreateInfo{}
                .set_image_view(swapchain.image_views()[i])
                .set_render_pass(render_pass.handle())
                .set_extent(swapchain.extent())
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
            wk::FenceCreateInfo{}.to_vk_fence_create_info()
        );
    }

    wk::Shader vert(device.handle(),
        wk::ShaderCreateInfo{}
            .set_byte_code(wk::ReadSpirvShader("build/bin/example1_basic/Debug/shaders/triangle.vert.spv"))
            .to_vk_shader_module_create_info()
    );
    wk::Shader frag(device.handle(),
        wk::ShaderCreateInfo{}
            .set_byte_code(wk::ReadSpirvShader("build/bin/example1_basic/Debug/shaders/triangle.frag.spv"))
            .to_vk_shader_module_create_info()
    );
    
    wk::DescriptorSetLayout descriptor_set_layout(device.handle(),
        wk::DescriptorSetLayoutCreateInfo{}
            .set_bindings({
                wk::DescriptorSetLayoutBinding{}
                    .set_binding(0)
                    .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                    .set_descriptor_count(1)
                    .set_stage_flags(VK_SHADER_STAGE_VERTEX_BIT)
                    .to_vk_descriptor_set_layout_binding()
            })
            .to_vk_descriptor_set_layout_create_info()
    );

    wk::DescriptorPool descriptor_pool(device.handle(),
        wk::DescriptorPoolCreateInfo{}
            .set_max_sets(1)
            .set_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .set_pool_sizes({
                wk::DescriptorPoolSize{}
                    .set_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                    .set_descriptor_count(1)
                    .to_vk_descriptor_pool_size()
            })
            .to_vk_descriptor_pool_create_info()
    );

    wk::DescriptorSet descriptor_set(device.handle(),
        wk::DescriptorSetAllocateInfo{}
            .set_descriptor_pool(descriptor_pool.handle())
            .set_layouts({ descriptor_set_layout.handle() })
            .to_vk_descriptor_set_allocate_info()
    );

    wk::PipelineLayout pipeline_layout(device.handle(), 
        wk::PipelineLayoutCreateInfo{}
            .set_set_layouts({ descriptor_set_layout.handle() })
            .set_push_constant_ranges({})
            .to_vk_pipeline_layout_create_info()
    );

    wk::Pipeline pipeline(device.handle(),
        wk::PipelineCreateInfo{}
            .set_render_pass(render_pass.handle())
            .set_layout(pipeline_layout.handle())
            .set_vert_shader(vert.handle())
            .set_frag_shader(frag.handle())
            .set_viewport(
                wk::ViewportInfo{}
                    .set_x(0.0f)
                    .set_y(0.0f)
                    .set_width((float)swapchain.extent().width)
                    .set_height((float)swapchain.extent().height)
                    .set_min_depth(0.0f)
                    .set_max_depth(1.0f)
                    .to_vk_viewport()
            )
            .set_scissor(
                wk::ScissorInfo{}
                    .set_offset({ 0, 0 })
                    .set_extent(swapchain.extent())
                    .to_vk_rect_2D()
            )
            .set_vertex_binding_descriptions({
                wk::VertexBindingDescription{}
                    .set_binding(0)
                    .set_stride(sizeof(Vertex))
                    .set_input_rate(VK_VERTEX_INPUT_RATE_VERTEX)
                    .to_vk_vertex_input_binding_description()
            })
            .set_vertex_attribute_descriptions({
                wk::VertexAttributeDescription{}
                    .set_binding(0).set_location(0)
                    .set_format(VK_FORMAT_R32G32B32_SFLOAT)
                    .set_offset(offsetof(Vertex, position))
                    .to_vk_vertex_input_attribute_description(),
                wk::VertexAttributeDescription{}
                    .set_binding(0).set_location(1)
                    .set_format(VK_FORMAT_R32G32B32_SFLOAT)
                    .set_offset(offsetof(Vertex, color))
                    .to_vk_vertex_input_attribute_description()
            })
            .to_vk_graphics_pipeline_create_info()
    );

    wk::Allocator allocator(
        wk::AllocatorCreateInfo{}
            .set_version(VK_API_VERSION_1_3)
            .set_instance(instance.handle())
            .set_physical_device(physical_device.handle())
            .set_device(device.handle())
            .to_vk_allocator_create_info()
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
    
    result = vkBeginCommandBuffer(upload_command_buffer.handle(), 
        &wk::CommandBufferBeginInfo{}
            .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
            .to_vk_command_buffer_begin_info()
    );
    if (result != VK_SUCCESS) {
        std::cerr << "could not begin command buffer" << std::endl;
        return -1;
    }
    
    vkCmdCopyBuffer(upload_command_buffer.handle(), vertex_staging_buffer.handle(), vertex_buffer.handle(), 1, 
        &wk::BufferCopy{}
            .set_size(vertices.size() * sizeof(Vertex))
            .to_vk_buffer_copy()
    );
    vkCmdCopyBuffer(upload_command_buffer.handle(), index_staging_buffer.handle(), index_buffer.handle(), 1, 
        &wk::BufferCopy{}
            .set_size(indices.size() * sizeof(uint16_t))
            .to_vk_buffer_copy()
    );

    result = vkEndCommandBuffer(upload_command_buffer.handle());
    if (result != VK_SUCCESS) {
        std::cerr << "failed to end command buffer" << std::endl;
        return -1;
    }

    vkQueueSubmit(device.graphics_queue(), 1, 
        &wk::GraphicsQueueSubmitInfo{}
            .set_command_buffers({ upload_command_buffer.handle() })
            .to_vk_submit_info(),
        VK_NULL_HANDLE
    );

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

    vkUpdateDescriptorSets(device.handle(), 1,
        &wk::WriteDescriptorSet{}
            .set_dst_set(descriptor_set.handle())
            .set_dst_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(1)
            .set_buffer_info(
                &wk::DescriptorBufferInfo{}
                    .set_buffer(uniform_buffer.handle())
                    .set_offset(0)
                    .set_range(sizeof(UniformBufferObject))
                    .to_vk_descriptor_buffer_info()
            )
            .to_vk_write_descriptor_set(),
        0, nullptr
    );

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
        
        if (is_swapchain_outdated) {
            width, height = 0;
            while (width == 0 || height == 0) {
                glfwGetFramebufferSize(window, &width, &height);
                glfwWaitEvents();
            }

            vkDeviceWaitIdle(device.handle());

            swapchain = wk::Swapchain(device.handle(),
                wk::SwapchainCreateInfo{}
                    .set_surface(surface.handle())
                    .set_physical_device(physical_device.handle())
                    .set_width(width)
                    .set_height(height)
                    .set_queue_family_indices(physical_device.queue_family_indices())
                    .set_old_swapchain(swapchain.handle())
                    .to_vk_swapchain_create_info()
            );
            
            framebuffers.clear();
            framebuffers.reserve(swapchain.image_views().size());
            for (const auto& image_view : swapchain.image_views()) {
                framebuffers.emplace_back(device.handle(),
                    wk::FramebufferCreateInfo{}
                        .set_image_view(image_view)
                        .set_render_pass(render_pass.handle())
                        .set_extent(swapchain.extent())
                        .to_vk_framebuffer_create_info()
                );
            }

            pipeline = wk::Pipeline(device.handle(),
            wk::PipelineCreateInfo{}
                .set_render_pass(render_pass.handle())
                .set_layout(pipeline_layout.handle())
                .set_vert_shader(vert.handle())
                .set_frag_shader(frag.handle())
                .set_viewport(
                    wk::ViewportInfo{}
                        .set_x(0.0f)
                        .set_y(0.0f)
                        .set_width((float)swapchain.extent().width)
                        .set_height((float)swapchain.extent().height)
                        .set_min_depth(0.0f)
                        .set_max_depth(1.0f)
                        .to_vk_viewport()
                )
                .set_scissor(
                    wk::ScissorInfo{}
                        .set_offset({ 0, 0 })
                        .set_extent(swapchain.extent())
                        .to_vk_rect_2D()
                )
                .set_vertex_binding_descriptions({
                    wk::VertexBindingDescription{}
                        .set_binding(0)
                        .set_stride(sizeof(Vertex))
                        .set_input_rate(VK_VERTEX_INPUT_RATE_VERTEX)
                        .to_vk_vertex_input_binding_description()
                })
                .set_vertex_attribute_descriptions({
                    wk::VertexAttributeDescription{}
                        .set_binding(0)
                        .set_location(0)
                        .set_format(VK_FORMAT_R32G32_SFLOAT)
                        .set_offset(offsetof(Vertex, position))
                        .to_vk_vertex_input_attribute_description(),
                    wk::VertexAttributeDescription{}
                        .set_binding(0)
                        .set_location(1)
                        .set_format(VK_FORMAT_R32G32B32_SFLOAT)
                        .set_offset(offsetof(Vertex, color))
                        .to_vk_vertex_input_attribute_description()
                })
                .to_vk_graphics_pipeline_create_info()
            );

            is_swapchain_outdated = false;
            continue;
        }

        vkResetFences(device.handle(), 1, &frame_in_flight_fences[current_frame_in_flight].handle());
        vkResetCommandBuffer(command_buffers[current_frame_in_flight].handle(), 0);

        result = vkBeginCommandBuffer(command_buffers[current_frame_in_flight].handle(), 
            &wk::CommandBufferBeginInfo{}.to_vk_command_buffer_begin_info()
        );
        if (result != VK_SUCCESS) {
            std::cerr << "could not begin command buffer" << std::endl;
            return -1;
        }
        
        vkCmdBeginRenderPass(command_buffers[current_frame_in_flight].handle(), 
            &wk::RenderPassBeginInfo{}
                .set_render_pass(render_pass.handle())
                .set_framebuffer(framebuffers[available_image_index].handle())
                .set_render_area({ { 0, 0 }, swapchain.extent() })
                .set_clear_values({ {{{ 0.0f, 0.0f, 0.0f, 1.0f }}} })
                .to_vk_render_pass_begin_info(),
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

        vkQueueSubmit(device.graphics_queue(), 1, 
            &wk::GraphicsQueueSubmitInfo{}
                .set_command_buffers({ command_buffers[current_frame_in_flight].handle() })
                .set_wait_semaphores({ { image_available_semaphores[current_frame_in_flight].handle(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } })
                .set_signal_semaphores({ { render_finished_semaphores[current_frame_in_flight].handle() } })
                .to_vk_submit_info(), 
            frame_in_flight_fences[current_frame_in_flight].handle()
        );

        result = vkQueuePresentKHR(device.present_queue(), 
            &wk::PresentInfo{}
                .set_swapchains({ swapchain.handle() })
                .set_image_indices({ available_image_index })
                .set_wait_semaphores({ render_finished_semaphores[current_frame_in_flight].handle() })
                .to_vk_present_info()
        );
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            is_swapchain_outdated = true;
            continue;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to present");
        }
        
        current_frame_in_flight = (current_frame_in_flight + 1) % max_frames_in_flight;
    }

    vkDeviceWaitIdle(device.handle());
    glfwTerminate();
}
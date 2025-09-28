#define WLK_ENABLE_VALIDATION_LAYERS
#include <wk/wulkan.hpp>
#include <wk/ext/rt/rt_internal.hpp>
#include <wk/ext/glfw/glfw_internal.hpp>
#include <wk/ext/glfw/surface.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <array>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
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

#pragma region glfw_init
    int width = 900;
    int height = 600;
    glfwSetErrorCallback(wk::ext::glfw::DefaultGlfwErrorCallback);
    if (!glfwInit()) {
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(width, height, "example1_basic", nullptr, nullptr);

    if (!glfwVulkanSupported()) {
        std::cerr << "GLFW: Vulkan Not Supported" << std::endl;
        return 1;
    }

#pragma region vulkan_init
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

    VkResult result;
    std::vector<const char*> instance_extensions = wk::ext::glfw::GetDefaultGlfwRequiredInstanceExtensions();
    std::vector<const char*> instance_layers;
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    VkApplicationInfo application_info = wk::ApplicationInfo{}
        .set_application_name("2_rt")
        .set_application_version(VK_MAKE_VERSION(1, 0, 0))
        .set_engine_name("No Engine")
        .set_engine_version(VK_MAKE_VERSION(1, 0, 0))
        .set_api_version(VK_API_VERSION_1_3)
        .to_vk();
    wk::Instance instance(
        wk::InstanceCreateInfo{}
            .set_flags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR)
            .set_extensions(instance_extensions.size(), instance_extensions.data())
            .set_layers(instance_layers.size(), instance_layers.data())
            .set_application_info(&application_info)
#ifdef WLK_ENABLE_VALIDATION_LAYERS
            .set_p_next(&debug_ci)
#endif
            .to_vk()
    );
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    wk::DebugMessenger debug_messenger(instance.handle(), debug_ci);
#endif

#pragma region device_init
    wk::ext::glfw::Surface surface(instance.handle(), window);

    VkPhysicalDeviceBufferDeviceAddressFeaturesEXT buffer_features = 
        wk::ext::rt::PhysicalDeviceBufferDeviceAddressFeatures{}
            .set_buffer_device_address(VK_TRUE)
            .set_buffer_device_address_capture_replay(VK_TRUE)
            .to_vk();
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rt_pipeline_features =
        wk::ext::rt::PhysicalDeviceRayTracingPipelineFeatures{}
            .set_ray_tracing_pipeline(VK_TRUE)
            .set_ray_tracing_pipeline_trace_rays_indirect(VK_TRUE)
            .set_ray_tracing_pipeline_shader_group_handle_capture_replay(VK_TRUE)
            .set_p_next(&buffer_features)
            .to_vk();
    VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features = 
        wk::ext::rt::PhysicalDeviceAccelerationStructureFeatures{}
            .set_acceleration_structure(VK_TRUE)
            .set_acceleration_structure_capture_replay(VK_TRUE)
            .set_acceleration_structure_indirect_build(VK_TRUE)
            .set_p_next(&rt_pipeline_features)
            .to_vk();
    VkPhysicalDeviceFeatures2 features2 = 
        wk::PhysicalDeviceFeatures2{}
            .set_p_next(&acceleration_structure_features)
            .to_vk();
    wk::PhysicalDevice physical_device(instance.handle(), surface.handle(), 
        wk::ext::rt::GetRTRequiredDeviceExtensions(), 
        &features2, 
        &wk::DefaultPhysicalDeviceFeatureScorer);
    
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
                .to_vk(),
            wk::DeviceQueueCreateInfo{}
                .set_queue_family_index(queue_family_indices.present_family.value())
                .set_queue_count(1)
                .set_p_queue_priorities(&queue_priorities)
                .to_vk()
        };
    } else {
        queue_create_infos = {
            wk::DeviceQueueCreateInfo{}
                .set_queue_family_index(queue_family_indices.graphics_family.value())
                .set_queue_count(1)
                .set_p_queue_priorities(&queue_priorities)
                .to_vk()
        };
    }

    wk::Device device(physical_device.handle(), queue_family_indices, surface.handle(), 
        wk::DeviceCreateInfo{}
            .set_p_next(&features2)
            .set_p_enabled_features(&physical_device.features())
            .set_enabled_extensions(physical_device.extensions().size(), physical_device.extensions().data())
            .set_queue_create_infos(queue_create_infos.size(), queue_create_infos.data())
            .to_vk()
    );

    VmaVulkanFunctions vulkan_functions{};
    wk::Allocator allocator(
        wk::AllocatorCreateInfo{}
            .set_vulkan_api_version(VK_API_VERSION_1_3)
            .set_instance(instance.handle())
            .set_physical_device(physical_device.handle())
            .set_device(device.handle())
            .set_p_vulkan_functions(&vulkan_functions)
            .to_vk()
    );

    VkSurfaceFormatKHR surface_format = wk::ChooseSurfaceFormat(physical_device_support.formats);

    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<uint32_t> queue_family_indices_vec = queue_family_indices.to_vec();
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
            .to_vk()
    );

    wk::CommandPool command_pool(device.handle(),
        wk::CommandPoolCreateInfo{}
            .set_queue_family_index(queue_family_indices.graphics_family.value())
            .to_vk()
    );

    wk::Buffer vertex_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(vertices.size() * sizeof(Vertex))
            .set_usage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    wk::Buffer index_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(indices.size() * sizeof(uint16_t))
            .set_usage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .to_vk()
    );

    wk::Buffer vertex_staging_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(vertices.size() * sizeof(Vertex))
            .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
            .to_vk()
    );

    wk::Buffer index_staging_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(indices.size() * sizeof(uint16_t))
            .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
            .to_vk()
    );
    
{
    void* dst = nullptr;
    vmaMapMemory(allocator.handle(), vertex_staging_buffer.allocation(), &dst);
    memcpy(dst, vertices.data(), sizeof(Vertex) * vertices.size());
    vmaUnmapMemory(allocator.handle(), vertex_staging_buffer.allocation());

    vmaMapMemory(allocator.handle(), index_staging_buffer.allocation(), &dst);
    memcpy(dst, indices.data(), sizeof(uint16_t) * indices.size());
    vmaUnmapMemory(allocator.handle(), index_staging_buffer.allocation());

    wk::CommandBuffer upload_command_buffer(device.handle(),
        wk::CommandBufferAllocateInfo{}
                .set_command_pool(command_pool.handle())
                .to_vk()
    );
    
    VkCommandBufferBeginInfo upload_begin_info = wk::CommandBufferBeginInfo{}
        .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
        .to_vk();
    result = vkBeginCommandBuffer(upload_command_buffer.handle(), &upload_begin_info);
    if (result != VK_SUCCESS) {
        std::cerr << "could not begin command buffer" << std::endl;
        return 1;
    }
    
    VkBufferCopy vertex_buffer_copy = wk::BufferCopy{}
        .set_size(vertices.size() * sizeof(Vertex))
        .to_vk();
    vkCmdCopyBuffer(upload_command_buffer.handle(), 
        vertex_staging_buffer.handle(), 
        vertex_buffer.handle(), 
        1, 
        &vertex_buffer_copy
    );
    VkBufferCopy index_buffer_copy = wk::BufferCopy{}
        .set_size(indices.size() * sizeof(uint16_t))
        .to_vk();
    vkCmdCopyBuffer(upload_command_buffer.handle(), 
        index_staging_buffer.handle(), 
        index_buffer.handle(), 
        1, 
        &index_buffer_copy
    );

    result = vkEndCommandBuffer(upload_command_buffer.handle());
    if (result != VK_SUCCESS) {
        std::cerr << "failed to end command buffer" << std::endl;
        return 1;
    }

    std::vector<VkCommandBuffer> upload_command_buffers = { upload_command_buffer.handle() };
    VkSubmitInfo gq_submit_info = wk::GraphicsQueueSubmitInfo{}
        .set_command_buffers(upload_command_buffers.size(), upload_command_buffers.data())
        .to_vk();
    result = vkQueueSubmit(device.graphics_queue(), 1, &gq_submit_info, VK_NULL_HANDLE);
    if (result != VK_SUCCESS) {
        std::cerr << "failed to submit queue" << std::endl;
        return 1;
    }
    vkDeviceWaitIdle(device.handle());
}

    VkBufferDeviceAddressInfo vertex_address_info = wk::BufferDeviceAddressInfo{}.set_buffer(vertex_buffer.handle()).to_vk();
    VkDeviceAddress vertex_address = vkGetBufferDeviceAddress(device.handle(), &vertex_address_info);

    VkBufferDeviceAddressInfo index_address_info = wk::BufferDeviceAddressInfo{}.set_buffer(index_buffer.handle()).to_vk();
    VkDeviceAddress index_address  = vkGetBufferDeviceAddress(device.handle(), &index_address_info);

    VkAccelerationStructureGeometryTrianglesDataKHR acceleration_structure_tri_data = wk::ext::rt::AccelerationStructureGeometryTrianglesData{}
        .set_vertex_format(VK_FORMAT_R32G32B32_SFLOAT)
        .set_vertex_data(wk::ext::rt::DeviceOrHostAddressConst{}.set_device_address(vertex_address).to_vk())
        .set_vertex_stride(sizeof(Vertex))
        .set_max_vertex(static_cast<uint32_t>(vertices.size()))
        .set_index_type(VK_INDEX_TYPE_UINT16)
        .set_index_data(wk::ext::rt::DeviceOrHostAddressConst{}.set_device_address(index_address).to_vk())
        .to_vk();

    VkAccelerationStructureGeometryKHR acceleration_structure_geometry = wk::ext::rt::AccelerationStructureGeometry{}
        .set_triangles(&acceleration_structure_tri_data)
        .set_flags(VK_GEOMETRY_OPAQUE_BIT_KHR)
        .to_vk();

    VkAccelerationStructureBuildGeometryInfoKHR blas_build_info = wk::ext::rt::AccelerationStructureBuildGeometryInfo{}
        .set_type(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR)
        .set_flags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR)
        .set_geometries(1, &acceleration_structure_geometry)
        .to_vk();

    uint32_t primitive_count = static_cast<uint32_t>(indices.size() / 3);
    VkAccelerationStructureBuildSizesInfoKHR size_info = wk::ext::rt::AccelerationStructureBuildSizesInfo{}.to_vk();
    vkGetAccelerationStructureBuildSizesKHR(
        device.handle(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &blas_build_info,
        &primitive_count,
        &size_info
    );

    wk::Buffer acceleration_structure_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(size_info.accelerationStructureSize)
            .set_usage(
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
                VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
            )
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    VkDeviceSize scratch_size = wk::AlignUp<VkDeviceSize>(size_info.buildScratchSize,
            wk::ext::rt::GetPhysicalDeviceAccelerationStructureProperties(physical_device.handle()).minAccelerationStructureScratchOffsetAlignment);
    wk::Buffer scratch_buffer(allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(scratch_size)
            .set_usage(
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
            )
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    wk::ext::rt::AccelerationStructure blas(device.handle(), 
        wk::ext::rt::AccelerationStructureCreateInfo{}
            .set_buffer(acceleration_structure_buffer.handle())
            .set_size(size_info.accelerationStructureSize)
            .set_type(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR)
            .to_vk());

    blas_build_info.dstAccelerationStructure = blas.handle();
    blas_build_info.scratchData = wk::ext::rt::DeviceOrHostAddress{}
        .set_device_address(
            vkGetBufferDeviceAddress(
                device.handle(),
                &wk::BufferDeviceAddressInfo{}.set_buffer(scratch_buffer.handle()).to_vk()
            ))
        .to_vk();

    VkAccelerationStructureBuildRangeInfoKHR blas_range = wk::ext::rt::AccelerationStructureBuildRangeInfo{}
        .set_primitive_count(primitive_count)
        .to_vk();

{
    wk::CommandBuffer cmd(device.handle(),
        wk::CommandBufferAllocateInfo{}
            .set_command_pool(command_pool.handle()).to_vk());

    vkBeginCommandBuffer(cmd.handle(),
        &wk::CommandBufferBeginInfo{}
            .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT).to_vk());

    const VkAccelerationStructureBuildRangeInfoKHR* blas_ranges[] = { &blas_range };
    vkCmdBuildAccelerationStructuresKHR(cmd.handle(), 1,
        &blas_build_info, blas_ranges);

    vkEndCommandBuffer(cmd.handle());

    VkSubmitInfo submit = wk::GraphicsQueueSubmitInfo{}
        .set_command_buffers(1, &cmd.handle()).to_vk();
    vkQueueSubmit(device.graphics_queue(), 1, &submit, VK_NULL_HANDLE);
    vkDeviceWaitIdle(device.handle());
}

    VkDeviceAddress blas_device_address = vkGetAccelerationStructureDeviceAddressKHR(device.handle(),
            &wk::ext::rt::AccelerationStructureDeviceAddressInfo{}
                .set_acceleration_structure(blas.handle())
                .to_vk()
        );

    VkAccelerationStructureInstanceKHR acceleration_structure_instance = wk::ext::rt::AccelerationStructureInstance{}
        .set_transform(wk::ext::rt::TransformMatrix{}.to_vk())
        .set_instance_custom_index(0)
        .set_mask(0xFF)
        .set_sbt_record_offset(0)
        .set_flags(VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR)
        .set_acceleration_structure_device_address(blas_device_address)
        .to_vk();

    VkDeviceSize instance_bytes = sizeof(acceleration_structure_instance);
    std::vector<VkAccelerationStructureInstanceKHR> instances{ acceleration_structure_instance };

    wk::Buffer instance_buffer(
        allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(instance_bytes)
            .set_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk());
            
    wk::Buffer instance_staging(
    allocator.handle(),
    wk::BufferCreateInfo{}
        .set_size(instance_bytes)
        .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        .to_vk(),
    wk::AllocationCreateInfo{}
        .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
        .to_vk());

{
    void* dst = nullptr;
    vmaMapMemory(allocator.handle(), instance_staging.allocation(), &dst);
    memcpy(dst, instances.data(), instance_bytes);
    vmaUnmapMemory(allocator.handle(), instance_staging.allocation());

    wk::CommandBuffer cmd(device.handle(),
        wk::CommandBufferAllocateInfo{}
            .set_command_pool(command_pool.handle()).to_vk());

    vkBeginCommandBuffer(cmd.handle(),
        &wk::CommandBufferBeginInfo{}
            .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT).to_vk());
            

    vkCmdCopyBuffer(cmd.handle(), instance_staging.handle(), instance_buffer.handle(),
                    1, &wk::BufferCopy{}.set_size(instance_bytes).to_vk());

    vkEndCommandBuffer(cmd.handle());

    VkSubmitInfo submit = wk::GraphicsQueueSubmitInfo{}
        .set_command_buffers(1, &cmd.handle()).to_vk();
    vkQueueSubmit(device.graphics_queue(), 1, &submit, VK_NULL_HANDLE);
    vkDeviceWaitIdle(device.handle());
}

    VkDeviceAddress instance_address = vkGetBufferDeviceAddress(device.handle(),
        &wk::BufferDeviceAddressInfo{}.set_buffer(instance_buffer.handle()).to_vk());
    
    VkAccelerationStructureGeometryInstancesDataKHR instance_data = wk::ext::rt::AccelerationStructureGeometryInstancesData{}
        .set_array_of_pointers(VK_FALSE)
        .set_data_device_address(instance_address)
        .to_vk();
    
    VkAccelerationStructureGeometryKHR tlas_geometry = wk::ext::rt::AccelerationStructureGeometry{}
        .set_instances(&instance_data)
        .set_flags(VK_GEOMETRY_OPAQUE_BIT_KHR)
        .to_vk();
    
    VkAccelerationStructureBuildGeometryInfoKHR tlas_build_info = wk::ext::rt::AccelerationStructureBuildGeometryInfo{}
        .set_type(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR)
        .set_flags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR)
        .set_geometries(1, &tlas_geometry)
        .to_vk();
    
    uint32_t instance_count = instances.size();
    VkAccelerationStructureBuildSizesInfoKHR tlas_sizes = wk::ext::rt::AccelerationStructureBuildSizesInfo{}.to_vk();
    vkGetAccelerationStructureBuildSizesKHR(
        device.handle(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &tlas_build_info,
        &instance_count,
        &tlas_sizes);
    
    wk::Buffer tlas_buffer(
        allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(tlas_sizes.accelerationStructureSize)
            .set_usage(VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
                    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}.set_usage(VMA_MEMORY_USAGE_GPU_ONLY).to_vk());

    wk::Buffer tlas_scratch(
        allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(tlas_sizes.buildScratchSize)
            .set_usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}.set_usage(VMA_MEMORY_USAGE_GPU_ONLY).to_vk());
    
    VkAccelerationStructureCreateInfoKHR tlas_build_info_info = wk::ext::rt::AccelerationStructureCreateInfo{}
        .set_buffer(tlas_buffer.handle())
        .set_size(tlas_sizes.accelerationStructureSize)
        .set_type(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR)
        .to_vk();
    wk::ext::rt::AccelerationStructure tlas = wk::ext::rt::AccelerationStructure(
        device.handle(),
        tlas_build_info_info
    );
    
    tlas_build_info.dstAccelerationStructure = tlas.handle();
    tlas_build_info.scratchData = wk::ext::rt::DeviceOrHostAddress{}
        .set_device_address(vkGetBufferDeviceAddress(
            device.handle(),
                &wk::BufferDeviceAddressInfo{}.set_buffer(tlas_scratch.handle()).to_vk()))
        .to_vk();
    
    VkAccelerationStructureBuildRangeInfoKHR tlas_range = wk::ext::rt::AccelerationStructureBuildRangeInfo{}
        .set_primitive_count(instance_count)
        .to_vk();

{
    wk::CommandBuffer cmd(device.handle(),
        wk::CommandBufferAllocateInfo{}
            .set_command_pool(command_pool.handle()).to_vk());

    vkBeginCommandBuffer(cmd.handle(),
        &wk::CommandBufferBeginInfo{}
            .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT).to_vk());

    const VkAccelerationStructureBuildRangeInfoKHR* tlas_ranges[] = { &tlas_range };
    vkCmdBuildAccelerationStructuresKHR(cmd.handle(), 1,
        &blas_build_info, tlas_ranges);

    vkEndCommandBuffer(cmd.handle());

    VkSubmitInfo submit = wk::GraphicsQueueSubmitInfo{}
        .set_command_buffers(1, &cmd.handle()).to_vk();
    vkQueueSubmit(device.graphics_queue(), 1, &submit, VK_NULL_HANDLE);
    vkDeviceWaitIdle(device.handle());
}

    wk::Image storage_img(
    allocator.handle(),
    wk::ImageCreateInfo{}
        .set_image_type(VK_IMAGE_TYPE_2D)
        .set_format(VK_FORMAT_R8G8B8A8_UNORM)
        .set_extent({uint32_t(width), uint32_t(height), 1})
        .set_usage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
        .to_vk(),
    wk::AllocationCreateInfo{}.to_vk());

    wk::ImageView storage_view(
        device.handle(),
        wk::ImageViewCreateInfo{}
            .set_image(storage_img.handle())
            .set_view_type(VK_IMAGE_VIEW_TYPE_2D)
            .set_format(VK_FORMAT_R8G8B8A8_UNORM)
            .set_subresource_range({VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1})
            .to_vk());
    
    VkDescriptorSetLayoutBinding bindings[] = {
        wk::DescriptorSetLayoutBinding{}
            .set_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
            .set_descriptor_count(1)
            .set_stage_flags(VK_SHADER_STAGE_RAYGEN_BIT_KHR)
            .to_vk(),
        wk::DescriptorSetLayoutBinding{}
            .set_binding(1)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
            .set_descriptor_count(1)
            .set_stage_flags(VK_SHADER_STAGE_RAYGEN_BIT_KHR)
            .to_vk()
    };

    wk::DescriptorSetLayout rt_ds_layout = wk::DescriptorSetLayout(
        device.handle(),
        wk::DescriptorSetLayoutCreateInfo{}
            .set_bindings(2, bindings)
            .to_vk());

    std::vector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1 });
    pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 });
    wk::DescriptorPool descriptor_pool = wk::DescriptorPool(
        device.handle(),
        wk::DescriptorPoolCreateInfo{}
            .set_max_sets(1)
            .set_pool_sizes(pool_sizes.size(), pool_sizes.data())
            .to_vk());

    wk::DescriptorSet descriptor_set(device.handle(), 
        wk::DescriptorSetAllocateInfo{}
            .set_descriptor_pool(descriptor_pool.handle())
            .set_set_layouts(1, &rt_ds_layout.handle())
            .to_vk()
    );

    // update
    VkWriteDescriptorSetAccelerationStructureKHR as_write = wk::ext::rt::WriteDescriptorSetAccelerationStructure{}
        .set_acceleration_structures(1, &tlas.handle())
        .to_vk();

    VkDescriptorImageInfo img_info{ VK_NULL_HANDLE, storage_view.handle(), VK_IMAGE_LAYOUT_GENERAL };

    VkWriteDescriptorSet writes[] = {
        wk::WriteDescriptorSet{}
            .set_dst_set(descriptor_set.handle())
            .set_dst_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
            .set_p_next(&as_write)
            .to_vk(),
        wk::WriteDescriptorSet{}
            .set_dst_set(descriptor_set.handle())
            .set_dst_binding(1)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
            .set_p_image_info(&img_info)
            .to_vk()
    };
    vkUpdateDescriptorSets(device.handle(), 2, writes, 0, nullptr);

    bool is_swapchain_outdated = false;
    size_t current_frame_in_flight = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    vkDeviceWaitIdle(device.handle());
    glfwTerminate();
}
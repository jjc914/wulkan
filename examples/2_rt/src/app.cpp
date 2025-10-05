#include "app.hpp"

#include <wk/wulkan_internal.hpp>
#include <wk/ext/rt/rt_internal.hpp>

#include <iostream>
#include <cstring>
#include <algorithm>

App::App() {

}

App::~App() {

}

// ------------------------- public -------------------------

int App::run() {
    if (_init_window()) {
        return 1;
    }
    if (_init_vulkan()) {
        return 1;
    }
    if (_main_loop()) {
        return 1;
    }
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

    VkValidationFeatureEnableEXT enables[] = {
        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
    };
    VkValidationFeaturesEXT features = wk::ValidationFeatures{}
        .set_enabled_features(1, enables)
        .to_vk();

    VkApplicationInfo application_info = wk::ApplicationInfo{}
        .set_application_name("2_rt")
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
            .set_p_next(&features)
#endif
            .to_vk()
    );
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    _debug_messenger = wk::DebugMessenger(_instance.handle(), debug_ci);
#endif
    _surface = wk::ext::glfw::Surface(_instance.handle(), _window);

    // ---------- device ----------
    VkPhysicalDeviceFeatures2 physical_device_features = wk::PhysicalDeviceFeatures2{}.to_vk();

    _physical_device = wk::PhysicalDevice(_instance.handle(), _surface.handle(),
        wk::ext::rt::GetRequiredDeviceExtensions(), &physical_device_features,
        &wk::DefaultPhysicalDeviceFeatureScorer
    );
    wk::PhysicalDeviceSurfaceSupport physical_device_support = wk::GetPhysicalDeviceSurfaceSupport(_physical_device.handle(), _surface.handle());
    wk::DeviceQueueFamilyIndices queue_family_indices = _physical_device.queue_family_indices();

    wk::ext::rt::FeatureChain rt_feature_chain = wk::ext::rt::MakeFeatureChain();

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
            .set_enabled_extensions(_physical_device.extensions().size(),
                                    _physical_device.extensions().data())
            .set_queue_create_infos(queue_create_infos.size(), queue_create_infos.data())
            .set_p_next(&rt_feature_chain.features2)
            .to_vk());
    _device_functions = wk::ext::rt::LoadFunctions(_device.handle());

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
            .set_flags(VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT)
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
            .set_image_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .set_image_sharing_mode(image_sharing_mode)
            .set_queue_family_indices(static_cast<uint32_t>(queue_family_indices_vec.size()),
                                      queue_family_indices_vec.data())
            .to_vk()
    );
    _is_swapchain_image_initialized.assign(_swapchain.image_count(), false);

    if (_build_storage_img()) return 1;
    
    if (_build_blas(queue_family_indices.graphics_family.value())) return 1;
    if (_build_tlas(queue_family_indices.graphics_family.value())) return 1;

    if (_build_pipeline()) return 1;

    // if (_build_descriptors()) return 1;
    if (_build_shader_binding_table()) return 1;

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

int App::_build_storage_img() {
    _rt_image = wk::Image(_allocator.handle(),
        wk::ImageCreateInfo{}
            .set_image_type(VK_IMAGE_TYPE_2D)
            .set_format(_rt_format)
            .set_extent(wk::Extent(_swapchain.extent()).to_vk())
            .set_mip_levels(1)
            .set_array_layers(1)
            .set_samples(VK_SAMPLE_COUNT_1_BIT)
            .set_tiling(VK_IMAGE_TILING_OPTIMAL)
            .set_usage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
            .to_vk(),
        wk::AllocationCreateInfo{}.set_usage(VMA_MEMORY_USAGE_GPU_ONLY).to_vk()
    );

    _rt_image_view = wk::ImageView(_device.handle(),
        wk::ImageViewCreateInfo{}
            .set_image(_rt_image.handle())
            .set_view_type(VK_IMAGE_VIEW_TYPE_2D)
            .set_format(_rt_format)
            .set_components(wk::ComponentMapping::identity().to_vk())
            .set_subresource_range(
                wk::ImageSubresourceRange{}
                    .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT)
                    .set_base_mip_level(0).set_level_count(1)
                    .set_base_array_layer(0).set_layer_count(1).to_vk())
            .to_vk()
    );

    _is_rt_image_initialized = false;

    return 0;
}

int App::_build_blas(uint32_t graphics_family_index) {
    // ---------- Geometry buffers ----------
    _vertex_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(_VERTICES.size() * sizeof(Vertex))
            .set_usage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR)
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    _index_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(_INDICES.size() * sizeof(uint16_t))
            .set_usage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR)
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

    VkBufferDeviceAddressInfoKHR vertex_buffer_address_info = wk::BufferDeviceAddressInfo{}
        .set_buffer(_vertex_buffer.handle())
        .to_vk();
    VkDeviceAddress vertex_buffer_address = vkGetBufferDeviceAddress(_device.handle(), 
        &vertex_buffer_address_info
    );

    VkBufferDeviceAddressInfoKHR index_buffer_address_info = wk::BufferDeviceAddressInfo{}
        .set_buffer(_index_buffer.handle())
        .to_vk();
    VkDeviceAddress index_buffer_address = vkGetBufferDeviceAddress(_device.handle(), 
        &index_buffer_address_info
    );

    // ---------- Triangles data ----------
    VkAccelerationStructureGeometryTrianglesDataKHR accel_triangles_data = wk::ext::rt::AccelerationStructureGeometryTrianglesData{}
        .set_vertex_format(VK_FORMAT_R32G32B32_SFLOAT)
        .set_vertex_data(
            wk::DeviceOrHostAddressConst{}
                .set_device_address(vertex_buffer_address)
                .to_vk()
        )
        .set_vertex_stride(sizeof(Vertex))
        .set_max_vertex(_VERTICES.size() - 1)
        .set_index_type(VK_INDEX_TYPE_UINT16)
        .set_index_data(
            wk::DeviceOrHostAddressConst{}
                .set_device_address(index_buffer_address)
                .to_vk()
        )
        .to_vk();

    // ---------- Acceleration structures ----------
    VkAccelerationStructureGeometryKHR accel_geometry = wk::ext::rt::AccelerationStructureGeometry{}
        .set_geometry_type(VK_GEOMETRY_TYPE_TRIANGLES_KHR)
        .set_geometry(
            wk::ext::rt::AccelerationStructureGeometryData{}
                .set_triangles(accel_triangles_data)
                .to_vk()
        )
        .to_vk();

    VkAccelerationStructureBuildGeometryInfoKHR accel_build_info = wk::ext::rt::AccelerationStructureBuildGeometryInfo{}
        .set_type(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR)
        .set_mode(VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR)
        .set_geometries(1, &accel_geometry)
        .to_vk();

    uint32_t primitive_count = static_cast<uint32_t>(_INDICES.size() / 3);
    VkAccelerationStructureBuildSizesInfoKHR sizes = wk::ext::rt::AccelerationStructureBuildSizesInfo{}.to_vk();
    _device_functions.vkGetAccelerationStructureBuildSizesKHR(_device.handle(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &accel_build_info,
        &primitive_count,
        &sizes
    );

    _blas_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(sizes.accelerationStructureSize)
            .set_usage(VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    _blas = wk::ext::rt::AccelerationStructure(_device.handle(), _device_functions,
        wk::ext::rt::AccelerationStructureCreateInfo{}
            .set_buffer(_blas_buffer.handle())
            .set_offset(0)
            .set_size(sizes.accelerationStructureSize)
            .set_type(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR)
            .to_vk()
    );

    // ---------- Scratch buffer ----------
    wk::Buffer scratch_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(std::max(sizes.buildScratchSize,
                               sizes.updateScratchSize))
            .set_usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    VkBufferDeviceAddressInfo scratch_address_info = wk::BufferDeviceAddressInfo{}
        .set_buffer(scratch_buffer.handle())
        .to_vk();
    VkDeviceAddress scratch_address = vkGetBufferDeviceAddress(_device.handle(),
        &scratch_address_info
    );

    // ---------- Build command ----------
    VkAccelerationStructureBuildRangeInfoKHR range = wk::ext::rt::AccelerationStructureBuildRangeInfo{}
        .set_primitive_count(primitive_count)
        .to_vk();
    const VkAccelerationStructureBuildRangeInfoKHR* ranges[] = { &range };

    wk::CommandBuffer command_buffer(
        _device.handle(),
        wk::CommandBufferAllocateInfo{}
            .set_command_pool(_command_pool.handle())
            .to_vk()
    );

    VkCommandBufferBeginInfo begin_info = wk::CommandBufferBeginInfo{}
        .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
        .to_vk();
    vkBeginCommandBuffer(command_buffer.handle(), &begin_info);

    VkAccelerationStructureBuildGeometryInfoKHR build_info = wk::ext::rt::AccelerationStructureBuildGeometryInfo{}
        .set_type(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR)
        .set_mode(VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR)
        .set_geometries(1, &accel_geometry)
        .set_dst_acceleration_structure(_blas.handle())
        .set_scratch_data(
            wk::DeviceOrHostAddress{}
                .set_device_address(scratch_address)
                .to_vk()
        )
        .to_vk();
    _device_functions.vkCmdBuildAccelerationStructuresKHR(command_buffer.handle(), 1, &build_info, ranges);

    vkEndCommandBuffer(command_buffer.handle());

    VkSubmitInfo gq_submit_info = wk::GraphicsQueueSubmitInfo{}
        .set_command_buffers(1, &command_buffer.handle())
        .to_vk();
    vkQueueSubmit(_device.graphics_queue().handle(),
        1,
        &gq_submit_info,
        VK_NULL_HANDLE
    );

    vkQueueWaitIdle(_device.graphics_queue().handle());

    return 0;
}

// ------------------------- TLAS (single instance) -------------------------

int App::_build_tlas(uint32_t graphics_family_index) {
    // BLAS device address
    VkAccelerationStructureDeviceAddressInfoKHR accel_device_address_info = wk::ext::rt::AccelerationStructureDeviceAddressInfo{}
        .set_acceleration_structure(_blas.handle())
        .to_vk();
    VkDeviceAddress blas_address = _device_functions.vkGetAccelerationStructureDeviceAddressKHR(_device.handle(), &accel_device_address_info);

    // Instance record
    VkAccelerationStructureInstanceKHR accel_instance = wk::ext::rt::AccelerationStructureInstance{}
        .set_transform(wk::ext::rt::TransformMatrix{}.set_identity().to_vk())
        .set_instance_custom_index(0)
        .set_mask(0xFF)
        .set_instance_shader_binding_table_record_offset(0)
        .set_flags(VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR)
        .set_acceleration_structure_reference(blas_address)
        .to_vk();

    // ---------- Instance buffer ----------
    _instance_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(sizeof(accel_instance))
            .set_usage(VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    // ---------- Staging buffer ----------
    wk::Buffer instance_staging_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(sizeof(accel_instance))
            .set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_CPU_ONLY)
            .to_vk()
    );

    // ---------- Upload instances ----------
    {
        void* instance_data;
        vmaMapMemory(_allocator.handle(), instance_staging_buffer.allocation(), &instance_data);
        memcpy(instance_data, &accel_instance, sizeof(accel_instance));
        vmaUnmapMemory(_allocator.handle(), instance_staging_buffer.allocation());

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

        VkBufferCopy instance_copy = wk::BufferCopy{}
            .set_size(sizeof(accel_instance))
            .to_vk();

        vkCmdCopyBuffer(upload_command_buffer.handle(),
            instance_staging_buffer.handle(), _instance_buffer.handle(), 1, &instance_copy);

        vkEndCommandBuffer(upload_command_buffer.handle());

        // ---------- Submit & Wait ----------
        VkSubmitInfo submit_info = wk::GraphicsQueueSubmitInfo{}
            .set_command_buffers(1, &upload_command_buffer.handle())
            .to_vk();

        vkQueueSubmit(_device.graphics_queue().handle(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(_device.graphics_queue().handle());
    }

    VkBufferDeviceAddressInfo instance_buffer_address_info = wk::BufferDeviceAddressInfo{}
        .set_buffer(_instance_buffer.handle())
        .to_vk();
    VkDeviceAddress instance_buffer_address = vkGetBufferDeviceAddress(_device.handle(), &instance_buffer_address_info);

    // ---------- Build TLAS ----------
    VkAccelerationStructureGeometryKHR accel_geometry = wk::ext::rt::AccelerationStructureGeometry{}
        .set_geometry_type(VK_GEOMETRY_TYPE_INSTANCES_KHR)
        .set_geometry(
            wk::ext::rt::AccelerationStructureGeometryData{}
                .set_instances(
                    wk::ext::rt::AccelerationStructureGeometryInstancesData{}
                        .set_array_of_pointers(VK_FALSE)
                        .set_data(
                            wk::DeviceOrHostAddressConst{}
                                .set_device_address(instance_buffer_address)
                                .to_vk()
                        )
                        .to_vk()
                )
                .to_vk()
        )
        .to_vk();

    VkAccelerationStructureBuildGeometryInfoKHR accel_build_info = wk::ext::rt::AccelerationStructureBuildGeometryInfo{}
        .set_type(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR)
        .set_mode(VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR)
        .set_geometries(1, &accel_geometry)
        .to_vk();

    uint32_t primitive_count = 1;
    VkAccelerationStructureBuildSizesInfoKHR sizes = wk::ext::rt::AccelerationStructureBuildSizesInfo{}.to_vk();
    _device_functions.vkGetAccelerationStructureBuildSizesKHR(_device.handle(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &accel_build_info,
        &primitive_count,
        &sizes
    );

    _tlas_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(sizes.accelerationStructureSize)
            .set_usage(VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT).to_vk(),
        wk::AllocationCreateInfo{}.set_usage(VMA_MEMORY_USAGE_GPU_ONLY).to_vk()
    );
    
    _tlas = wk::ext::rt::AccelerationStructure(_device.handle(), _device_functions,
        wk::ext::rt::AccelerationStructureCreateInfo{}
            .set_buffer(_tlas_buffer.handle())
            .set_offset(0)
            .set_size(sizes.accelerationStructureSize)
            .set_type(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR)
            .to_vk());

    wk::Buffer scratch_buffer = wk::Buffer(
        _allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(std::max(sizes.buildScratchSize,
                               sizes.updateScratchSize))
            .set_usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
            .to_vk(),
        wk::AllocationCreateInfo{}
            .set_usage(VMA_MEMORY_USAGE_GPU_ONLY)
            .to_vk()
    );

    VkBufferDeviceAddressInfo scratch_address_info = wk::BufferDeviceAddressInfo{}
        .set_buffer(scratch_buffer.handle())
        .to_vk();
    VkDeviceAddress scratch_address = vkGetBufferDeviceAddress(_device.handle(),
        &scratch_address_info
    );

    VkAccelerationStructureBuildRangeInfoKHR range = wk::ext::rt::AccelerationStructureBuildRangeInfo{}
        .set_primitive_count(primitive_count)
        .to_vk();
    const VkAccelerationStructureBuildRangeInfoKHR* ranges[] = { &range };

    wk::CommandBuffer command_buffer(
        _device.handle(),
        wk::CommandBufferAllocateInfo{}
            .set_command_pool(_command_pool.handle())
            .to_vk()
    );

    VkCommandBufferBeginInfo begin_info = wk::CommandBufferBeginInfo{}
        .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
        .to_vk();
    vkBeginCommandBuffer(command_buffer.handle(), &begin_info);

    VkAccelerationStructureBuildGeometryInfoKHR build_info = wk::ext::rt::AccelerationStructureBuildGeometryInfo{}
        .set_type(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR)
        .set_mode(VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR)
        .set_geometries(1, &accel_geometry)
        .set_dst_acceleration_structure(_tlas.handle())
        .set_scratch_data(
            wk::DeviceOrHostAddress{}
                .set_device_address(scratch_address)
                .to_vk()
        )
        .to_vk();
    _device_functions.vkCmdBuildAccelerationStructuresKHR(command_buffer.handle(), 1, &build_info, ranges);

    vkEndCommandBuffer(command_buffer.handle());

    VkSubmitInfo gq_submit_info = wk::GraphicsQueueSubmitInfo{}
        .set_command_buffers(1, &command_buffer.handle())
        .to_vk();
    vkQueueSubmit(_device.graphics_queue().handle(),
        1,
        &gq_submit_info,
        VK_NULL_HANDLE
    );

    vkQueueWaitIdle(_device.graphics_queue().handle());
    
    return 0;
}

// ------------------------- pipeline -------------------------

int App::_build_pipeline() {
    // set0: b0 = AS, b1 = storage image
    VkDescriptorSetLayoutBinding binds[] = {
        wk::DescriptorSetLayoutBinding{}
            .set_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
            .set_descriptor_count(1)
            .set_stage_flags(VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
            .to_vk(),
        wk::DescriptorSetLayoutBinding{}
            .set_binding(1)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
            .set_descriptor_count(1)
            .set_stage_flags(VK_SHADER_STAGE_RAYGEN_BIT_KHR)
            .to_vk()
    };

    wk::DescriptorSetLayout descriptor_set_layout(_device.handle(),
        wk::DescriptorSetLayoutCreateInfo{}
            .set_bindings(2, binds)
            .to_vk()
    );

    VkDescriptorSetLayout layouts[] = { descriptor_set_layout.handle() };

    _pipeline_layout = wk::PipelineLayout(_device.handle(), 
        wk::PipelineLayoutCreateInfo{}
            .set_set_layouts(1, layouts)
            .set_push_constant_ranges(0, nullptr)
            .to_vk()
    );

    std::vector<uint8_t> rgen_byte_code = wk::ReadSpirvShader("shaders/rt.rgen.spv");
    std::vector<uint8_t> rmiss_byte_code = wk::ReadSpirvShader("shaders/rt.rmiss.spv");
    std::vector<uint8_t> rchit_byte_code = wk::ReadSpirvShader("shaders/rt.rchit.spv");

    wk::Shader rgen(_device.handle(), 
        wk::ShaderCreateInfo{}
            .set_byte_code(rgen_byte_code.size(), rgen_byte_code.data())
            .to_vk()
    );
    wk::Shader rmiss(_device.handle(), 
        wk::ShaderCreateInfo{}
            .set_byte_code(rmiss_byte_code.size(), rmiss_byte_code.data())
            .to_vk()
    );
    wk::Shader rchit(_device.handle(), 
        wk::ShaderCreateInfo{}
            .set_byte_code(rchit_byte_code.size(), rchit_byte_code.data())
            .to_vk()
    );

    VkPipelineShaderStageCreateInfo stages[] = {
        wk::PipelineShaderStageCreateInfo{}
            .set_stage(VK_SHADER_STAGE_RAYGEN_BIT_KHR)
            .set_module(rgen.handle())
            .set_p_name("main").to_vk(),
        wk::PipelineShaderStageCreateInfo{}
            .set_stage(VK_SHADER_STAGE_MISS_BIT_KHR)
            .set_module(rmiss.handle())
            .set_p_name("main")
            .to_vk(),
        wk::PipelineShaderStageCreateInfo{}
            .set_stage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
            .set_module(rchit.handle())
            .set_p_name("main")
            .to_vk()
    };

    VkRayTracingShaderGroupCreateInfoKHR groups[] = {
        wk::ext::rt::RayTracingShaderGroupCreateInfo{}
            .set_type(VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR)
            .set_general_shader(0)
            .set_closest_hit_shader(VK_SHADER_UNUSED_KHR)
            .set_any_hit_shader(VK_SHADER_UNUSED_KHR)
            .set_intersection_shader(VK_SHADER_UNUSED_KHR)
            .to_vk(),
        wk::ext::rt::RayTracingShaderGroupCreateInfo{}
            .set_type(VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR)
            .set_general_shader(1)
            .set_closest_hit_shader(VK_SHADER_UNUSED_KHR)
            .set_any_hit_shader(VK_SHADER_UNUSED_KHR)
            .set_intersection_shader(VK_SHADER_UNUSED_KHR)
            .to_vk(),
        wk::ext::rt::RayTracingShaderGroupCreateInfo{}
            .set_type(VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR)
            .set_general_shader(VK_SHADER_UNUSED_KHR)
            .set_closest_hit_shader(2)
            .set_any_hit_shader(VK_SHADER_UNUSED_KHR)
            .set_intersection_shader(VK_SHADER_UNUSED_KHR)
            .to_vk()
    };

    _pipeline = wk::ext::rt::RayTracingPipeline(_device.handle(), _device_functions,
        wk::ext::rt::RayTracingPipelineCreateInfo{}
            .set_stages(3, stages)
            .set_groups(3, reinterpret_cast<const VkRayTracingShaderGroupCreateInfoKHR*>(groups))
            .set_max_pipeline_ray_recursion_depth(1)
            .set_layout(_pipeline_layout.handle())
            .to_vk()
    );

    VkDescriptorPoolSize pool_sizes[] = {
        wk::DescriptorPoolSize{}
            .set_type(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
            .set_descriptor_count(1)
            .to_vk(),
        wk::DescriptorPoolSize{}
            .set_type(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
            .set_descriptor_count(1)
            .to_vk()
    };

    _descriptor_pool = wk::DescriptorPool(_device.handle(),
        wk::DescriptorPoolCreateInfo{}
            .set_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .set_max_sets(1)
            .set_pool_sizes(2, pool_sizes)
            .to_vk()
    );

    _descriptor_set = wk::DescriptorSet(_device.handle(),
        wk::DescriptorSetAllocateInfo{}
            .set_descriptor_pool(_descriptor_pool.handle())
            .set_set_layouts(1, layouts)
            .to_vk()
    );

    // AS write (pNext)
    VkWriteDescriptorSetAccelerationStructureKHR accel_write = wk::ext::rt::WriteDescriptorSetAccelerationStructure{}
        .set_acceleration_structures(1, (VkAccelerationStructureKHR*)&_tlas.handle())
        .to_vk();

    VkWriteDescriptorSet w0 = wk::WriteDescriptorSet{}
        .set_dst_set(_descriptor_set.handle())
        .set_dst_binding(0)
        .set_descriptor_type(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
        .set_descriptor_count(1)
        .set_p_next(&accel_write)
        .to_vk();

    // storage image
    VkDescriptorImageInfo img = wk::DescriptorImageInfo{}
        .set_image_layout(VK_IMAGE_LAYOUT_GENERAL)
        .set_image_view(_rt_image_view.handle())
        .set_sampler(VK_NULL_HANDLE)
        .to_vk();

    VkWriteDescriptorSet w1 = wk::WriteDescriptorSet{}
        .set_dst_set(_descriptor_set.handle())
        .set_dst_binding(1)
        .set_descriptor_type(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
        .set_descriptor_count(1)
        .set_p_image_info(&img)
        .to_vk();

    std::array<VkWriteDescriptorSet,2> writes{w0,w1};
    vkUpdateDescriptorSets(_device.handle(), (uint32_t)writes.size(), writes.data(), 0, nullptr);

    return 0;
}

// ------------------------- SBT -------------------------

int App::_build_shader_binding_table() {
    // query RT props
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_properties = wk::ext::rt::PhysicalDeviceRayTracingPipelineProperties{}
        .to_vk();
    VkPhysicalDeviceProperties2 physical_device_properties = wk::PhysicalDeviceProperties2{}
        .set_p_next(&ray_tracing_properties)
        .to_vk();
    vkGetPhysicalDeviceProperties2(_physical_device.handle(), &physical_device_properties);

    const uint32_t handle_size = ray_tracing_properties.shaderGroupHandleSize;
    const uint32_t base_align  = ray_tracing_properties.shaderGroupBaseAlignment;
    const uint32_t group_count = 3;

    std::vector<uint8_t> handles(group_count * handle_size);
    if (_device_functions.vkGetRayTracingShaderGroupHandlesKHR(_device.handle(), _pipeline.handle(), 0, group_count, handles.size(), handles.data()) != VK_SUCCESS) {
        std::cerr << "failed to get group handles" << std::endl;
        return 1; 
    }

    auto align_up = [](VkDeviceSize v, VkDeviceSize a){ return (v + a - 1) & ~(a - 1); };
    VkDeviceSize stride = align_up(handle_size, base_align);
    VkDeviceSize rgen_size = stride, miss_size = stride, hit_size = stride;
    VkDeviceSize sbt_size = rgen_size + miss_size + hit_size;

    _shader_binding_table_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}
            .set_size(sbt_size)
            .set_usage(VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR |
                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT).to_vk(),
        wk::AllocationCreateInfo{}.set_usage(VMA_MEMORY_USAGE_GPU_ONLY).to_vk()
    );
    wk::Buffer staging_buffer = wk::Buffer(_allocator.handle(),
        wk::BufferCreateInfo{}.set_size(sbt_size).set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT).to_vk(),
        wk::AllocationCreateInfo{}.set_usage(VMA_MEMORY_USAGE_CPU_ONLY).to_vk()
    );

    {   // pack [rgen][miss][hit]
        uint8_t* data = nullptr;
        vmaMapMemory(_allocator.handle(), staging_buffer.allocation(), (void**)&data);
        memcpy(data + 0, handles.data(), handle_size);
        memcpy(data + rgen_size, handles.data() + handle_size, handle_size);
        memcpy(data + rgen_size+miss_size, handles.data() + 2 * handle_size, handle_size);
        vmaUnmapMemory(_allocator.handle(), staging_buffer.allocation());

        wk::CommandBuffer upload_command_buffer(_device.handle(),
            wk::CommandBufferAllocateInfo{}
                .set_command_pool(_command_pool.handle())
                .to_vk());

        VkCommandBufferBeginInfo begin_info = wk::CommandBufferBeginInfo{}
            .set_flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
            .to_vk();

        vkBeginCommandBuffer(upload_command_buffer.handle(), &begin_info);

        VkBufferCopy copy = wk::BufferCopy{}
            .set_size(sbt_size)
            .to_vk();

        vkCmdCopyBuffer(upload_command_buffer.handle(), 
            staging_buffer.handle(), _shader_binding_table_buffer.handle(), 1, &copy);

        vkEndCommandBuffer(upload_command_buffer.handle());

        VkSubmitInfo submit_info = wk::GraphicsQueueSubmitInfo{}
            .set_command_buffers(1, &upload_command_buffer.handle())
            .to_vk();

        vkQueueSubmit(_device.graphics_queue().handle(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(_device.graphics_queue().handle());
    }

    VkBufferDeviceAddressInfo base_address_info = wk::BufferDeviceAddressInfo{}
        .set_buffer(_shader_binding_table_buffer.handle())
        .to_vk();
    VkDeviceAddress base_address = vkGetBufferDeviceAddress(_device.handle(), 
        &base_address_info
    );

    _rgen = wk::ext::rt::StridedDeviceAddressRegion{}
        .set_device_address(base_address)
        .set_stride(stride)
        .set_size(rgen_size)
        .to_vk();

    _miss = wk::ext::rt::StridedDeviceAddressRegion{}
        .set_device_address(base_address + rgen_size)
        .set_stride(stride)
        .set_size(miss_size)
        .to_vk();

    _hit  = wk::ext::rt::StridedDeviceAddressRegion{}
        .set_device_address(base_address + rgen_size + miss_size)
        .set_stride(stride)
        .set_size(hit_size)
        .to_vk();

    _call = wk::ext::rt::StridedDeviceAddressRegion{}
        .set_device_address(0)
        .set_stride(0)
        .set_size(0)
        .to_vk();

    return 0;
}

// ------------------------- main loop -------------------------

int App::_main_loop() {
    size_t current_frame_in_flight = 0;
    while (!glfwWindowShouldClose(_window)) {
        vkWaitForFences(_device.handle(), 1, &_frame_in_flight_fences[current_frame_in_flight].handle(), VK_TRUE, UINT64_MAX);
        glfwPollEvents();

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

        // Transition rt image: UNDEFINED -> GENERAL (shader write)
        VkImageMemoryBarrier b0 = wk::ImageMemoryBarrier{}
            .set_old_layout(_is_rt_image_initialized ? VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED)
            .set_new_layout(VK_IMAGE_LAYOUT_GENERAL)
            .set_src_access(_is_rt_image_initialized ? VK_ACCESS_TRANSFER_READ_BIT : 0)
            .set_dst_access(VK_ACCESS_SHADER_WRITE_BIT)
            .set_image(_rt_image.handle())
            .set_aspect(VK_IMAGE_ASPECT_COLOR_BIT)
            .to_vk();
        _is_rt_image_initialized = true;

        vkCmdPipelineBarrier(_command_buffers[current_frame_in_flight].handle(),
            _is_rt_image_initialized ? VK_PIPELINE_STAGE_TRANSFER_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
            VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
            0, 0, nullptr, 0, nullptr, 1, &b0);

        // Transition swapchain image: UNDEFINED -> TRANSFER_DST_OPTIMAL
        VkImageMemoryBarrier b1 = wk::ImageMemoryBarrier{}
            .set_old_layout(_is_swapchain_image_initialized[available_image_index] ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_UNDEFINED)
            .set_new_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            .set_src_access(0)
            .set_dst_access(VK_ACCESS_TRANSFER_WRITE_BIT)
            .set_image(_swapchain.images()[available_image_index])
            .set_aspect(VK_IMAGE_ASPECT_COLOR_BIT)
            .to_vk();
        _is_swapchain_image_initialized[available_image_index] = true;

        vkCmdPipelineBarrier(_command_buffers[current_frame_in_flight].handle(),
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &b1);

        // bind & trace
        vkCmdBindPipeline(_command_buffers[current_frame_in_flight].handle(), 
            VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _pipeline.handle());
        vkCmdBindDescriptorSets(_command_buffers[current_frame_in_flight].handle(),
            VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _pipeline_layout.handle(), 0, 1,
            &_descriptor_set.handle(), 0, nullptr);

        _device_functions.vkCmdTraceRaysKHR(_command_buffers[current_frame_in_flight].handle(),
            &_rgen, &_miss, &_hit, &_call,
            _swapchain.extent().width, _swapchain.extent().height, 1);

        // rt image: GENERAL -> TRANSFER_SRC_OPTIMAL
        VkImageMemoryBarrier b2 = wk::ImageMemoryBarrier{}
            .set_old_layout(VK_IMAGE_LAYOUT_GENERAL)
            .set_new_layout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            .set_src_access(VK_ACCESS_SHADER_WRITE_BIT)
            .set_dst_access(VK_ACCESS_TRANSFER_READ_BIT)
            .set_image(_rt_image.handle())
            .set_aspect(VK_IMAGE_ASPECT_COLOR_BIT)
            .to_vk();

        vkCmdPipelineBarrier(_command_buffers[current_frame_in_flight].handle(),
            VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0,nullptr, 0,nullptr, 1, &b2);

        // copy rt -> swapchain
        VkImageSubresourceLayers sub{}; sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; sub.layerCount = 1;
        VkImageCopy region{};
        region.srcSubresource = sub;
        region.dstSubresource = sub;
        region.extent = wk::Extent(_swapchain.extent()).to_vk();

        vkCmdCopyImage(_command_buffers[current_frame_in_flight].handle(),
            _rt_image.handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            _swapchain.images()[available_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &region);

        // swapchain: TRANSFER_DST_OPTIMAL -> PRESENT_SRC_KHR
        VkImageMemoryBarrier b3 = wk::ImageMemoryBarrier{}
            .set_old_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            .set_new_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
            .set_src_access(VK_ACCESS_TRANSFER_WRITE_BIT)
            .set_dst_access(0)
            .set_image(_swapchain.images()[available_image_index])
            .set_aspect(VK_IMAGE_ASPECT_COLOR_BIT)
            .to_vk();

        vkCmdPipelineBarrier(_command_buffers[current_frame_in_flight].handle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &b3);

        result = vkEndCommandBuffer(_command_buffers[current_frame_in_flight].handle());
        if (result != VK_SUCCESS) {
            std::cerr << "failed to end command buffer" << std::endl;
            return 1;
        }

        std::vector<VkCommandBuffer> gq_command_buffers = { _command_buffers[current_frame_in_flight].handle() };
        std::vector<VkSemaphore> gq_wait_semaphores = { _image_available_semaphores[current_frame_in_flight].handle() };
        std::vector<VkPipelineStageFlags> gq_wait_stage_flags = { VK_PIPELINE_STAGE_TRANSFER_BIT };
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

// ------------------------- swapchain rebuild -------------------------

void App::_rebuild_swapchain() {
    vkDeviceWaitIdle(_device.handle());

    wk::PhysicalDeviceSurfaceSupport physical_device_support = wk::GetPhysicalDeviceSurfaceSupport(_physical_device.handle(), _surface.handle());
    VkSurfaceFormatKHR surface_format = wk::ChooseSurfaceFormat(physical_device_support.formats);
    VkExtent2D e = wk::ChooseSurfaceExtent(_WIDTH, _HEIGHT, physical_device_support.capabilities);

    wk::Swapchain new_swapchain = wk::Swapchain(_device.handle(),
        wk::SwapchainCreateInfo{}
            .set_surface(_surface.handle())
            .set_present_mode(wk::ChooseSurfacePresentationMode(physical_device_support.present_modes))
            .set_min_image_count(std::clamp(
                physical_device_support.capabilities.minImageCount + 1,
                physical_device_support.capabilities.minImageCount,
                physical_device_support.capabilities.maxImageCount))
            .set_image_extent(e)
            .set_image_format(surface_format.format)
            .set_image_color_space(surface_format.colorSpace)
            .set_image_array_layers(1)
            .set_image_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .set_image_sharing_mode(_swapchain.image_sharing_mode())
            .set_queue_family_indices((uint32_t)_swapchain.queue_family_indices().size(),
                                      _swapchain.queue_family_indices().data())
            .set_old_swapchain(_swapchain.handle())
            .to_vk()
    );
    _swapchain = std::move(new_swapchain);
    _is_swapchain_image_initialized.assign(_swapchain.image_count(), false);

    // recreate storage image
    _build_storage_img();
}

// ------------------------- cleanup -------------------------

void App::_cleanup() {
    if (!_device.handle()) return;
    vkDeviceWaitIdle(_device.handle());
    glfwTerminate();
}

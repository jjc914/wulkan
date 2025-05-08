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

int main() {
    int width = 900;
    int height = 600;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(width, height, "example2_rt", nullptr, nullptr);

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
    wk::PhysicalDevice physical_device(instance.handle(), surface.handle(), wk::ext::rt::GetRTRequiredDeviceExtensions());
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

    bool is_swapchain_outdated = false;
    size_t current_frame_in_flight = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    vkDeviceWaitIdle(device.handle());
    glfwTerminate();
}
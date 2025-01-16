cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME wulkan)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(Vulkan REQUIRED)

include_directories("vendor/VulkanMemoryAllocator")
set(LIB_DEPENDENCIES ${LIB_DEPENDENCIES} GPUOpen::VulkanMemoryAllocator)

add_library(${PROJECT_NAME} STATIC
    "include/wulkan_internal.hpp"
    "include/instance.hpp"
    "include/debug_messenger.hpp"
    "include/surface.hpp"
    "include/device.hpp"
    "include/swapchain.hpp"
    "include/render_pass.hpp"
    "include/framebuffer.hpp"
    "include/command_pool.hpp"
    "include/command_buffer.hpp"
    "include/semaphore.hpp"
    "include/fence.hpp"
    "include/shader.hpp"
    "include/pipeline.hpp"
)

target_link_libraries(${PROJECT_NAME} GPUOpen::VulkanMemoryAllocator Vulkan::Vulkan)
cmake_minimum_required(VERSION 3.5)

project(wulkan LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Central output directory for all executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Options
option(WULKAN_ENABLE_GLFW "Build wulkan_glfw extension" ON)
option(WULKAN_BUILD_EXAMPLES "Build the example applications" OFF)

# Module path and shared deps
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CMakeDeps)

# ----------------
# wulkan_glfw extension
# ----------------
if(WULKAN_ENABLE_GLFW)
    add_library(wulkan_glfw STATIC
        "include/ext/glfw/glfw_internal.hpp" "src/ext/glfw_internal.cpp"
        "include/ext/glfw/surface.hpp"
    )
    target_include_directories(wulkan_glfw PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(wulkan_glfw PUBLIC Vulkan::Vulkan glfw)
endif()

# ----------------
# main library
# ----------------
add_library(wulkan STATIC
    "include/wk/wulkan.hpp"
    "include/wk/wulkan_internal.hpp" "src/wulkan_internal.cpp"
    "include/wk/vma_include.hpp"     "src/vma_include.cpp"
    "include/wk/instance.hpp"
    "include/wk/debug_messenger.hpp"
    "include/wk/physical_device.hpp"
    "include/wk/device.hpp"
    "include/wk/swapchain.hpp"
    "include/wk/image_view.hpp"
    "include/wk/image.hpp"
    "include/wk/render_pass.hpp"
    "include/wk/framebuffer.hpp"
    "include/wk/command_pool.hpp"
    "include/wk/command_buffer.hpp"
    "include/wk/semaphore.hpp"
    "include/wk/fence.hpp"
    "include/wk/shader.hpp"
    "include/wk/pipeline_layout.hpp"
    "include/wk/pipeline.hpp"
    "include/wk/descriptor_set_layout.hpp"
)
# target_include_directories(wulkan PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(wulkan PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(wulkan PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

if(WULKAN_ENABLE_GLFW)
    target_link_libraries(wulkan PRIVATE wulkan_glfw)
endif()

# ----------------
# examples
# ----------------
if(WULKAN_BUILD_EXAMPLES AND PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(STATUS "Building examples")
    add_subdirectory("examples/1")
endif()

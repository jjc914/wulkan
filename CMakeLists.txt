cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME wulkan)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

project(${PROJECT_NAME} LANGUAGES CXX)

find_package(Vulkan REQUIRED)
set(LIB_DEPENDENCIES ${LIB_DEPENDENCIES} ${Vulkan_LIBRARIES})

add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE vendor/VulkanMemoryAllocator/include)
set(LIB_DEPENDENCIES ${LIB_DEPENDENCIES} VulkanMemoryAllocator)

# ----------------
# glfw extension library
# ----------------
add_library(${PROJECT_NAME}_glfw STATIC
    "include/ext/glfw/glfw_internal.hpp" "src/ext/glfw_internal.cpp"
    "include/ext/glfw/surface.hpp"
)
set_target_properties(${PROJECT_NAME}_glfw PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}_glfw PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME}_glfw PUBLIC "include")
target_link_libraries(${PROJECT_NAME}_glfw PUBLIC Vulkan::Vulkan glfw)

# ----------------
# wulkan main library
# ----------------
add_library(${PROJECT_NAME} STATIC
    "include/wk/wulkan.hpp"
    "include/wk/wulkan_internal.hpp" "src/wulkan_internal.cpp"
    "include/wk/vma_include.hpp" "src/vma_include.cpp"
    "include/wk/instance.hpp"
    "include/wk/debug_messenger.hpp"
    "include/wk/physical_device.hpp"
    "include/wk/device.hpp"
    "include/wk/swapchain.hpp"
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
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    "include"
    vendor/VulkanMemoryAllocator/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_DEPENDENCIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_glfw)

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

add_library(${PROJECT_NAME} STATIC
    "include/wk/wulkan_internal.hpp" "src/wulkan_internal.cpp"
    "include/wk/instance.hpp"
    "include/wk/debug_messenger.hpp"
    "include/wk/surface.hpp"
    "include/wk/device.hpp"
    "include/wk/swapchain.hpp"
    "include/wk/render_pass.hpp"
    "include/wk/framebuffer.hpp"
    "include/wk/command_pool.hpp"
    "include/wk/command_buffer.hpp"
    "include/wk/semaphore.hpp"
    "include/wk/fence.hpp"
    "include/wk/shader.hpp"
    "include/wk/pipeline.hpp"
    "include/wk/vma_include.hpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    "include"
    vendor/VulkanMemoryAllocator/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_DEPENDENCIES})

# target_include_directories(${PROJECT_NAME} INTERFACE
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#     $<INSTALL_INTERFACE:include>
# )
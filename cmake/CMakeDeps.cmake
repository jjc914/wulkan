# ============================
# dependency configuration
# ============================

find_package(Vulkan REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/VulkanMemoryAllocator)
if(WULKAN_ENABLE_GLFW)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw)
endif()

cmake_minimum_required(VERSION 3.5)

project(wulkan LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Central output directory for all executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Options
option(WULKAN_ENABLE_GLFW "Build wulkan_glfw extension" ON)
option(WULKAN_ENABLE_RT "Build wulkan_rt extension" OFF)
option(WULKAN_BUILD_EXAMPLES "Build the example applications" OFF)

# Module path and shared deps
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CMakeDeps)

# configure extensions
if(WULKAN_ENABLE_GLFW)
    file(GLOB_RECURSE WK_GLFW_HEADERS
        ${CMAKE_SOURCE_DIR}/include/wk/ext/glfw/*.hpp
    )
    add_library(wulkan_glfw STATIC ${WK_GLFW_HEADERS} "src/ext/glfw_internal.cpp")
    target_include_directories(wulkan_glfw PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(wulkan_glfw PUBLIC Vulkan::Vulkan glfw)
endif()

if(WULKAN_ENABLE_RT)
    file(GLOB_RECURSE WK_RT_HEADERS
        ${CMAKE_SOURCE_DIR}/include/wk/ext/rt/*.hpp
    )
    add_library(wulkan_rt STATIC ${WK_RT_HEADERS} "src/ext/rt_internal.cpp")
    target_include_directories(wulkan_rt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(wulkan_rt PUBLIC
        Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator
    )
endif()

file(GLOB_RECURSE WK_HEADERS # get wk headers
    ${CMAKE_SOURCE_DIR}/include/wk/*.hpp
)
list(FILTER WK_HEADERS EXCLUDE REGEX ".*/include/wk/ext/.*") # remove ext from wk headers

# ----------------
# main library
# ----------------
add_library(wulkan STATIC ${WK_HEADERS} "src/wulkan_internal.cpp" "src/vma_include.cpp")
target_include_directories(wulkan PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(wulkan PUBLIC
    Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator
)

if(WULKAN_ENABLE_GLFW)
    target_link_libraries(wulkan PUBLIC wulkan_glfw)
endif()

# ----------------
# examples
# ----------------
if(WULKAN_BUILD_EXAMPLES AND PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(STATUS "Building examples")
    # automatically include every examples/* directory
    file(GLOB EXAMPLE_PATHS
        RELATIVE ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/examples/*)

    foreach(EXAMPLE_DIR IN LISTS EXAMPLE_PATHS)
        if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${EXAMPLE_DIR})
            message(STATUS "Adding example subdir: ${EXAMPLE_DIR}")
            add_subdirectory(${EXAMPLE_DIR})
        endif()
    endforeach()
endif()

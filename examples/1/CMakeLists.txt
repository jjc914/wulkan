project(example1_basic LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------
# imgui
# ----------------
set(IMGUI_PATH ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui)
file(GLOB IMGUI_GLOB
    ${IMGUI_PATH}/imgui*.cpp
    ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
    ${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp
)
add_library(imgui STATIC ${IMGUI_GLOB})
target_include_directories(imgui PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends)
target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)

# ----------------
# executable
# ----------------
add_executable(example1_basic src/main.cpp)

target_include_directories(example1_basic PUBLIC src)
target_link_libraries(example1_basic PRIVATE
    wulkan
    Vulkan::Vulkan
    glfw
    GPUOpen::VulkanMemoryAllocator
    imgui
)

# ----------------
# header-only glm
# ----------------
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_link_libraries(example1_basic PRIVATE glm)
set_target_properties(example1_basic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/example1_basic"
)

# ----------------
# shader compilation
# ----------------
get_filename_component(VULKAN_PATH ${Vulkan_INCLUDE_DIRS} DIRECTORY)
set(GLSL_VALIDATOR ${VULKAN_PATH}/bin/glslangValidator)

file(GLOB_RECURSE GLSL_SOURCE_FILES "shaders/*.vert" "shaders/*.frag")

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(example1_basic Shaders)

add_custom_command(TARGET example1_basic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:example1_basic>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:example1_basic>/shaders"
)

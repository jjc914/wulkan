project(example2_rt LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------
# executable
# ----------------
add_executable(example2_rt src/main.cpp)
target_include_directories(example2_rt PUBLIC src)
set_target_properties(example2_rt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/example2_rt"
)

# ----------------
# header-only glm
# ----------------
target_include_directories(example2_rt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)

# ----------------
# library linkage
# ----------------
target_link_libraries(example2_rt PRIVATE
    wulkan
    Vulkan::Vulkan
    glfw
    GPUOpen::VulkanMemoryAllocator
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

add_custom_target(example2shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(example2_rt example2shaders)

add_custom_command(TARGET example2_rt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:example2_rt>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:example2_rt>/shaders"
)

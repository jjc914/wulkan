#ifndef wk_ext_glfw_GLFW_INTERNAL_HPP
#define wk_ext_glfw_GLFW_INTERNAL_HPP

#include "../../wk/wulkan_internal.hpp"
#include <GLFW/glfw3.h>

namespace wk::ext::glfw {

std::vector<const char*> GetDefaultGlfwRequiredInstanceExtensions() {
    std::vector<const char*> instance_extensions;
    instance_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#ifdef __APPLE__
    instance_extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif
#ifdef WLK_ENABLE_VALIDATION_LAYERS
    instance_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    uint32_t glfw_required_extension_count = 0;
    const char** glfw_required_instance_extensions = glfwGetRequiredInstanceExtensions(&glfw_required_extension_count);
    for (uint32_t i = 0; i < glfw_required_extension_count; ++i) {
        instance_extensions.emplace_back(glfw_required_instance_extensions[i]);
    }
    return instance_extensions;
}

}

#endif
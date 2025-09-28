#ifndef wk_ext_glfw_GLFW_INTERNAL_HPP
#define wk_ext_glfw_GLFW_INTERNAL_HPP

#include "../../wulkan_internal.hpp"
#include <GLFW/glfw3.h>

namespace wk::ext::glfw {

static void DefaultGlfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

std::vector<const char*> GetDefaultGlfwRequiredInstanceExtensions();

}

#endif
#ifndef wk_ext_glfw_GLFW_INTERNAL_HPP
#define wk_ext_glfw_GLFW_INTERNAL_HPP

#include "../../wulkan_internal.hpp"
#include <GLFW/glfw3.h>

#include <string>
#include <sstream>

namespace wk::ext::glfw {

static void DefaultGlfwErrorCallback(int error, const char* description) {
    std::ostringstream oss;
    oss << "glfw error " << error << ": " << description;
    throw std::runtime_error(oss.str());
}

std::vector<const char*> GetDefaultGlfwRequiredInstanceExtensions();

}

#endif
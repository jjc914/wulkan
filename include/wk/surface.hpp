#ifndef wulkan_wk_handle_HPP
#define wulkan_wk_handle_HPP

#include <cstdint>
#include <iostream>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#include <GLFW/glfw3.h>

namespace wk {

class SurfaceCreateInfo {
public:
    SurfaceCreateInfo& set_window(GLFWwindow* window) { _window = window; return *this; }
    SurfaceCreateInfo& set_instance(VkInstance instance) { _instance = instance; return *this; }
private:
    GLFWwindow* _window;
    VkInstance _instance;

    friend class Surface;
};

class Surface {
public:
    Surface(const SurfaceCreateInfo& ci) {
        _create_info = ci;

        if (glfwCreateWindowSurface(ci._instance, ci._window, nullptr, &_handle)) {
            std::cerr << "failed to create surface" << std::endl;
        }
        std::clog << "created surface" << std::endl;
    }

    ~Surface() {
        vkDestroySurfaceKHR(_create_info._instance, _handle, nullptr);
    }

    VkSurfaceKHR handle() const { return _handle; }
private:
    SurfaceCreateInfo _create_info;
    VkSurfaceKHR _handle;
};

}

#endif
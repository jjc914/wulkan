#ifndef wulkan_ext_glfw_SURFACE_HPP
#define wulkan_ext_glfw_SURFACE_HPP

#include "glfw_internal.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>

namespace wk::ext::glfw {

class Surface {
public:
    Surface() : _handle(VK_NULL_HANDLE) {}
    Surface(VkInstance instance, GLFWwindow* window)
        : _instance(instance) 
    {
        if (glfwCreateWindowSurface(_instance, window, nullptr, &_handle)) {
            std::cerr << "failed to create surface" << std::endl;
            exit(-1);
        }
    }

    ~Surface() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(_instance, _handle, nullptr);
        }
    }

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

    Surface(Surface&& other) noexcept
        : _handle(other._handle), 
          _instance(other._instance) 
    {
        other._handle = VK_NULL_HANDLE;
        other._instance = VK_NULL_HANDLE;
    }

    Surface& operator=(Surface&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroySurfaceKHR(_instance, _handle, nullptr);
            }

            _handle = other._handle;
            _instance = other._instance;

            other._handle = VK_NULL_HANDLE;
            other._instance = VK_NULL_HANDLE;
        }
        return *this;
    }

    VkSurfaceKHR handle() const { return _handle; }
private:
    VkSurfaceKHR _handle;
    VkInstance _instance;
};

}

#endif
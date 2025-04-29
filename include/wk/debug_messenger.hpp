#ifndef wulkan_wk_DEBUG_MESSENGER_HPP
#define wulkan_wk_DEBUG_MESSENGER_HPP

#include "wulkan_internal.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>

namespace wk {

class DebugMessengerCreateInfo {
public:
    DebugMessengerCreateInfo& set_pnext(void* pnext) { _pnext = pnext; return *this; }
    DebugMessengerCreateInfo& set_flags(VkDebugUtilsMessengerCreateFlagsEXT flags) { _flags = flags; return *this; }
    DebugMessengerCreateInfo& set_message_severity(VkDebugUtilsMessageSeverityFlagsEXT severity) { _message_severity = severity; return *this; }
    DebugMessengerCreateInfo& set_message_type(VkDebugUtilsMessageTypeFlagsEXT type) { _message_type = type; return *this; }
    DebugMessengerCreateInfo& set_user_callback(PFN_vkDebugUtilsMessengerCallbackEXT callback) { _user_callback = callback; return *this; }
    DebugMessengerCreateInfo& set_user_data(void* user_data) { _user_data = user_data; return *this; }
    DebugMessengerCreateInfo& set_instance(VkInstance instance) { _instance = instance; return *this; }

    VkDebugUtilsMessengerCreateInfoEXT to_vk_debug_messenger_create_info() {
        VkDebugUtilsMessengerCreateInfoEXT ci{};
        ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        ci.pNext = _pnext;
        ci.flags = _flags;
        ci.messageSeverity = _message_severity;
        ci.messageType = _message_type;
        ci.pfnUserCallback = _user_callback;
        ci.pUserData = _user_data;
        return ci;
    }
private:
    void* _pnext = nullptr;
    VkDebugUtilsMessengerCreateFlagsEXT _flags = 0;
    VkDebugUtilsMessageSeverityFlagsEXT _message_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    VkDebugUtilsMessageTypeFlagsEXT _message_type = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    PFN_vkDebugUtilsMessengerCallbackEXT _user_callback = nullptr;
    void* _user_data = nullptr;
    VkInstance _instance = VK_NULL_HANDLE;

    friend class DebugMessenger;
};

class DebugMessenger {
public:
    DebugMessenger(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT ci)
        : _instance(instance) 
    {
        if (!IsValidationLayersSupported()) {
            std::clog << "validation layers requested, but not available" << std::endl;
            return;
        }

        _vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
        if (_vkCreateDebugUtilsMessengerEXT == nullptr) {
            std::cerr << "failed to create vkCreateDebugUtilsMessengerEXT function" << std::endl;
            exit(-1);
        }

        _vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (_vkDestroyDebugUtilsMessengerEXT == nullptr) {
            std::cerr << "failed to create vkDestroyDebugUtilsMessengerEXT function" << std::endl;
            exit(-1);
        }

        if (_vkCreateDebugUtilsMessengerEXT(_instance, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create debug messenger" << std::endl;
            exit(-1);
        }
    }

    ~DebugMessenger() {
        if (_handle != VK_NULL_HANDLE) {
            _vkDestroyDebugUtilsMessengerEXT(_instance, _handle, nullptr);
        }
    }

    DebugMessenger(const DebugMessenger&) = delete;
    DebugMessenger& operator=(const DebugMessenger&) = delete;

    DebugMessenger(DebugMessenger&& other) noexcept
            : _handle(other._handle),
              _vkCreateDebugUtilsMessengerEXT(other._vkCreateDebugUtilsMessengerEXT),
              _vkDestroyDebugUtilsMessengerEXT(other._vkDestroyDebugUtilsMessengerEXT) {
        other._handle = VK_NULL_HANDLE;
        other._vkCreateDebugUtilsMessengerEXT = nullptr;
        other._vkDestroyDebugUtilsMessengerEXT = nullptr;
        other._instance = VK_NULL_HANDLE;
    }

    DebugMessenger& operator=(DebugMessenger&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                _vkDestroyDebugUtilsMessengerEXT(_instance, _handle, nullptr);
            }

            _handle = other._handle;
            _vkCreateDebugUtilsMessengerEXT = other._vkCreateDebugUtilsMessengerEXT;
            _vkDestroyDebugUtilsMessengerEXT = other._vkDestroyDebugUtilsMessengerEXT;
            _instance = other._instance;

            other._handle = VK_NULL_HANDLE;
            other._vkCreateDebugUtilsMessengerEXT = nullptr;
            other._vkDestroyDebugUtilsMessengerEXT = nullptr;
            other._instance = VK_NULL_HANDLE;
        }
        return *this;
    }
private:
    VkDebugUtilsMessengerEXT _handle = VK_NULL_HANDLE;
    PFN_vkCreateDebugUtilsMessengerEXT _vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT _vkDestroyDebugUtilsMessengerEXT = nullptr;

    VkInstance _instance = VK_NULL_HANDLE;
};

}

#endif
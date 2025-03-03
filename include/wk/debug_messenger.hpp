#ifndef wulkan_wk_DEBUG_MESSENGER_HPP
#define wulkan_wk_DEBUG_MESSENGER_HPP

#include <iostream>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "wulkan_internal.hpp"

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
    DebugMessenger(const DebugMessengerCreateInfo& ci) {
        _create_info = ci;

        if (!IsValidationLayersSupported()) {
            std::clog << "validation layers requested, but not available" << std::endl;
        }

        _vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(ci._instance, "vkCreateDebugUtilsMessengerEXT");
        if (_vkCreateDebugUtilsMessengerEXT == nullptr) {
            std::cerr << "failed to create vkCreateDebugUtilsMessengerEXT function" << std::endl;;
        }

        _vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(ci._instance, "vkDestroyDebugUtilsMessengerEXT");
        if (_vkCreateDebugUtilsMessengerEXT == nullptr) {
            std::cerr << "failed to create vkDestroyDebugUtilsMessengerEXT function" << std::endl;;
        }

        VkDebugUtilsMessengerCreateInfoEXT vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        vkci.pNext = ci._pnext;
        vkci.flags = ci._flags;
        vkci.messageSeverity = ci._message_severity;
        vkci.messageType = ci._message_type;
        vkci.pfnUserCallback = ci._user_callback;
        vkci.pUserData = ci._user_data;

        if (_vkCreateDebugUtilsMessengerEXT(ci._instance, &vkci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create debug messenger" << std::endl;
        }
        std::clog << "created debug messenger" << std::endl;
    }

    ~DebugMessenger() {
        _vkDestroyDebugUtilsMessengerEXT(_create_info._instance, _handle, nullptr);
    }
private:
    DebugMessengerCreateInfo _create_info;
    VkDebugUtilsMessengerEXT _handle;
    PFN_vkCreateDebugUtilsMessengerEXT _vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT _vkDestroyDebugUtilsMessengerEXT;
};

}

#endif
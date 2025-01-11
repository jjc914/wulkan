#ifndef wulkan_vk_DEBUGMESSENGER_HPP
#define wulkan_vk_DEBUGMESSENGER_HPP

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace vk {

class DebugMessengerCreateInfo {
public:
    DebugMessengerCreateInfo& set_pnext(void* pnext) { _pnext = pnext; return *this; }
    DebugMessengerCreateInfo& set_flags(VkDebugUtilsMessengerCreateInfoEXT flags) { _flags = flags; return *this; }
    DebugMessengerCreateInfo& set_message_severity(VkDebugUtilsMessageSeverityFlagsEXT severity) { _message_severity = severity; return *this; }
    DebugMessengerCreateInfo& set_message_type(VkDebugUtilsMessageTypeFlagsEXT type) { _message_type = type; return *this; }
    DebugMessengerCreateInfo& set_user_callback(PFN_vkDebugUtilsMessengerCallbackEXT callback) { _user_callback = callback; return *this; }
    DebugMessengerCreateInfo& set_user_data(void* user_data) { _user_data = user_data; return *this; }
private:
    void* _pnext;
    VkDebugUtilsMessengerCreateFlagsEXT _flags;
    VkDebugUtilsMessageSeverityFlagsEXT _message_severity;
    VkDebugUtilsMessageTypeFlagsEXT _message_type;
    PFN_vkDebugUtilsMessengerCallbackEXT _user_callback;
    void* _user_data;

    friend class DebugMessenger;
};

class DebugMessenger {
public:
    DebugMessenger(const DebugMessengerCreateInfo& ci) {
        VkDebugUtilsMessengerCreateInfoEXT vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        vkci.pNext = ci._pnext;
        vkci.flags = ci._flags;
        vkci.messageSeverity = ci._message_severity;
        vkci.messageType = ci._message_type;
        vkci.pfnUserCallback = ci._user_callback;
        vkci.pUserData = ci._user_data;
    }

    ~DebugMessenger() {

    }
private:
};

}

#endif
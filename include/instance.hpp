#ifndef wulkan_wk_INSTANCE_HPP
#define wulkan_wk_INSTANCE_HPP

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "debug_messenger.hpp"

namespace wk {

class ApplicationInfo {
public:
    ApplicationInfo& set_pnext(void* pnext) { _pnext = pnext; return *this; }
    ApplicationInfo& set_application_name(const char* name) { _application_name = name; return *this; }
    ApplicationInfo& set_application_version(uint32_t version) { _application_version = version; return *this; }
    ApplicationInfo& set_engine_name(const char* name) { _engine_name = name; return *this; }
    ApplicationInfo& set_engine_version(uint32_t version) { _engine_version = version; return *this; }
    ApplicationInfo& set_api_version(uint32_t version) { _api_version = version; return *this; }
private:
    void* _pnext = nullptr;
    const char* _application_name = nullptr;
    uint32_t _application_version = 0;
    const char* _engine_name = nullptr;
    uint32_t _engine_version = 0;
    uint32_t _api_version = VK_API_VERSION_1_0;

    friend class Instance;
};

class InstanceCreateInfo {
public:
    InstanceCreateInfo& set_pnext(void* pnext) { _pnext = pnext; return *this; }
    InstanceCreateInfo& set_flags(VkInstanceCreateFlags flags) { _flags = flags; return *this; }
    InstanceCreateInfo& set_extensions(const std::vector<const char*>& extensions) { _extensions = extensions; return *this; }
    InstanceCreateInfo& set_layers(const std::vector<const char*>& layers) { _layers = layers; return *this; }
private:
    void* _pnext = nullptr;
    VkInstanceCreateFlags _flags = 0;
    std::vector<const char*> _extensions;
    std::vector<const char*> _layers;

    friend class Instance;
};

class Instance {
public:
    Instance(const ApplicationInfo& ai, const InstanceCreateInfo& ci) {
        VkApplicationInfo vkai{};
        vkai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkai.pApplicationName = ai._application_name;
        vkai.applicationVersion = ai._application_version;
        vkai.pEngineName = ai._engine_name;
        vkai.engineVersion = ai._engine_version;
        vkai.apiVersion = ai._api_version;

        VkInstanceCreateInfo vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkci.pNext = nullptr;
        vkci.flags = ci._flags;
        vkci.pApplicationInfo = &vkai;
        vkci.enabledLayerCount =(uint32_t)ci._layers.size();
        vkci.ppEnabledLayerNames = ci._layers.data();
        vkci.enabledExtensionCount =(uint32_t)ci._extensions.size();
        vkci.ppEnabledExtensionNames = ci._extensions.data();

        if (vkCreateInstance(&vkci, nullptr, &_handle)) {
            std::cerr << "failed to create Vulkan instance" << std::endl;
        }
        std::clog << "created Vulkan instance" << std::endl;
    }

    ~Instance() {
        vkDestroyInstance(_handle, nullptr);
    }

    VkInstance handle() const { return _handle; }
private:
    VkInstance _handle;
};

}

#endif

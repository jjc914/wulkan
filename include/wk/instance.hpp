#ifndef wulkan_wk_INSTANCE_HPP
#define wulkan_wk_INSTANCE_HPP

#include"wulkan_internal.hpp"
#include "debug_messenger.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace wk {

class ApplicationInfo {
public:
    ApplicationInfo& set_pnext(void* pnext) { _pnext = pnext; return *this; }
    ApplicationInfo& set_application_name(const char* name) { _application_name = name; return *this; }
    ApplicationInfo& set_application_version(uint32_t version) { _application_version = version; return *this; }
    ApplicationInfo& set_engine_name(const char* name) { _engine_name = name; return *this; }
    ApplicationInfo& set_engine_version(uint32_t version) { _engine_version = version; return *this; }
    ApplicationInfo& set_api_version(uint32_t version) { _api_version = version; return *this; }

    VkApplicationInfo to_vk_application_info() {
        VkApplicationInfo vkai{};
        vkai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkai.pApplicationName = _application_name;
        vkai.applicationVersion = _application_version;
        vkai.pEngineName = _engine_name;
        vkai.engineVersion = _engine_version;
        vkai.apiVersion = _api_version;
        return vkai;
    }
private:
    void* _pnext = nullptr;
    const char* _application_name = nullptr;
    uint32_t _application_version = 0;
    const char* _engine_name = nullptr;
    uint32_t _engine_version = 0;
    uint32_t _api_version = VK_API_VERSION_1_0;
};

class InstanceCreateInfo {
public:
    InstanceCreateInfo& set_pnext(void* pnext) { _pnext = pnext; return *this; }
    InstanceCreateInfo& set_flags(VkInstanceCreateFlags flags) { _flags = flags; return *this; }
    InstanceCreateInfo& set_extensions(const std::vector<const char*>& extensions) { _extensions = extensions; return *this; }
    InstanceCreateInfo& set_layers(const std::vector<const char*>& layers) { _layers = layers; return *this; }
    InstanceCreateInfo& set_application_info(const VkApplicationInfo& vkai) { _vkai = vkai; return *this; }

    VkInstanceCreateInfo to_vk_instance_create_info() {
        VkInstanceCreateInfo vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkci.pNext = nullptr;
        vkci.flags = _flags;
        vkci.pApplicationInfo = &_vkai;
        vkci.enabledLayerCount = (uint32_t)_layers.size();
        vkci.ppEnabledLayerNames = _layers.data();
        vkci.enabledExtensionCount =(uint32_t)_extensions.size();
        vkci.ppEnabledExtensionNames = _extensions.data();
        return vkci;
    }
private:
    void* _pnext = nullptr;
    VkApplicationInfo _vkai{};
    VkInstanceCreateFlags _flags = 0;
    std::vector<const char*> _extensions;
    std::vector<const char*> _layers;

    friend class Instance;
};

class Instance {
public:
    Instance(const VkInstanceCreateInfo& ci) {
        if (vkCreateInstance(&ci, nullptr, &_handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance");
        }
    }

    ~Instance() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyInstance(_handle, nullptr);
        }
    }

    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

    Instance(Instance&& other) noexcept
            : _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }

    Instance& operator=(Instance&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyInstance(_handle, nullptr);
            }
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    VkInstance handle() const { return _handle; }
private:
    VkInstance _handle = VK_NULL_HANDLE;
};

}

#endif

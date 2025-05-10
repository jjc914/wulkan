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
    ApplicationInfo& set_p_next(void* p_next) { _p_next = p_next; return *this; }
    ApplicationInfo& set_application_name(const char* name) { _application_name = name; return *this; }
    ApplicationInfo& set_application_version(uint32_t version) { _application_version = version; return *this; }
    ApplicationInfo& set_engine_name(const char* name) { _engine_name = name; return *this; }
    ApplicationInfo& set_engine_version(uint32_t version) { _engine_version = version; return *this; }
    ApplicationInfo& set_api_version(uint32_t version) { _api_version = version; return *this; }

    VkApplicationInfo to_vk_application_info() {
        VkApplicationInfo vkai{};
        vkai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkai.pNext = _p_next;
        vkai.pApplicationName = _application_name;
        vkai.applicationVersion = _application_version;
        vkai.pEngineName = _engine_name;
        vkai.engineVersion = _engine_version;
        vkai.apiVersion = _api_version;
        return vkai;
    }
private:
    void* _p_next = nullptr;
    const char* _application_name = nullptr;
    uint32_t _application_version = 0;
    const char* _engine_name = nullptr;
    uint32_t _engine_version = 0;
    uint32_t _api_version = VK_API_VERSION_1_0;
};

class InstanceCreateInfo {
public:
    InstanceCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    InstanceCreateInfo& set_flags(VkInstanceCreateFlags flags) { _flags = flags; return *this; }
    InstanceCreateInfo& set_extensions(uint32_t count, const char* const* extensions) { 
        _extension_count = count;
        _pp_extensions = extensions;
        return *this;
    }
    InstanceCreateInfo& set_layers(uint32_t count, const char* const* layers) { 
        _layer_count = count;
        _pp_layers = layers;
        return *this;
    }
    InstanceCreateInfo& set_p_application_info(const VkApplicationInfo* p_app_info) { 
        _p_application_info = p_app_info;
        return *this;
    }

    VkInstanceCreateInfo to_vk_instance_create_info() const {
        VkInstanceCreateInfo vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkci.pNext = _p_next;
        vkci.flags = _flags;
        vkci.pApplicationInfo = _p_application_info;
        vkci.enabledExtensionCount = _extension_count;
        vkci.ppEnabledExtensionNames = _pp_extensions;
        vkci.enabledLayerCount = _layer_count;
        vkci.ppEnabledLayerNames = _pp_layers;
        return vkci;
    }

private:
    const void* _p_next = nullptr;
    VkInstanceCreateFlags _flags = 0;
    const VkApplicationInfo* _p_application_info = nullptr;
    uint32_t _extension_count = 0;
    const char* const* _pp_extensions = nullptr;
    uint32_t _layer_count = 0;
    const char* const* _pp_layers = nullptr;
};
    

class Instance {
public:
    Instance(const VkInstanceCreateInfo& ci) {
        if (vkCreateInstance(&ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create vulkan instance" << std::endl;
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

    const VkInstance& handle() const { return _handle; }
private:
    VkInstance _handle = VK_NULL_HANDLE;
};

}

#endif

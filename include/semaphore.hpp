#ifndef wulkan_wk_SEMAPHORE_HPP
#define wulkan_wk_SEMAPHORE_HPP

#include <cstdint>
#include <iostream>

#include <vulkan/vulkan_core.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace wk {

class SemaphoreCreateInfo {
public:
    SemaphoreCreateInfo& set_device(VkDevice device) { _device = device; return *this; }
private:
    VkDevice _device = VK_NULL_HANDLE;

    friend class Semaphore;
};

class Semaphore {
public:
    Semaphore(const SemaphoreCreateInfo& ci) {
        VkSemaphoreCreateInfo vkci{};
        vkci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        if (vkCreateSemaphore(ci._device, &vkci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create semaphore" << std::endl;;
        }

        _device = ci._device;

        std::clog << "created semaphore" << std::endl;
    }

    ~Semaphore() {
        vkDestroySemaphore(_device, _handle, nullptr);
    }   

    VkSemaphore handle() const { return _handle; }
private:
    VkSemaphore _handle;
    VkDevice _device;
};

}

#endif
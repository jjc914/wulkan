#ifndef wulkan_wk_EVENT_HPP
#define wulkan_wk_EVENT_HPP

#include "wulkan_internal.hpp"

namespace wk {

class Event {
public:
    Event() = default;
    Event(VkDevice device, const VkEventCreateInfo& ci) 
        : _device(device) 
    {
        if (vkCreateEvent(device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create event" << std::endl;
        }
    }

    ~Event() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroyEvent(_device, _handle, nullptr);
        }
    }

    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event(Event&& other) noexcept : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    Event& operator=(Event&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroyEvent(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkEvent& handle() const { return _handle; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkEvent _handle = VK_NULL_HANDLE;
};

class EventCreateInfo {
public:
    EventCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    EventCreateInfo& set_flags(VkEventCreateFlags flags) { _flags = flags; return *this; }

    VkEventCreateInfo to_vk() const {
        VkEventCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkEventCreateFlags _flags = 0;
};

} // namespace wk

#endif
#ifndef wulkan_wk_QUEUE_HPP
#define wulkan_wk_QUEUE_HPP

#include <vulkan/vulkan.h>

namespace wk {

class Queue {
public:
    Queue() = default;
    Queue(VkDevice device, uint32_t family_index, uint32_t queue_index = 0) : _family_index(family_index) {
        vkGetDeviceQueue(device, family_index, queue_index, &_queue);
    }

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&& other) : _queue(other._queue) {
        other._queue = VK_NULL_HANDLE;
    }
    Queue& operator=(Queue&& other) {
        if (this != &other) {
            _queue = other._queue;
            _family_index = other._family_index;
            other._queue = VK_NULL_HANDLE;
            other._family_index = static_cast<uint32_t>(-1);
        }
        return *this;
    }

    ~Queue() = default;

    VkQueue handle() const { return _queue; }
    uint32_t family_index() const { return _family_index; }

private:
    VkQueue _queue = VK_NULL_HANDLE;
    uint32_t _family_index;
};

} // namespace wk

#endif
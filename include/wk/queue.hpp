#ifndef wulkan_wk_QUEUE_HPP
#define wulkan_wk_QUEUE_HPP

#include <vulkan/vulkan.h>

namespace wk {

class Queue {
public:
    Queue() noexcept = default;
    explicit Queue(VkQueue queue) noexcept : _queue(queue) {}

    Queue(VkDevice device, uint32_t family_index, uint32_t queue_index = 0) {
        vkGetDeviceQueue(device, family_index, queue_index, &_queue);
    }

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&& other) noexcept : _queue(other._queue) {
        other._queue = VK_NULL_HANDLE;
    }
    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            _queue = other._queue;
            other._queue = VK_NULL_HANDLE;
        }
        return *this;
    }

    ~Queue() = default;

    VkQueue handle() const noexcept { return _queue; }

private:
    VkQueue _queue = VK_NULL_HANDLE;
};

} // namespace wk

#endif
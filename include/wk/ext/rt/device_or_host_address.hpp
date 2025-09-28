#ifndef wk_ext_rt_DEVICE_OR_HOST_ADDRESS_HPP
#define wk_ext_rt_DEVICE_OR_HOST_ADDRESS_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class DeviceOrHostAddress {
public:
    DeviceOrHostAddress& set_device_address(VkDeviceAddress addr) {
        _device_address = addr;
        return *this;
    }
    DeviceOrHostAddress& set_host_address(void* addr) {
        _host_address = addr;
        return *this;
    }

    VkDeviceOrHostAddressKHR to_vk() const {
        VkDeviceOrHostAddressKHR addr{};
        addr.deviceAddress = _device_address;
        addr.hostAddress = _host_address;
        return addr;
    }

private:
    VkDeviceAddress _device_address = 0;
    void* _host_address = nullptr;
};

}

#endif

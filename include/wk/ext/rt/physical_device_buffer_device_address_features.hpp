#ifndef wk_ext_rt_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_HPP
#define wk_ext_rt_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class PhysicalDeviceBufferDeviceAddressFeatures {
public:
    PhysicalDeviceBufferDeviceAddressFeatures& set_p_next(void* p_next) { _p_next = p_next; return *this; }
    PhysicalDeviceBufferDeviceAddressFeatures& set_buffer_device_address(VkBool32 enabled) { _buffer_device_address = enabled; return *this; }
    PhysicalDeviceBufferDeviceAddressFeatures& set_buffer_device_address_capture_replay(VkBool32 enabled) { _buffer_device_address_capture_replay = enabled; return *this; }
    PhysicalDeviceBufferDeviceAddressFeatures& set_buffer_device_address_multi_device(VkBool32 enabled) { _buffer_device_address_multi_device = enabled; return *this; }

    VkPhysicalDeviceBufferDeviceAddressFeaturesEXT to_vk() const {
        VkPhysicalDeviceBufferDeviceAddressFeaturesEXT features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        features.pNext = _p_next;
        features.bufferDeviceAddress = _buffer_device_address;
        features.bufferDeviceAddressCaptureReplay = _buffer_device_address_capture_replay;
        features.bufferDeviceAddressMultiDevice = _buffer_device_address_multi_device;
        return features;
    }

private:
    void* _p_next = nullptr;
    VkBool32 _buffer_device_address = VK_FALSE;
    VkBool32 _buffer_device_address_capture_replay = VK_FALSE;
    VkBool32 _buffer_device_address_multi_device = VK_FALSE;
};

}

#endif 
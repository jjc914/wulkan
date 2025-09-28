#ifndef wk_ext_rt_ACCELERATION_STRUCTURE_HPP
#define wk_ext_rt_ACCELERATION_STRUCTURE_HPP

#include "../../wulkan_internal.hpp"

namespace wk::ext::rt {

class WriteDescriptorSetAccelerationStructure {
public:
    WriteDescriptorSetAccelerationStructure& set_next(const void* next){ _next = next; return *this; }
    WriteDescriptorSetAccelerationStructure& set_acceleration_structures(uint32_t count, const VkAccelerationStructureKHR* ptr){ _count = count; _ptr = ptr; return *this; }

    VkWriteDescriptorSetAccelerationStructureKHR to_vk() const {
        VkWriteDescriptorSetAccelerationStructureKHR s{};
        s.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        s.pNext = _next;
        s.accelerationStructureCount = _count;
        s.pAccelerationStructures = _ptr;
        return s;
    }
private:
    const void* _next = nullptr;
    uint32_t _count = 0;
    const VkAccelerationStructureKHR* _ptr = nullptr;
};


class TransformMatrix {
public:
    TransformMatrix() {
        _matrix[0][0] = 1.0f; _matrix[0][1] = 0.0f; _matrix[0][2] = 0.0f; _matrix[0][3] = 0.0f;
        _matrix[1][0] = 0.0f; _matrix[1][1] = 1.0f; _matrix[1][2] = 0.0f; _matrix[1][3] = 0.0f;
        _matrix[2][0] = 0.0f; _matrix[2][1] = 0.0f; _matrix[2][2] = 1.0f; _matrix[2][3] = 0.0f;
    }

    TransformMatrix& set_matrix(const float matrix[3][4]) {
        std::memcpy(_matrix, matrix, sizeof(_matrix));
        return *this;
    }
    TransformMatrix& set_element(uint32_t row, uint32_t col, float value) {
        _matrix[row][col] = value;
        return *this;
    }

    VkTransformMatrixKHR to_vk() const {
        VkTransformMatrixKHR tm{};
        std::memcpy(tm.matrix, _matrix, sizeof(_matrix));
        return tm;
    }
private:
    float _matrix[3][4];
};

class AccelerationStructureDeviceAddressInfo {
public:
    AccelerationStructureDeviceAddressInfo& set_p_next(const void* p) { _p_next = p; return *this; }
    AccelerationStructureDeviceAddressInfo& set_acceleration_structure(VkAccelerationStructureKHR a) { _accel = a; return *this; }

    VkAccelerationStructureDeviceAddressInfoKHR to_vk() const {
        VkAccelerationStructureDeviceAddressInfoKHR v{};
        v.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        v.pNext = _p_next;
        v.accelerationStructure = _accel;
        return v;
    }

private:
    const void* _p_next = nullptr;
    VkAccelerationStructureKHR _accel = VK_NULL_HANDLE;
};

class AccelerationStructure {
public:
    AccelerationStructure(VkDevice device, const VkAccelerationStructureCreateInfoKHR& ci)
        : _device(device) {
        vkCreateAccelerationStructureKHR(_device, &ci, nullptr, &_handle);
    }

    ~AccelerationStructure() {
        if (_handle != VK_NULL_HANDLE)
            vkDestroyAccelerationStructureKHR(_device, _handle, nullptr);
    }

    AccelerationStructure(const AccelerationStructure&) = delete;
    AccelerationStructure& operator=(const AccelerationStructure&) = delete;

    AccelerationStructure(AccelerationStructure&& other) noexcept
        : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    AccelerationStructure& operator=(AccelerationStructure&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE)
                vkDestroyAccelerationStructureKHR(_device, _handle, nullptr);
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkAccelerationStructureKHR& handle() const { return _handle; }

    VkDeviceAddress device_address() const {
        VkAccelerationStructureDeviceAddressInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        info.accelerationStructure = _handle;
        return vkGetAccelerationStructureDeviceAddressKHR(_device, &info);
    }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkAccelerationStructureKHR _handle = VK_NULL_HANDLE;
};

}

#endif

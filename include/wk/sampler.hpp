#ifndef wulkan_wk_SAMPLER_HPP
#define wulkan_wk_SAMPLER_HPP

#include "wulkan_internal.hpp"

namespace wk {

class Sampler {
public:
    Sampler() = default;
    Sampler(VkDevice device, const VkSamplerCreateInfo& ci) 
        : _device(device) 
    {
        if (vkCreateSampler(device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create sampler" << std::endl;
        }
    }

    ~Sampler() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroySampler(_device, _handle, nullptr);
        }
    }

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    Sampler(Sampler&& other) noexcept : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    Sampler& operator=(Sampler&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroySampler(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkSampler& handle() const { return _handle; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkSampler _handle = VK_NULL_HANDLE;
};

class SamplerCreateInfo {
public:
    SamplerCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    SamplerCreateInfo& set_flags(VkSamplerCreateFlags flags) { _flags = flags; return *this; }
    SamplerCreateInfo& set_mag_filter(VkFilter filter) { _mag_filter = filter; return *this; }
    SamplerCreateInfo& set_min_filter(VkFilter filter) { _min_filter = filter; return *this; }
    SamplerCreateInfo& set_mipmap_mode(VkSamplerMipmapMode mode) { _mipmap_mode = mode; return *this; }
    SamplerCreateInfo& set_address_mode_u(VkSamplerAddressMode mode) { _address_mode_u = mode; return *this; }
    SamplerCreateInfo& set_address_mode_v(VkSamplerAddressMode mode) { _address_mode_v = mode; return *this; }
    SamplerCreateInfo& set_address_mode_w(VkSamplerAddressMode mode) { _address_mode_w = mode; return *this; }
    SamplerCreateInfo& set_mip_lod_bias(float bias) { _mip_lod_bias = bias; return *this; }
    SamplerCreateInfo& set_anisotropy_enable(VkBool32 enable) { _anisotropy_enable = enable; return *this; }
    SamplerCreateInfo& set_max_anisotropy(float max) { _max_anisotropy = max; return *this; }
    SamplerCreateInfo& set_compare_enable(VkBool32 enable) { _compare_enable = enable; return *this; }
    SamplerCreateInfo& set_compare_op(VkCompareOp op) { _compare_op = op; return *this; }
    SamplerCreateInfo& set_min_lod(float lod) { _min_lod = lod; return *this; }
    SamplerCreateInfo& set_max_lod(float lod) { _max_lod = lod; return *this; }
    SamplerCreateInfo& set_border_color(VkBorderColor color) { _border_color = color; return *this; }
    SamplerCreateInfo& set_unnormalized_coordinates(VkBool32 unnormalized) { _unnormalized_coordinates = unnormalized; return *this; }

    VkSamplerCreateInfo to_vk() const {
        VkSamplerCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        ci.pNext = _p_next;
        ci.flags = _flags;
        ci.magFilter = _mag_filter;
        ci.minFilter = _min_filter;
        ci.mipmapMode = _mipmap_mode;
        ci.addressModeU = _address_mode_u;
        ci.addressModeV = _address_mode_v;
        ci.addressModeW = _address_mode_w;
        ci.mipLodBias = _mip_lod_bias;
        ci.anisotropyEnable = _anisotropy_enable;
        ci.maxAnisotropy = _max_anisotropy;
        ci.compareEnable = _compare_enable;
        ci.compareOp = _compare_op;
        ci.minLod = _min_lod;
        ci.maxLod = _max_lod;
        ci.borderColor = _border_color;
        ci.unnormalizedCoordinates = _unnormalized_coordinates;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkSamplerCreateFlags _flags = 0;
    VkFilter _mag_filter = VK_FILTER_LINEAR;
    VkFilter _min_filter = VK_FILTER_LINEAR;
    VkSamplerMipmapMode _mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VkSamplerAddressMode _address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode _address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode _address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    float _mip_lod_bias = 0.0f;
    VkBool32 _anisotropy_enable = VK_FALSE;
    float _max_anisotropy = 1.0f;
    VkBool32 _compare_enable = VK_FALSE;
    VkCompareOp _compare_op = VK_COMPARE_OP_ALWAYS;
    float _min_lod = 0.0f;
    float _max_lod = 0.0f;
    VkBorderColor _border_color = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    VkBool32 _unnormalized_coordinates = VK_FALSE;
};

} // namespace wk

#endif
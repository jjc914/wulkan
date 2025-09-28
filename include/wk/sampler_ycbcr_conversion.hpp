#ifndef wulkan_wk_SAMPLER_YCBCR_CONVERSION_HPP
#define wulkan_wk_SAMPLER_YCBCR_CONVERSION_HPP

#include "wulkan_internal.hpp"

namespace wk {

class SamplerYcbcrConversionCreateInfo {
public:
    SamplerYcbcrConversionCreateInfo& set_p_next(const void* p_next) { _p_next = p_next; return *this; }
    SamplerYcbcrConversionCreateInfo& set_format(VkFormat format) { _format = format; return *this; }
    SamplerYcbcrConversionCreateInfo& set_ycbcr_model(VkSamplerYcbcrModelConversion model) { _ycbcr_model = model; return *this; }
    SamplerYcbcrConversionCreateInfo& set_ycbcr_range(VkSamplerYcbcrRange range) { _ycbcr_range = range; return *this; }
    SamplerYcbcrConversionCreateInfo& set_components(VkComponentMapping components) { _components = components; return *this; }
    SamplerYcbcrConversionCreateInfo& set_x_chroma_offset(VkChromaLocation offset) { _x_chroma_offset = offset; return *this; }
    SamplerYcbcrConversionCreateInfo& set_y_chroma_offset(VkChromaLocation offset) { _y_chroma_offset = offset; return *this; }
    SamplerYcbcrConversionCreateInfo& set_chroma_filter(VkFilter filter) { _chroma_filter = filter; return *this; }
    SamplerYcbcrConversionCreateInfo& set_force_explicit_reconstruction(VkBool32 force) { _force_explicit_reconstruction = force; return *this; }

    VkSamplerYcbcrConversionCreateInfo to_vk() const {
        VkSamplerYcbcrConversionCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
        ci.pNext = _p_next;
        ci.format = _format;
        ci.ycbcrModel = _ycbcr_model;
        ci.ycbcrRange = _ycbcr_range;
        ci.components = _components;
        ci.xChromaOffset = _x_chroma_offset;
        ci.yChromaOffset = _y_chroma_offset;
        ci.chromaFilter = _chroma_filter;
        ci.forceExplicitReconstruction = _force_explicit_reconstruction;
        return ci;
    }

private:
    const void* _p_next = nullptr;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkSamplerYcbcrModelConversion _ycbcr_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
    VkSamplerYcbcrRange _ycbcr_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
    VkComponentMapping _components{};
    VkChromaLocation _x_chroma_offset = VK_CHROMA_LOCATION_COSITED_EVEN;
    VkChromaLocation _y_chroma_offset = VK_CHROMA_LOCATION_COSITED_EVEN;
    VkFilter _chroma_filter = VK_FILTER_LINEAR;
    VkBool32 _force_explicit_reconstruction = VK_FALSE;
};

class SamplerYcbcrConversion {
public:
    SamplerYcbcrConversion() noexcept = default;
    SamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo& ci) 
        : _device(device) 
    {
        if (vkCreateSamplerYcbcrConversion(device, &ci, nullptr, &_handle) != VK_SUCCESS) {
            std::cerr << "failed to create sampler ycbcr conversion" << std::endl;
        }
    }

    ~SamplerYcbcrConversion() {
        if (_handle != VK_NULL_HANDLE) {
            vkDestroySamplerYcbcrConversion(_device, _handle, nullptr);
        }
    }

    SamplerYcbcrConversion(const SamplerYcbcrConversion&) = delete;
    SamplerYcbcrConversion& operator=(const SamplerYcbcrConversion&) = delete;

    SamplerYcbcrConversion(SamplerYcbcrConversion&& other) noexcept : _device(other._device), _handle(other._handle) {
        other._handle = VK_NULL_HANDLE;
    }
    SamplerYcbcrConversion& operator=(SamplerYcbcrConversion&& other) noexcept {
        if (this != &other) {
            if (_handle != VK_NULL_HANDLE) {
                vkDestroySamplerYcbcrConversion(_device, _handle, nullptr);
            }
            _device = other._device;
            _handle = other._handle;
            other._handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    const VkSamplerYcbcrConversion& handle() const { return _handle; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkSamplerYcbcrConversion _handle = VK_NULL_HANDLE;
};

} // namespace wk

#endif

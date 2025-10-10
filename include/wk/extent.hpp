#ifndef wulkan_wk_EXTENT_HPP
#define wulkan_wk_EXTENT_HPP

#include "wulkan_internal.hpp"

namespace wk {

class Extent {
public:
    Extent() {}
    Extent(uint32_t width, uint32_t height) {
        _width = width;
        _height = height;
        _depth = 1;
    }
    Extent(uint32_t width, uint32_t height, uint32_t depth) {
        _width = width;
        _height = height;
        _depth = depth;
    }
    Extent(VkExtent2D e) {
        _width = e.width;
        _height = e.height;
        _depth = 1;
    }
    Extent(VkExtent3D e) {
        _width = e.width;
        _height = e.height;
        _depth = e.depth;
    }

    Extent& set_width(uint32_t width) { _width = width; return *this; }
    Extent& set_height(uint32_t height) { _height = height; return *this; }
    Extent& set_depth(uint32_t depth) { _depth = depth; return *this; }

    VkExtent2D to_vk_extent_2d() const {
        return VkExtent2D{ _width, _height };
    }
    VkExtent3D to_vk() const {
        return VkExtent3D{ _width, _height, _depth };
    }

private:
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _depth = 1;
};

} // namespace wk

#endif // wulkan_wk_EXTENT_HPP
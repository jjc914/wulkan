#ifndef wulkan_wk_VMA_INCLUDE_HPP
#define wulkan_wk_VMA_INCLUDE_HPP

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

#else
#include "vk_mem_alloc.h"
#endif
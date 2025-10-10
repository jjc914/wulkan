#ifndef wulkan_wk_WULKAN_HPP
#define wulkan_wk_WULKAN_HPP

#include "extent.hpp"

// Core instance & devices
#include "instance.hpp"
#include "debug_messenger.hpp"
#include "physical_device.hpp"
#include "device.hpp"
#include "queue.hpp"

// Swapchain & presentation
#include "swapchain.hpp"

// Synchronization
#include "semaphore.hpp"
#include "fence.hpp"
#include "event.hpp"

// Command system
#include "command_pool.hpp"
#include "command_buffer.hpp"

// Render targets
#include "render_pass.hpp"
#include "framebuffer.hpp"
#include "image.hpp"
#include "image_view.hpp"
#include "sampler.hpp"
#include "sampler_ycbcr_conversion.hpp"

// Shaders & pipelines
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "pipeline.hpp"
#include "pipeline_cache.hpp"

// Descriptors
#include "descriptor_pool.hpp"
#include "descriptor_set_layout.hpp"
#include "descriptor_set.hpp"
#include "descriptor_update_template.hpp"

// Queries
#include "query_pool.hpp"

// Validation
#include "validation_cache.hpp"

// Memory (VMA + wrappers)
#include "allocator.hpp"
#include "buffer.hpp"

// Sync
#include "sync.hpp"

#endif

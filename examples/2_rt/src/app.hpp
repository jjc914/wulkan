#ifndef BASIC_2_APP_HPP
#define BASIC_2_APP_HPP

#define WLK_ENABLE_VALIDATION_LAYERS
#include <wk/wulkan.hpp>
#include <wk/ext/glfw/glfw_internal.hpp>
#include <wk/ext/glfw/surface.hpp>

#include <wk/ext/rt/acceleration_structure.hpp>
#include <wk/ext/rt/ray_tracing_pipeline.hpp>
#include <wk/ext/rt/deferred_operation.hpp>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <array>

struct Vertex {
    glm::vec3 position;
};

class App {
public:
    App();
    ~App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&& other) noexcept = default;
    App& operator=(App&& other) noexcept = default;

    int run();

private:
    // lifecycle
    int _init_window();
    int _init_vulkan();
    int _main_loop();
    void _rebuild_swapchain();
    void _cleanup();

    // rt setup (impl details kept local inside these; no extra members leaked)
    int _build_storage_img();
    int _build_blas(uint32_t graphics_family_index);
    int _build_tlas(uint32_t graphics_family_index);
    int _build_pipeline();
    // int _build_descriptors();
    int _build_shader_binding_table();
    int record_trace_copy_commands_(uint32_t frame, uint32_t swap_img_index);

private:
    // ---------- geometry ----------
    const std::vector<Vertex> _VERTICES = {
        {{-1.0f, -1.0f,  1.0f} /*xyz*/}, // front
        {{ 1.0f, -1.0f,  1.0f},       },
        {{ 1.0f,  1.0f,  1.0f},       },
        {{-1.0f,  1.0f,  1.0f},       },

        {{-1.0f, -1.0f, -1.0f},       }, // back
        {{ 1.0f, -1.0f, -1.0f},       },
        {{ 1.0f,  1.0f, -1.0f},       },
        {{-1.0f,  1.0f, -1.0f},       },
    };
    const std::vector<uint16_t> _INDICES = {
        0, 1, 2, 2, 3, 0, // front
        1, 5, 6, 6, 2, 1, // right
        7, 6, 5, 5, 4, 7, // back
        4, 0, 3, 3, 7, 4, // left
        3, 2, 6, 6, 7, 3, // top
        4, 5, 1, 1, 0, 4, // bottom
    };

    // ---------- renderer constants ----------
    const size_t _MAX_FRAMES_IN_FLIGHT = 2;
    int _WIDTH = 900;
    int _HEIGHT = 600;

    GLFWwindow* _window = nullptr;

    wk::Instance _instance;
    wk::DebugMessenger _debug_messenger;
    wk::ext::glfw::Surface _surface;
    wk::PhysicalDevice _physical_device;
    wk::Device _device;
    wk::ext::rt::DeviceFunctions _device_functions;

    wk::CommandPool _command_pool;
    wk::Allocator _allocator;

    wk::Swapchain _swapchain;
    std::vector<bool> _is_swapchain_image_initialized;

    VkFormat _rt_format = VK_FORMAT_R8G8B8A8_UNORM;
    wk::Image _rt_image;
    wk::ImageView _rt_image_view;
    bool _is_rt_image_initialized;

    wk::Buffer _vertex_buffer, _index_buffer;

    wk::Buffer _blas_buffer, _tlas_buffer;
    wk::ext::rt::AccelerationStructure _blas, _tlas;

    wk::Buffer _instance_buffer;

    wk::PipelineLayout _pipeline_layout;
    wk::ext::rt::RayTracingPipeline _pipeline;

    wk::DescriptorPool _descriptor_pool;
    wk::DescriptorSet  _descriptor_set;

    wk::Buffer _shader_binding_table_buffer;
    VkStridedDeviceAddressRegionKHR _rgen{0,0,0}, _miss{0,0,0}, _hit{0,0,0}, _call{0,0,0};

    std::vector<wk::CommandBuffer> _command_buffers;
    std::vector<wk::Semaphore> _image_available_semaphores;
    std::vector<wk::Semaphore> _render_finished_semaphores;
    std::vector<wk::Fence> _frame_in_flight_fences;
};

#endif // BASIC_2_APP_HPP
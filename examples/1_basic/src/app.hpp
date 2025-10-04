#ifndef BASIC_1_APP_HPP
#define BASIC_1_APP_HPP

#define WLK_ENABLE_VALIDATION_LAYERS
#include <wk/wulkan.hpp>
#include <wk/ext/glfw/glfw_internal.hpp>
#include <wk/ext/glfw/surface.hpp>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <cstring>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
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
    int _init_window();
    int _init_vulkan();
    int _init_geometry();
    int _main_loop();
    void _rebuild_swapchain();
    void _cleanup();

    // ---------- geometry ----------
    const std::vector<Vertex> _VERTICES = {
        {{-1.0f, -1.0f,  1.0f} /*xyz*/, {1, 0, 0} /*rgb*/}, // front
        {{ 1.0f, -1.0f,  1.0f},         {0, 1, 0}},
        {{ 1.0f,  1.0f,  1.0f},         {0, 0, 1}},
        {{-1.0f,  1.0f,  1.0f},         {1, 1, 0}},

        {{-1.0f, -1.0f, -1.0f},         {1, 0, 1}}, // back
        {{ 1.0f, -1.0f, -1.0f},         {0, 1, 1}},
        {{ 1.0f,  1.0f, -1.0f},         {1, 1, 1}},
        {{-1.0f,  1.0f, -1.0f},         {0, 0, 0}},
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

    const std::vector<VkFormat> _DEPTH_FORMATS = {
        VK_FORMAT_D32_SFLOAT, 
        VK_FORMAT_D32_SFLOAT_S8_UINT, 
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    GLFWwindow* _window;

    wk::Instance _instance;
    wk::DebugMessenger _debug_messenger;
    wk::ext::glfw::Surface _surface;
    wk::PhysicalDevice _physical_device;
    wk::Device _device;

    wk::CommandPool _command_pool;
    wk::Allocator _allocator;

    wk::RenderPass _render_pass;

    wk::Swapchain _swapchain;

    std::vector<wk::Image> _depth_images;
    std::vector<wk::ImageView> _depth_image_views;
    std::vector<wk::Framebuffer> _framebuffers;

    wk::PipelineLayout _pipeline_layout;
    wk::Pipeline _pipeline;

    wk::Buffer _vertex_buffer;
    wk::Buffer _index_buffer;
    std::vector<wk::Buffer> _uniform_buffers;

    wk::DescriptorPool _descriptor_pool;
    std::vector<wk::DescriptorSet> _descriptor_sets;

    std::vector<wk::CommandBuffer> _command_buffers;
    std::vector<wk::Semaphore> _image_available_semaphores{};
    std::vector<wk::Semaphore> _render_finished_semaphores{};
    std::vector<wk::Fence> _frame_in_flight_fences{};
};

#endif // BASIC_1_APP_HPP
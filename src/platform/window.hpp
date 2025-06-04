#ifndef ROBOTACT_PLATFORM_WINDOW_HPP
#define ROBOTACT_PLATFORM_WINDOW_HPP

#include "../config/window_settings.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <iostream>


namespace RoboTact::Platform {

/*
 * @brief RAII wrapper for GLFW window management
 * 
 * Handles window creation, lifecycle and OpenGL context management
*/

class Window {
public:
    /**
     * @brief Construct a new Window object
     * @param window_settings Configuration for the window
     * @throws std::runtime_error using custom RA_ASSERT if window creation fails
     */

    explicit Window(const Config::WindowSettings& window_settings = Config::WindowSettings{});
    ~Window() noexcept;

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    void poll_events() const noexcept { glfwPollEvents(); };
    void glfw_swap_buffers() const noexcept;
    [[nodiscard]] GLFWwindow* get_glfw_window() const noexcept { return m_window; }
    [[nodiscard]] bool should_close() const noexcept {return glfwWindowShouldClose(m_window); }
    [[nodiscard]] glm::ivec2 get_size() const noexcept { return m_window_settings.size; }
    [[nodiscard]] int get_width() const noexcept { return m_window_settings.size.x; }
    [[nodiscard]] int get_height() const noexcept { return m_window_settings.size.y; }
private:
    void initialize();
    void cleanup() noexcept;
    void update_window_size(int width, int height) noexcept;

    static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
    static void glfw_error_callback(int error, const char* description);

    Config::WindowSettings m_window_settings;
    GLFWwindow*  m_window{nullptr};
};


} // namespace RoboTact::Platform

#endif // ROBOTACT_PLATFORM_WINDOW_HPP

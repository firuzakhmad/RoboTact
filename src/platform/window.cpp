#include "window.hpp"
#include "../core/assert.hpp"
#include "../core/logger.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <iostream>

namespace RoboTact::Platform {

Window::Window(const Config::WindowSettings& window_settings)
    : m_window_settings(window_settings) 
{
    m_window_settings.validate();
    initialize();
}

Window::~Window() noexcept { cleanup(); }

void Window::cleanup() noexcept 
{
    if (m_window) 
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}



void Window::initialize() 
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        RA_ASSERT(false && "Failed to initialize GLFW!");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_window_settings.gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_window_settings.gl_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, m_window_settings.resizable ? GLFW_TRUE : GLFW_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(
        m_window_settings.size.x,
        m_window_settings.size.y,
        m_window_settings.title.c_str(),
        m_window_settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr
    );

    RA_ASSERT(m_window && "Failed to create GLFW window");

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(m_window_settings.v_sync ? 1 : 0);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) 
    {
        cleanup();
        RA_ASSERT(false && "Failed to initialize GLAD!");
    }

    LOG_INFO("Window created ({}x{})", m_window_settings.size.x, m_window_settings.size.y);
}

void Window::glfw_swap_buffers() const noexcept
{
    glfwSwapBuffers(m_window);
}

void Window::update_window_size(int width, int height) noexcept 
{
    m_window_settings.size.x = width;
    m_window_settings.size.y = height;
}

void Window::framebuffer_resize_callback(
        GLFWwindow* window, 
        int width, 
        int height) 
{
    if (auto* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        user_window->update_window_size(width, height);
        glViewport(0, 0, width, height);
    }
}

void Window::glfw_error_callback(int error, const char* description) 
{
    LOG_ERROR("GLFW Error {}: {}", error, description);
}



} // RoboTact::Platform

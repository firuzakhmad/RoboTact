#ifndef SDL_WINDOW_HPP
#define SDL_WINDOW_HPP

#include "window_settings.hpp"
#include "i_window.hpp"

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <string_view>

namespace RoboTact::Core
{

/* 
 * @class SDLWindow
 * @brief RAII wrapper for SDL window management
 * 
 * Handles window creation, lifecycle and OpenGL context management
*/	
class SDLWindow final : public IWindow
{
public:
	/**
	 * @brief Construct a new Window object
	 * @param window_settings Configuration for the window
	 * @throws std::runtime_error using custom RA_ASSERT if window creation fails
	 */
    explicit SDLWindow(WindowSettings window_settings = {});

    ~SDLWindow() override;

    void initialize();
    void shutdown() noexcept;

    void update_window_size(int width, int height) noexcept;

    [[nodiscard]] bool is_relative_mouse_mode_enabled() const noexcept;

    [[nodiscard]] bool is_capture_mouse_enabled() const noexcept;
    
    void set_relative_mouse_mode_enabled(bool relative_mouse_mode_enabled);

    void set_capture_mouse_enabled(bool capture_mouse_enabled);
    
    /**
     * @copydoc IWindow::poll_events
     */
    void poll_events() override;

    /**
     * @copydoc IWindow::swap_buffers
     */
    void swap_buffers() const noexcept override;

    /**
     * @copydoc IWindow::should_close
     */
    [[nodiscard]] bool should_close() const noexcept override;

private:
	SDL_Window* m_window				{nullptr};
	SDL_GLContext m_gl_context			{nullptr};

	bool m_relative_mouse_mode_enabled	{false};
    bool m_capture_mouse_enabled		{false};
    bool m_should_close					{false};
};

} // namespace RoboTact::Core
#endif // SDL_WINDOW_HPP
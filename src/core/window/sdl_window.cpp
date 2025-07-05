#include "sdl_window.hpp"
#include "core/utils/assert/assert.hpp"

#include <utility>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace RoboTact::Core
{

SDLWindow::SDLWindow(WindowSettings window_settings)
    : IWindow(std::move(window_settings))
{
    initialize();
}

SDLWindow::~SDLWindow() { shutdown(); }

void SDLWindow::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0) 
	{
        RA_ASSERT(true, "Failed to initialize SDL!");
        return;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_window_settings.gl_major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_window_settings.gl_minor_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    Uint32 window_flags = SDL_WINDOW_OPENGL;
    if (m_window_settings.resizable)
    {
        window_flags |= SDL_WINDOW_RESIZABLE;
    }
    if (m_window_settings.fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }

    m_window = SDL_CreateWindow(
        m_window_settings.title.data(),
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        m_window_settings.size.x,
        m_window_settings.size.y,
        window_flags
    );

    RA_ASSERT(m_window, "Failed to create SDL window");

    SDL_GL_GetDrawableSize(
    		m_window, 
    		reinterpret_cast<int *>(&m_window_settings.size.x), 
    		reinterpret_cast<int *>(&m_window_settings.size.y));
    if (m_relative_mouse_mode_enabled) 
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
    if (m_capture_mouse_enabled) 
    {
        SDL_CaptureMouse(SDL_TRUE);
    } else {
        SDL_CaptureMouse(SDL_FALSE);
    }

    m_gl_context = SDL_GL_CreateContext(m_window);
    RA_ASSERT(m_gl_context, "Failed to create OpenGL context");
    
    SDL_GL_MakeCurrent(m_window, m_gl_context);
    SDL_GL_SetSwapInterval(m_window_settings.v_sync ? 1 : 0);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) 
    {
        shutdown();
        LOG_ERROR("Failed to initialize GLAD!");
    }

    LOG_INFO(
    	"Window created (", 
    	m_window_settings.size.x, 
    	"x", 
    	m_window_settings.size.y, 
    	")");

    // callback functions 
    m_on_exit = [&]() { m_should_close = true; };
    m_on_key_down = [&](int key) { if (key == SDLK_ESCAPE) { m_should_close = true; }};
    m_on_late_keys_down = [&](const uint8_t *keys) {};
    m_on_mouse_move = [&](int x, int y, int x_rel, int y_rel) {};
    m_on_mouse_down = [&](int button, int x, int y) {};
}

void SDLWindow::shutdown() noexcept
{
	if (m_gl_context)
	{
		SDL_GL_DeleteContext(m_gl_context);
        m_gl_context = nullptr;
	}

	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

    SDL_Quit();
}

void SDLWindow::update_window_size(int width, int height) noexcept
{
	m_window_settings.size.x = width;
    m_window_settings.size.y = height;
    glViewport(0, 0, width, height);
}

bool SDLWindow::is_relative_mouse_mode_enabled() const noexcept
{
	return m_relative_mouse_mode_enabled;
}

bool SDLWindow::is_capture_mouse_enabled() const noexcept
{
	return m_capture_mouse_enabled;
}

void SDLWindow::set_relative_mouse_mode_enabled(bool relative_mouse_mode_enabled)
{
	m_relative_mouse_mode_enabled = relative_mouse_mode_enabled;
    if (m_relative_mouse_mode_enabled) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

void SDLWindow::set_capture_mouse_enabled(bool capture_mouse_enabled)
{
	m_capture_mouse_enabled = capture_mouse_enabled;
    if (m_capture_mouse_enabled) {
        SDL_CaptureMouse(SDL_TRUE);
    } else {
        SDL_CaptureMouse(SDL_FALSE);
    }
}

void SDLWindow::poll_events()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (ImGui::GetCurrentContext()) 
		{
    		ImGui_ImplSDL2_ProcessEvent(&event);
		}

		switch(event.type)
		{
			case SDL_QUIT:
            	m_on_exit();
            	break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    update_window_size(event.window.data1, event.window.data2);
                }
                break;
            case SDL_KEYDOWN:
            	m_on_key_down(event.key.keysym.sym);
            	break;
            case SDL_MOUSEMOTION:
            	m_on_mouse_move(event.motion.x, 
            					event.motion.y, 
            					event.motion.xrel, 
            					event.motion.yrel);
            	break;
            case SDL_MOUSEBUTTONDOWN:
            	m_on_mouse_down(event.button.button, 
            					event.button.x, 
            					event.button.y);
            	break;
            default:
        		m_on_late_keys_down(SDL_GetKeyboardState(nullptr));
        		break;

		}
	}
}

void SDLWindow::swap_buffers() const noexcept
{
	SDL_GL_SwapWindow(m_window);
}

bool SDLWindow::should_close() const noexcept
{
	return m_should_close;
}


} // namespace RoboTact::Core
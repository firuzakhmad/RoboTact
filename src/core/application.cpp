#include "application.hpp"
#include "logger.hpp"
#include "timer.hpp"
#include "../state/main_menu_state.hpp"

namespace RoboTact
{
	Application::Application()
	{
		LOG_INFO("Application initialized.");
        m_window = std::make_shared<Platform::Window>();
        m_renderer = std::make_shared<Renderer>(*m_window);
        m_app_state_manager = std::make_unique<AppStateManager>(m_renderer);
	}

	Application::~Application()
	{
	}	
	

	void Application::run()
	{
		auto& timer = Timer::get_instance();
        timer.reset();

        m_app_state_manager->push_state(std::make_unique<MainMenuState>(m_renderer));

		while(!m_window->should_close())
		{
			timer.update();
			double delta_time = timer.get_delta_time();

			m_window->poll_events();

            double fixed_timestep = 1.0 / 60.0;
            while (timer.get_accumulated_time() >= fixed_timestep) {
                m_app_state_manager->update(delta_time);
                
                
                timer.consume_accumulated_time(fixed_timestep);
            }

            m_app_state_manager->render();

            m_window->glfw_swap_buffers();
		}
	}
}
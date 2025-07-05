#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "core/window/sdl_window.hpp"
#include "core/utils/thread/thread_manager.hpp"

#include <memory>

namespace RoboTact
{
	class Application
	{
	public:
		Application();
		~Application();
		
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();
		void request_stop();
		void initialize_services();

	private:
		void main_loop();
		void simulation_loop();
		void io_loop();
		bool should_continue() const noexcept;

		std::unique_ptr<Core::SDLWindow> m_window;
	};
} // namespace RoboTact

#endif // APPLICATION_HPP
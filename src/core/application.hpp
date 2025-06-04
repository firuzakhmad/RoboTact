#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "../platform/window.hpp"
#include "../graphics/renderer.hpp"
#include "../state/app_state_manager.hpp"

namespace RoboTact
{
	class Application
	{
	public:
		static constexpr int WIDTH  = 800;
    	static constexpr int HEIGHT = 600;

		Application();
		~Application();
		
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		std::shared_ptr<Platform::Window> m_window;
		std::shared_ptr<Renderer> m_renderer;
    	std::unique_ptr<AppStateManager> m_app_state_manager;

	};
} // namespace RoboTact

#endif // APPLICATION_HPP
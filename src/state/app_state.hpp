#ifndef APP_STATE_HPP
#define APP_STATE_HPP

#include "../graphics/renderer.hpp"

#include <memory>

namespace RoboTact
{
	class AppState
	{
	public:
		explicit AppState(std::shared_ptr<Renderer> renderer) 
		: m_renderer{renderer} {}
		virtual ~AppState() = default;

		virtual void on_enter() 							= 0;
		virtual void on_exit() 								= 0;
		virtual void update(double delta_time) 				= 0;
		virtual void render()								= 0;
		virtual void render_gui()							= 0;
	protected:
		std::shared_ptr<Renderer> m_renderer;
	};

} // namespace RoboTact

#endif // APP_STATE_HPP			
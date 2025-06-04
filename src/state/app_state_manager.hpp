#ifndef APP_STATE_MANAGER
#define APP_STATE_MANAGER

#include "app_state.hpp"

#include <memory>
#include <stack>

namespace RoboTact
{
	class AppStateManager
	{
	public:
		explicit AppStateManager(std::shared_ptr<Renderer> renderer)
			: m_renderer{renderer} {}
		~AppStateManager() = default;

		AppStateManager(const AppStateManager&) = delete;
		AppStateManager& operator=(const AppStateManager&) = delete;

		void push_state(std::unique_ptr<AppState> state);
		void pop_state();
		void change_state(std::unique_ptr<AppState> state);

		void update(double delta_time);
		void render();
		[[nodiscard]] bool is_empty() const { return m_state_stack.empty(); }
	protected:
		std::stack<std::unique_ptr<AppState>> m_state_stack;
		std::shared_ptr<Renderer> m_renderer;	
	};
} // namespace RoboTact

#endif // APP_STATE_MANAGER
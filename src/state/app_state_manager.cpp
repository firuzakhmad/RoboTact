#include "app_state_manager.hpp"
#include "../core/assert.hpp"

namespace RoboTact
{
	void AppStateManager::push_state(std::unique_ptr<AppState> state)
	{
		RA_ASSERT(state && "AppStateManager::push_state: Null state provided.");

		if (!m_state_stack.empty())
		{
			m_state_stack.top()->on_exit();
		}

		m_state_stack.push(std::move(state));
		m_state_stack.top()->on_enter();
	}

	void AppStateManager::pop_state()
	{
		RA_ASSERT(!m_state_stack.empty() && "AppStateManager::pop_state: No states to pop.");

		if (!m_state_stack.empty())
		{
			m_state_stack.top()->on_exit();
			m_state_stack.pop();
		}

		if (!m_state_stack.empty())
		{
			m_state_stack.top()->on_enter();
		}
	}

	void AppStateManager::change_state(std::unique_ptr<AppState> state)
	{
		RA_ASSERT(state && "AppStateManager::change_state: Null state provided.");

		// Cleanup the current state
	    if (!m_state_stack.empty()) {
	        m_state_stack.top()->on_exit();
	        m_state_stack.pop();
	    }

	    m_state_stack.push(std::move(state));
	    m_state_stack.top()->on_enter();
	}

	void AppStateManager::update(double delta_time)
	{
		if (!m_state_stack.empty())
		{
			m_state_stack.top()->update(delta_time);
		}
	}

	void AppStateManager::render()
	{
		if (!m_state_stack.empty())
		{
			m_state_stack.top()->render();
		}
	}
} // namespace RoboTact
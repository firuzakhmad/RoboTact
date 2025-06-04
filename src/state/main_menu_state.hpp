#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP

#include "app_state.hpp"
#include "../graphics/renderer.hpp"

#include "../controllers/robot_controller.hpp"
#include "../config/imgui_manager.hpp"
#include "../ui/ui_manager.hpp"

#include <iostream>

namespace RoboTact
{
	class MainMenuState : public AppState
	{
	public:
		explicit MainMenuState(std::shared_ptr<Renderer> renderer);

		void on_enter() override;
		void on_exit() override;
		void update(double delta_time) override;
		void render() override;
		void render_gui() override;
	private:
	    std::shared_ptr<Renderer> m_renderer					{nullptr};
	    std::shared_ptr<RobotController> m_controller			{nullptr};
	    
	    std::unique_ptr<Config::ImGuiManager> m_imgui_manager	{nullptr};
	    std::unique_ptr<UI::UIManager> m_ui_manager 			{nullptr};
	};
} // namespace RoboTact

#endif // MAIN_MENU_STATE_HPP
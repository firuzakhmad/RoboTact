#include "main_menu_state.hpp"


#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace RoboTact
{

MainMenuState::MainMenuState(
	std::shared_ptr<Renderer> renderer)
		:	AppState(renderer), m_renderer{renderer}
{	
	m_controller = std::make_shared<RobotController>();
    
    m_imgui_manager	= std::make_unique<Config::ImGuiManager>(m_renderer->get_window());
    m_ui_manager = std::make_unique<UI::UIManager>(m_controller);
}

void MainMenuState::on_enter() 
{
	std::cout << "Entering Main Menu State" << std::endl;
}

void MainMenuState::on_exit() 
{
	std::cout << "Exiting Main Menu State" << std::endl;
}

void MainMenuState::update(double delta_time) 
{	}

void MainMenuState::render() 
{
	m_renderer->render();
	render_gui();
}	

void MainMenuState::render_gui() 
{
	m_imgui_manager->begin_frame();
    m_ui_manager->render();
    m_imgui_manager->end_frame();
}
} // namespace RoboTact
#include "ui_manager.hpp"
#include "control_panel.hpp"
#include "../core/logger.hpp"

#include "../serial_ports/serial_ports.hpp"

namespace RoboTact::UI {

UIManager::UIManager(std::shared_ptr<RobotController> controller)
    : m_controller(std::move(controller)) 
{
	initialize();
	auto ports = SerialPort::list_serial_ports();
	for (const auto& port : ports)
	{
		std::cout << port.device_path << std::endl;
	}

}

UIManager::~UIManager() {
    LOG_INFO("UI Manager shutdown");
}

void UIManager::initialize() 
{
    // Register default panels
    register_panel<ControlPanel>(m_controller);
    LOG_INFO("UI Manager initialized");
}

void UIManager::render() {
    render_main_menu();

    // Render all registered panels
    for (auto& panel : m_panels) {
        if (panel->is_visible()) {
            panel->render();
        }
    }
}


void UIManager::render_main_menu() {



    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("View")) {
            for (auto& panel : m_panels) {
                bool visible = panel->is_visible();
                if (ImGui::MenuItem(panel->get_name().c_str(), nullptr, &visible)) {
                    panel->set_visible(visible);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Robot")) {
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("New")) {
            
            ImGui::EndMenu();
        }

        // Push remaining space to force right alignment
	    ImGui::SameLine(ImGui::GetWindowWidth() - 108); // Adjust 100 based on your needs
        

	    // Right-aligned items
	    if (ImGui::BeginMenu("Connection    ")) 
	    {
	        if (ImGui::MenuItem("Connect")) {
                // m_controller->connect();
            }
            if (ImGui::MenuItem("Disconnect")) {
                // m_controller->disconnect();
            }

	        ImGui::EndMenu();
	    }

	    // Calculating perfect alignment
		float menu_text_height = ImGui::GetTextLineHeight();
		ImGui::SameLine(ImGui::GetWindowWidth() - 35);

		// Adjusting vertical position to match menu text
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (menu_text_height - ImGui::GetTextLineHeight() + 1));

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0,0,1));
		ImGui::Bullet();
		ImGui::PopStyleColor();

        ImGui::EndMainMenuBar();

    }
}

Panel* UIManager::get_panel(const std::string& name) {
    auto it = std::find_if(m_panels.begin(), m_panels.end(),
        [&name](const auto& panel) { return panel->get_name() == name; });
    
    return (it != m_panels.end()) ? it->get() : nullptr;
}

} // namespace RoboTact::UI
#include "control_panel.hpp"
#include "imgui.h"
#include "../core/logger.hpp"

namespace RoboTact::UI {

ControlPanel::ControlPanel(std::shared_ptr<RobotController> controller)
    : Panel("Robot Controls"), m_controller(std::move(controller)) {}

void ControlPanel::render() {
    if (!m_visible) return;

    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(m_name.c_str(), &m_visible)) {
        apply_default_style();
        
        render_movement_controls();
        ImGui::Separator();
        render_operation_controls();
        ImGui::Separator();
        render_status_display();
        ImGui::Separator();
        render_emergency_stop();
        
        ImGui::End();
    }
}

void ControlPanel::render_movement_controls() {
    ImGui::Text("Movement Controls");
    
    if (ImGui::Button("Forward")) {
        // m_controller->move_forward();
    }
    // Add more movement controls...
}

void ControlPanel::render_operation_controls() {
    ImGui::Text("Arm Controls");
    // Add arm operation controls...
}

void ControlPanel::render_status_display() {
    ImGui::Text("Robot Status");
    // ImGui::Text("Battery: %.1f%%", m_controller->get_battery_level());
    // Add more status indicators...
}

void ControlPanel::render_emergency_stop() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    
    if (ImGui::Button("EMERGENCY STOP")) {
        LOG_ERROR("Emergency stop activated");
        // m_controller->emergency_stop();
    }
    
    ImGui::PopStyleColor(2);
}

} // namespace RoboTact::UI
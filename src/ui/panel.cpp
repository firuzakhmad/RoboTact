#include "panel.hpp"
#include "imgui.h"

namespace RoboTact::UI {

Panel::Panel(std::string name) 
    : m_name(std::move(name)) {}

void Panel::apply_default_style() const {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

    ImGui::PopStyleVar(2);
}

} // namespace RoboTact::UI
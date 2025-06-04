#ifndef CONTROL_PANEL_HPP
#define CONTROL_PANEL_HPP

#include "panel.hpp"
#include "../controllers/robot_controller.hpp"

namespace RoboTact::UI {

/**
 * @brief Main robot control panel with movement and operation controls
 */
class ControlPanel : public Panel {
public:
    explicit ControlPanel(std::shared_ptr<RobotController> controller);
    
    void render() override;

private:
    void render_movement_controls();
    void render_operation_controls();
    void render_status_display();
    void render_emergency_stop();

    std::shared_ptr<RobotController> m_controller;
};

} // namespace RoboTact::UI

#endif // CONTROL_PANEL_HPP
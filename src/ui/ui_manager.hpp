#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "panel.hpp"
#include "../controllers/robot_controller.hpp"
#include "../config/imgui_manager.hpp"
#include "../core/logger.hpp"

#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace RoboTact::UI {

/**
 * @brief Central manager for all UI panels and layout
 */
class UIManager {
public:
    explicit UIManager(std::shared_ptr<RobotController> controller);
    ~UIManager();

    void initialize();
    void render();

    template<typename T, typename... Args>
    void register_panel(Args&&... args) {
        static_assert(std::is_base_of_v<Panel, T>, 
                     "Registered panel must derive from Panel");
        m_panels.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        LOG_INFO("Registered panel: {}", typeid(T).name());
    }

    Panel* get_panel(const std::string& name);

private:
    void render_main_menu();

    std::shared_ptr<RobotController> m_controller;
    std::vector<std::unique_ptr<Panel>> m_panels;
};

} // namespace RoboTact::UI

#endif // UI_MANAGER_HPP
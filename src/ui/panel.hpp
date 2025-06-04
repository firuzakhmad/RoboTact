#ifndef PANEL_HPP
#define PANEL_HPP

#include <string>
#include <functional>

namespace RoboTact::UI {

/**
 * @brief Base class for all UI panels
 * 
 * Provides common functionality for ImGui panels with visibility control
 */
class Panel {
public:
    explicit Panel(std::string name);
    virtual ~Panel() = default;

    Panel(const Panel&) = delete;
    Panel& operator=(const Panel&) = delete;
    
    virtual void render() = 0;
    
    [[nodiscard]] const std::string& get_name() const noexcept { return m_name; }
    [[nodiscard]] bool is_visible() const noexcept { return m_visible; }
    void set_visible(bool visible) noexcept { m_visible = visible; }

protected:
    std::string m_name;
    bool m_visible{true};
    
    // Helper for consistent panel styling
    void apply_default_style() const;
};

} // namespace RoboTact::UI

#endif // PANEL_HPP
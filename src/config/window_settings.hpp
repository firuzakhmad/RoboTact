#ifndef WINDOW_SETTINGS_HPP
#define WINDOW_SETTINGS_HPP

#include <glm/glm.hpp>
#include <string>

namespace RoboTact::Config {

/**
 * @brief Configuration settings for window creation
 * 
 * Provides default values for window creation parameters with validation
 */
struct WindowSettings {
    std::string title       {"RoboTact"};
    glm::ivec2 size         {1280, 720};
    bool v_sync             {true};
    bool resizable          {true};
    bool fullscreen         {false};
    int gl_major_version    {3};
    int gl_minor_version    {3};

    /**
     * @brief Validates the window settings
     * @throws std::invalid_argument if settings are invalid
     */
    void validate() const {
        if (size.x <= 0 || size.y <= 0) {
            throw std::invalid_argument("Window size must be positive");
        }
        if (gl_major_version < 3 || (gl_major_version == 3 && gl_minor_version < 3)) {
            throw std::invalid_argument("OpenGL 3.3 or higher is required");
        }
    }
};

} // namespace RoboTact::Config

#endif // WINDOW_SETTINGS_HPP
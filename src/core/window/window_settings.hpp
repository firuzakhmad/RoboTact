#ifndef WINDOW_SETTINGS_HPP
#define WINDOW_SETTINGS_HPP

#include <glm/glm.hpp>

#include <string_view>
#include <stdexcept>

namespace RoboTact::Core 
{

/**
 * @struct WindowSettings
 * @brief Comprehensive configuration for window creation
 * 
 * Contains all parameters needed to create and configure a window,
 * with built-in validation for parameter sanity.
 */
struct WindowSettings 
{
    std::string_view title	{"RoboTact"};
    glm::uvec2 size 		{1280, 720};  
    bool v_sync				{true};
    bool resizable			{true};
    bool fullscreen			{false};
    bool decorated			{true};  
    bool visible			{true};    
    bool focused			{true};    
    int gl_major_version	{3};
    int gl_minor_version	{3};
    int msaa_samples		{4};   // Multisample anti-aliasing samples
    float opacity 			{1.0f};   // Window opacity (0.0 to 1.0)

    // Vulkan-specific settings could be added here 

    /**
     * @brief Validates all window settings
     * @throws std::invalid_argument if any setting is invalid
     */
    void validate() const 
    {
        if (title.empty()) 
        {
            throw std::invalid_argument("Window title cannot be empty");
        }
        if (size.x == 0 || size.y == 0) 
        {
            throw std::invalid_argument("Window size cannot be zero");
        }
        if (gl_major_version < 3 || (gl_major_version == 3 && gl_minor_version < 3)) 
        {
            throw std::invalid_argument("OpenGL 3.3 or higher is required");
        }
        if (msaa_samples < 0 || msaa_samples > 16) 
        {
            throw std::invalid_argument("MSAA samples must be between 0 and 16");
        }
        if (opacity < 0.0f || opacity > 1.0f) 
        {
            throw std::invalid_argument("Opacity must be between 0.0 and 1.0");
        }
    }
};

} // namespace RoboTact::Core

#endif // WINDOW_SETTINGS_HPP
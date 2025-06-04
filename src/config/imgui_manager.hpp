#ifndef IMGUI_MANAGER_HPP
#define IMGUI_MANAGER_HPP

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>

namespace RoboTact::Platform {
class Window;
}

namespace RoboTact::Config {

/*
 * @brief Manages ImGui context and rendering lifecycle
 * 
 * Handles initialization frame management, and rendering of ImGui
*/
class ImGuiManager {
public:
	/*
	 * @brief Construct a new ImGui Manager object
	 * @param window The window to attach ImGui to
	 */
	explicit ImGuiManager(Platform::Window& window);
	~ImGuiManager();

	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager(ImGuiManager&&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;
	ImGuiManager& operator=(ImGuiManager&&) = delete;

    void enable_dockspace();
    void begin_frame();
    void end_frame();

    static void set_dark_theme();
    void load_font(const std::string& path, float size_px);

private:
	void initialize();
    void shutdown() noexcept;

	Platform::Window& m_window;
    bool m_initialized{false};
};
} // namespace RoboTact::Config

#endif // IMGUI_MANAGER_HPP
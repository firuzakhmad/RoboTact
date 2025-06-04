#include "imgui_manager.hpp"
#include "../core/logger.hpp"
#include "../core/assert.hpp"
#include "../platform/window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace RoboTact::Config
{

ImGuiManager::ImGuiManager(Platform::Window& window) 
	: m_window{window} 
{
	initialize();
}

ImGuiManager::~ImGuiManager() { shutdown(); }

void ImGuiManager::initialize()
{
	if (m_initialized) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    set_dark_theme();

    if (!ImGui_ImplGlfw_InitForOpenGL(m_window.get_glfw_window(), true))
    {
    	LOG_ERROR("Failed to initialize ImGui GLFW backend");
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        ImGui_ImplGlfw_Shutdown();
        LOG_ERROR("Failed to initialize ImGui OpenGL backend");
    }

    load_font("../../assets/fonts/roboto/Roboto-Regular.ttf", 18.0f);

    m_initialized = true;
    LOG_INFO("ImGui initialized successfully");
}

void ImGuiManager::shutdown() noexcept
{
	if (!m_initialized) return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_initialized = false;
    LOG_INFO("ImGui shutdown completed");
}

void ImGuiManager::enable_dockspace()
{
	// Retrieve the main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Create a full-screen window to host the dockspace
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_NoCollapse |
                                         ImGuiWindowFlags_NoResize | 
                                         ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoBringToFrontOnFocus | 
                                         ImGuiWindowFlags_NoNavFocus |
                                         ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace Host", nullptr, host_window_flags);
    ImGui::PopStyleVar(3);

    // Create the dockspace
    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
}

void ImGuiManager::begin_frame()
{
	if (!m_initialized) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    enable_dockspace();
}

void ImGuiManager::end_frame()
{
	if (!m_initialized) return;

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
    {
        GLFWwindow* backup_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_context);
    }
}

void ImGuiManager::set_dark_theme()
{

}

void ImGuiManager::load_font(const std::string& path, float size_px)
{
	ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(path.c_str(), size_px);
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

} // namespace RoboTact::Config
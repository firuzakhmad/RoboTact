#include "renderer.hpp"

#include <glad/glad.h>

namespace RoboTact
{
	Renderer::Renderer(Platform::Window& window)
		: m_window(window) {}
	Renderer::~Renderer(){}

	void Renderer::render()
	{
	    const auto size = m_window.get_size();
	    glViewport(0, 0, size.x, size.y);
	    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT);
	}

} // namespace RoboTact
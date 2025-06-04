#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "../platform/window.hpp"

#include <memory>

namespace RoboTact
{
class Renderer
{
public:
	Renderer(Platform::Window& window);
	~Renderer();

	[[nodiscard]] Platform::Window& get_window() const noexcept { return m_window; }

	void render();
private:
	Platform::Window& m_window;
};
} // namespace RoboTact

#endif // RENDERER_HPP
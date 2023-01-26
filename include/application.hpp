#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include <SDL.h>
#include <memory>
#include <cstdint>
#include "renderer.hpp"

namespace TRIAD_6
{
	struct WindowDeleter
	{
		void operator()(SDL_Window *window)
		{
			SDL_DestroyWindow(window);
		}
	};

	using Window = std::unique_ptr<SDL_Window, WindowDeleter>;

	class Application
	{
		public:
			Application();
			~Application();
		private:
			Window MainWindow;
			Renderer MainRenderer;
	};
}

#endif

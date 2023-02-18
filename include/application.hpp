#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include <SDL.h>
#include <memory>
#include <cstdint>
#include "renderer.hpp"
#include "machine.hpp"

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
			void RunMainLoop();
			
			inline int GetReturnCode() const
			{
				return retcode;
			}
			
			inline bool Fail() const
			{
				return fail;
			}
		private:
			int retcode;
			bool fail;
			Window MainWindow;
			Renderer MainRenderer;
			Machine CurrentMachine;
	};

	consteval uint32_t GetDefaultWindowFlags()
	{
		uint32_t flags = 0x00000000;
		if constexpr (CheckCompatibleRendererType<renderer_type, RendererType::OpenGL_21, RendererType::OpenGL_30, RendererType::OpenGLES_2, RendererType::OpenGLES_3>())
		{
			flags |= SDL_WINDOW_OPENGL;
		}
		else if constexpr (CheckCompatibleRendererType<renderer_type, RendererType::Vulkan>())
		{
			flags |= SDL_WINDOW_VULKAN;
		}
		return flags;
	}
}

#endif

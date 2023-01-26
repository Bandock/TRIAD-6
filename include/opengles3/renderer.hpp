#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <GLES3/gl3.h>
#include <SDL.h>
#include <vector>

namespace TRIAD_6
{
	class Renderer
	{
		public:
			Renderer();
			~Renderer();
			bool Setup(SDL_Window *window);
	};
}

#endif

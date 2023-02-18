#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <GLES3/gl3.h>
#include <SDL.h>
#include <cstdint>
#include <array>
#include <vector>
#include "renderer_type.hpp"

namespace TRIAD_6
{
	struct VertexData
	{
		std::array<float, 2> pos;
		std::array<float, 2> tex;
	};

	struct ColorData
	{
		int8_t r;
		int8_t g;
		int8_t b;
		int8_t a; // Unused
	};

	const RendererType renderer_type = RendererType::OpenGLES_3;

	class Renderer
	{
		public:
			Renderer();
			~Renderer();
			bool Setup(SDL_Window &Window);
			void Render(SDL_Window &Window);
			const char *GetRendererError() const;
		private:
			const char *renderer_error_str;
			SDL_GLContext GLContext;
			GLuint PrimaryVertexShaderId, MachineFragmentShaderId;
			GLuint MainProgramId;
			GLuint VAOId;
			GLuint VBOId;
			GLuint IBOId;
			GLuint DisplayTextureId;
			std::array<VertexData, 4> vertices;
			std::array<uint8_t, 6> indices;
			std::vector<ColorData> display_buffer;

			bool CompileShader(GLuint &shader, GLuint shader_type, const char *shader_code);
			bool LinkProgram(GLuint &program, std::vector<GLuint> shader_list);
	};
}

#endif

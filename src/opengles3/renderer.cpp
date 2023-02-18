#include "renderer.hpp"
#include "renderer_error.hpp"
#include "shaders.hpp"
#include <memory>
#include <fmt/core.h>

TRIAD_6::Renderer::Renderer() : renderer_error_str(TRIAD_6::RendererError::NoError), GLContext(nullptr), PrimaryVertexShaderId(0), MachineFragmentShaderId(0), MainProgramId(0), VAOId(0), VBOId(0), IBOId(0), DisplayTextureId(0), display_buffer(315 * 195, { -4, -4, -4 })
{
	vertices = {
		VertexData { { -1.0f, 1.0f }, { 0.0f, 1.0f } },
		VertexData { { 1.0f, 1.0f }, { 1.0f, 1.0f } },
		VertexData { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
		VertexData { { 1.0f, -1.0f }, { 1.0f, 0.0f } }
	};
	indices = { 0, 2, 1, 2, 3, 1 };
}

TRIAD_6::Renderer::~Renderer()
{
	if (GLContext != nullptr)
	{
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (DisplayTextureId != 0)
		{
			glDeleteTextures(1, &DisplayTextureId);
		}
		if (IBOId != 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &IBOId);
		}
		if (VBOId != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &VBOId);
		}
		if (VAOId != 0)
		{
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &VAOId);
		}
		if (MainProgramId != 0)
		{
			glDetachShader(MainProgramId, PrimaryVertexShaderId);
			glDetachShader(MainProgramId, MachineFragmentShaderId);
			glDeleteProgram(MainProgramId);
		}
		if (PrimaryVertexShaderId != 0)
		{
			glDeleteShader(PrimaryVertexShaderId);
		}
		if (MachineFragmentShaderId != 0)
		{
			glDeleteShader(MachineFragmentShaderId);
		}
		SDL_GL_DeleteContext(GLContext);
	}
}

bool TRIAD_6::Renderer::Setup(SDL_Window &Window)
{
	GLContext = SDL_GL_CreateContext(&Window);
	if (GLContext == nullptr)
	{
		renderer_error_str = RendererError::GLContextCreationFailed;
		return false;
	}
	if (!CompileShader(PrimaryVertexShaderId, GL_VERTEX_SHADER, Shader::PrimaryVertexShader))
	{
		renderer_error_str = RendererError::PrimaryVertexShaderCompilationFailed;
		return false;
	}
	if (!CompileShader(MachineFragmentShaderId, GL_FRAGMENT_SHADER, Shader::MachineFragmentShader))
	{
		renderer_error_str = RendererError::MachineFragmentShaderCompilationFailed;
		return false;
	}
	std::vector<GLuint> shader_list;
	shader_list.push_back(PrimaryVertexShaderId);
	shader_list.push_back(MachineFragmentShaderId);
	if (!LinkProgram(MainProgramId, std::move(shader_list)))
	{
		renderer_error_str = RendererError::MainProgramLinkageFailed;
		return false;
	}
	glGenVertexArrays(1, &VAOId);
	glGenBuffers(1, &VBOId);
	glGenBuffers(1, &IBOId);
	glBindVertexArray(VAOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint8_t), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void *>(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void *>(sizeof(float) * 2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glGenTextures(1, &DisplayTextureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DisplayTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8I, 315, 195);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 315, 195, GL_RGBA_INTEGER, GL_BYTE, display_buffer.data());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, 630, 390);
	glUseProgram(MainProgramId);
	SDL_GL_SetSwapInterval(0);
	return true;
}

void TRIAD_6::Renderer::Render(SDL_Window &Window)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, reinterpret_cast<void *>(0));
	SDL_GL_SwapWindow(&Window);
}

const char *TRIAD_6::Renderer::GetRendererError() const
{
	return renderer_error_str;
}

bool TRIAD_6::Renderer::CompileShader(GLuint &shader, GLuint shader_type, const char *shader_code)
{
	if (shader != 0)
	{
		glDeleteShader(shader);
	}
	shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_code, nullptr);
	glCompileShader(shader);
	int compile_status = 0, info_log_len = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status == GL_FALSE)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);
		std::vector<char> info_log_str(info_log_len);
		glGetShaderInfoLog(shader, info_log_len, nullptr, info_log_str.data());
		fmt::print("{}\n", info_log_str.data());
		return false;
	}
	return true;
}

bool TRIAD_6::Renderer::LinkProgram(GLuint &program, std::vector<GLuint> shader_list)
{
	if (shader_list.size() == 0)
	{
		return false;
	}
	if (program != 0)
	{
		glDeleteProgram(program);
	}
	program = glCreateProgram();
	for (auto &i : shader_list)
	{
		glAttachShader(program, i);
	}
	glLinkProgram(program);
	int link_status = 0, info_log_len = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
		std::vector<char> info_log_str(info_log_len);
		glGetProgramInfoLog(program, info_log_len, nullptr, info_log_str.data());
		fmt::print("{}\n", info_log_str.data());
		return false;
	}
	return true;
}

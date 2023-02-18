#include "application.hpp"
#include <fmt/core.h>
#include <chrono>

TRIAD_6::Application::Application() : retcode(0), fail(false)
{
	SDL_Init(SDL_INIT_VIDEO);
	if constexpr (CheckCompatibleRendererType<renderer_type, RendererType::OpenGL_21, RendererType::OpenGL_30, RendererType::OpenGLES_2, RendererType::OpenGLES_3>())
	{
		if constexpr (renderer_type == RendererType::OpenGL_21)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		}
		else if constexpr (renderer_type == RendererType::OpenGL_30)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		}
		else if constexpr (renderer_type == RendererType::OpenGLES_2)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		}
		else if constexpr (renderer_type == RendererType::OpenGLES_3)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		}
		if constexpr (CheckCompatibleRendererType<renderer_type, RendererType::OpenGLES_2, RendererType::OpenGLES_3>())
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		}
	}
	else if constexpr (CheckCompatibleRendererType<renderer_type, RendererType::Direct3D_11, RendererType::Direct3D_12>())
	{
	}
	else if constexpr (CheckCompatibleRendererType<renderer_type, RendererType::Vulkan>())
	{
	}
	MainWindow = Window(SDL_CreateWindow("TRIAD-6", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 630, 390, GetDefaultWindowFlags()));
	if (!MainRenderer.Setup(*MainWindow))
	{
		fmt::print("Renderer Setup Error:  {}\n", MainRenderer.GetRendererError());
		fail = true;
		retcode = -1;
		return;
	}
}

TRIAD_6::Application::~Application()
{
	SDL_Quit();
}

void TRIAD_6::Application::RunMainLoop()
{
	bool exit = false;
	while (!exit)
	{
		std::chrono::high_resolution_clock::time_point current_tp = std::chrono::high_resolution_clock::now();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				{
					exit = true;
					break;
				}
			}
		}
		CurrentMachine(current_tp);
		MainRenderer.Render(*MainWindow);
	}
}

int main(int argc, char *argv[])
{
	TRIAD_6::Application MainApp;
	if (!MainApp.Fail())
	{
		MainApp.RunMainLoop();
	}
	return MainApp.GetReturnCode();
}

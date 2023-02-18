#ifndef _RENDERER_TYPE_HPP_
#define _RENDERER_TYPE_HPP_

#include <array>

namespace TRIAD_6
{
	enum class RendererType
	{
		OpenGL_21 = 0,
		OpenGL_30 = 1,
		OpenGLES_2 = 2,
		OpenGLES_3 = 3,
		Direct3D_11 = 4, // Target Direct3D 10 hardware using Shader Model 4.0
		Vulkan = 5, // Future Renderer
		Direct3D_12 = 6 // Future Renderer
	};

	template <RendererType Type, RendererType... Args>
	consteval bool CheckCompatibleRendererType()
	{
		std::array<RendererType, sizeof...(Args)> CompatibleTypes = { Args... };
		for (size_t i = 0; i < CompatibleTypes.size(); ++i)
		{
			if (Type == CompatibleTypes[i])
			{
				return true;
			}
		}
		return false;
	}
}

#endif

#ifndef _SHADERS_HPP_
#define _SHADERS_HPP_

namespace TRIAD_6
{
	namespace Shader
	{
		const char *PrimaryVertexShader = R"(#version 120
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

out vec2 outTex;

void main()
{
	gl_Position = vec4(pos, 0.0f, 1.0f);
	outTex = tex;
})";
		const char *MachineFragmentShader = R"(#version 120
#extension GL_ARB_explicit_attrib_location : require

in vec2 outTex;
out vec4 outColor;

uniform sampler2D DisplayTexture;

void main()
{
	vec4 colorData = texture2D(DisplayTexture, outTex);
	outColor = vec4(((colorData.r * 256.0f) / 8.0f), ((colorData.g * 256.0f) / 8.0f), ((colorData.b * 256.0f) / 8.0f), 1.0f);
})";
	}
}

#endif

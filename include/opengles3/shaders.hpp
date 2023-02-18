#ifndef _SHADERS_HPP_
#define _SHADERS_HPP_

namespace TRIAD_6
{
	namespace Shader
	{
		const char *PrimaryVertexShader = R"(#version 300 es

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

out vec2 outTex;

void main()
{
	gl_Position = vec4(pos, 0.0f, 1.0f);
	outTex = tex;
})";

		const char *MachineFragmentShader = R"(#version 300 es

precision highp float;
precision highp int;
precision highp isampler2D;

in vec2 outTex;
out vec4 outColor;

uniform isampler2D DisplayTexture;

void main()
{
	ivec2 texDim = textureSize(DisplayTexture, 0);
	ivec4 colorData = texelFetch(DisplayTexture, ivec2(int(outTex.x * float(texDim.x)), int(outTex.y * float(texDim.y))), 0);
	outColor = vec4((float(colorData.r) / 8.0f) + 0.5f, (float(colorData.g) / 8.0f) + 0.5f, (float(colorData.b) / 8.0f) + 0.5f, 1.0f);
})";
	}
}

#endif

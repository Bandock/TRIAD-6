#ifndef _RENDERER_ERROR_HPP_
#define _RENDERER_ERROR_HPP_

namespace TRIAD_6
{
	namespace RendererError
	{
		const char *NoError = "No Error.";
		const char *GLContextCreationFailed = "Unable to create GL Context.";
		const char *GLEWInitializationFailed = "GLEW initialization failed.";
		const char *ExplicitAttribLocationNotSupported = "Explicit attribute locations are not supported on this platform.";
		const char *TextureStorageNotSupported = "Texture storage functionality is not supported on this platform.";
		const char *PrimaryVertexShaderCompilationFailed = "Unable to compile Primary Vertex Shader.";
		const char *MachineFragmentShaderCompilationFailed = "Unable to compile Machine Fragment Shader.";
		const char *MainProgramLinkageFailed = "Unable to link Main Program.";
	}
}

#endif

// https://www.khronos.org/opengl/wiki/SPIR-V
// Read our shaders into the appropriate buffers
//std::vector<unsigned char> vertexSpirv = // Get SPIR-V for vertex shader.
//std::vector<unsigned char> fragmentSpirv = // Get SPIR-V for fragment shader.
//
//										   // Create an empty vertex shader handle
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//
//// Apply the vertex shader SPIR-V to the shader object.
//glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexSpirv.data(), vertexSpirv.size());
//
//// Specialize the vertex shader.
//std::string vsEntrypoint = ...; // Get VS entry point name
//glSpecializeShader(vertexShader, (const GLchar*)vsEntrypoint.c_str(), 0, nullptr, nullptr);
//
//// Specialization is equivalent to compilation.
//GLint isCompiled = 0;
//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
//if (isCompiled == GL_FALSE)
//{
//	GLint maxLength = 0;
//	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
//
//	// The maxLength includes the NULL character
//	std::vector<GLchar> infoLog(maxLength);
//	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
//
//	// We don't need the shader anymore.
//	glDeleteShader(vertexShader);
//
//	// Use the infoLog as you see fit.
//
//	// In this simple program, we'll just leave
//	return;
//}
//
//
//
//// Create an empty fragment shader handle
//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//
//// Apply the fragment shader SPIR-V to the shader object.
//glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentSpirv.data(), fragmentSpirv.size());
//
//// Specialize the fragment shader.
//std::string fsEntrypoint = ...; //Get VS entry point name
//glSpecializeShader(fragmentShader, (const GLchar*)fsEntrypoint.c_str(), 0, nullptr, nullptr);
//
//// Specialization is equivalent to compilation.
//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
//if (isCompiled == GL_FALSE)
//{
//	GLint maxLength = 0;
//	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
//
//	// The maxLength includes the NULL character
//	std::vector<GLchar> infoLog(maxLength);
//	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
//
//	// We don't need the shader anymore.
//	glDeleteShader(fragmentShader);
//	// Either of them. Don't leak shaders.
//	glDeleteShader(vertexShader);
//
//	// Use the infoLog as you see fit.
//
//	// In this simple program, we'll just leave
//	return;
//}
//
//// Vertex and fragment shaders are successfully compiled.
//// Now time to link them together into a program.
//// Get a program object.
//GLuint program = glCreateProgram();
//
//// Attach our shaders to our program
//glAttachShader(program, vertexShader);
//glAttachShader(program, fragmentShader);
//
//// Link our program
//glLinkProgram(program);
//
//// Note the different functions here: glGetProgram* instead of glGetShader*.
//GLint isLinked = 0;
//glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
//if (isLinked == GL_FALSE)
//{
//	GLint maxLength = 0;
//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
//
//	// The maxLength includes the NULL character
//	std::vector<GLchar> infoLog(maxLength);
//	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
//
//	// We don't need the program anymore.
//	glDeleteProgram(program);
//	// Don't leak shaders either.
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//	// Use the infoLog as you see fit.
//
//	// In this simple program, we'll just leave
//	return;
//}
//
//// Always detach shaders after a successful link.
//glDetachShader(program, vertexShader);
//glDetachShader(program, fragmentShader);
#include "RrShaderProgram.h"
#include "RrShaderMasterSubsystem.h"

#include "renderer/gpuw/ShaderPipeline.h"
#include "renderer/gpuw/Shader.h"

RrShaderProgram*
RrShaderProgram::Load ( const rrShaderProgramVsPs& params )
{

	return NULL;
}


RrShaderProgram::RrShaderProgram (
	const char* s_resourceId,
	gpu::Shader* vvs,
	gpu::Shader* hs,
	gpu::Shader* ds,
	gpu::Shader* gs,
	gpu::Shader* ps,
	gpu::Shader* cs
)
{
	if (vvs)
		m_pipeline.attach(vvs, "main");
	if (hs)
		m_pipeline.attach(hs, "main");
	if (ds)
		m_pipeline.attach(ds, "main");
	if (gs)
		m_pipeline.attach(gs, "main");
	if (ps)
		m_pipeline.attach(ps, "main");
	if (cs)
		m_pipeline.attach(cs, "main");
	m_pipeline.assemble();
}
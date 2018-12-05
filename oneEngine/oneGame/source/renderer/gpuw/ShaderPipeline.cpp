#include "renderer/gpuw/ShaderPipeline.h"
#include "renderer/gpuw/Shader.h"
#include "renderer/gpuw/Error.h"

#include "renderer/ogl/GLCommon.h"

#include <vector>


int gpu::ShaderPipeline::attach ( Shader* shader, const char* entrypoint_symbol )
{
	glSpecializeShader(shader->m_handle, entrypoint_symbol, 0, nullptr, nullptr);

	// Specialization is equivalent to compilation.
	GLint isCompiled = 0;
	glGetShaderiv(shader->m_handle, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader->m_handle, GL_INFO_LOG_LENGTH, &maxLength);
	
		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength); // TODO: remove this?????
		glGetShaderInfoLog(shader->m_handle, maxLength, &maxLength, &infoLog[0]);
	
		// We don't need the shader anymore.
		//glDeleteShader(shader->m_handle);
	
		// Use the infoLog as you see fit.
	
		// In this simple program, we'll just leave
		return;
	}
}

int gpu::ShaderPipeline::assemble ( void )
{
	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.
	GLuint program = glCreateProgram();
	
	// Attach our shaders to our program
	glAttachShader(program, m_vs);
	glAttachShader(program, m_ps);
	
	// Link our program
	glLinkProgram(program);
	
	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
	
		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
	
		// We don't need the program anymore.
		glDeleteProgram(program);

		// Don't leak shaders either.
		glDeleteShader(m_vs);
		glDeleteShader(m_ps);
	
		// Use the infoLog as you see fit.
	
		// In this simple program, we'll just leave
		return;
	}
}

int gpu::ShaderPipeline::destroy ( void )
{
}

bool gpu::ShaderPipeline::valid ( void )
{
	return false;
}
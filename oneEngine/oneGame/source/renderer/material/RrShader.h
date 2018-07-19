// When using RrShader objects, keep in mind that "delete RrShader" is NOT a valid statement!
// Because of how FUCKING ANNOYING THEY ARE, memory management of RrShader objects is handled AUTOMATICALLY.
// All RrShader objects MUST be created via new, and they must be 'destroyed' with ReleaseReference()!!

#ifndef _GL_SHADER_H_
#define _GL_SHADER_H_

// CG SHADER DEFINITIONS
// needs to have support for
/*
#include
#vert
#frag

needs to check at the top to see what kind of shader it is
#define _S_GLSL
#define _S_ASSEMBLY
#define _S_CG

*/

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/math/Math3d.h"

#include <string>
#include <unordered_map>

class RrShaderManager;

#include "RrShader.cbuffers.h"

class RrShader
{
public:
	RENDER_API RrShader ( const char* a_sShaderName, const renderer::rrShaderTag a_nShaderTag = renderer::SHADER_TAG_DEFAULT, const bool a_bCompileOnDemand = false );
	RENDER_API ~RrShader();
public:
	// Enumeration for the shader type
	enum eShaderType
	{
		None,
		/*Assembly,
		GLSL,
		HLSL,
		CG*/
		GLSL,
		Binary,
	};
	// Enumeration for compile error types
	enum eShaderCompileError
	{
		SCE_NOERROR		= 0,
		SCE_NOTCOMPILED = 1,
		SCE_NOFILE		= 2,
		SCE_VERTPARSE	= 4,
		SCE_FRAGPARSE	= 8,
		SCE_VERTCOMPILE	= 16,
		SCE_FRAGCOMPILE = 32,
		SCE_PROGRAM		= 64
	};
public:
	// Enable and disable shader for drawing
	void begin ( void );
	void end ( void );

	void prime ( void );
	bool isPrimed ( void );
public:
	// Public recompile
	bool recompile ( void );
	
	// Return the program
	unsigned int get_program ( void ) const
	{
		if ( !bIsReference )
			return iProgramID;
		else
			return pParentShader->get_program();
	}
	// get uniform block location
	int getUniformBlockLocation ( const char* name );

	// Memory management functions
	void GrabReference ( void );
	void ReleaseReference ( void );
	unsigned int ReferenceCount ( void );
	
	const char* GetFilename ( void ) const {
		return sShaderFilename.c_str();
	}
private:
	void AddReference ( void );
	void DecrementReference ( void );
	friend RrShaderManager;

protected:
	// System info
	std::string sShaderFilename;
	std::string sRawShader;
	renderer::rrShaderTag	stTag;

	// Shader properties
	char * sVertexShader;
	int iVertexShaderLength;
	char * sPixelShader;
	int iPixelShaderLength;

	// OpenGL shader ids
	unsigned int iVertexShaderID;
	unsigned int iPixelShaderID;
	// OpenGL program id
	unsigned int iProgramID;

	// Shader type
	eShaderType stShaderType;

	// Whether or not this shader is a reference
	bool bIsReference;
	// A pointer to the real shader
	RrShader* pParentShader;

	// Whether or not to compile it on start or on first use
	bool bCompileOnDemand;
	// If the shader has compiled
	bool bIsCompiled;

	bool bHasCompileError;
	unsigned int iCompileResult;

	// Has this shader been used before?
	bool	primed;

protected:
	// This loads the shader from the file and into the sRawShader string
	void open_shader ( void );
	// This takes the data in sRawShader and outputs proper shaders into the vertex and pixel shader strings
	void parse_shader ( void );
	// This compiles the shaders and adds them to a program object
	void compile_shader ( void );

	//	createConstantBufferBindings() : Internal post-compile step, grab engine refs.
	// Locates constant buffer bindings and links them to their index
	void createConstantBufferBindings ( void );

private:
	// Memory management
	unsigned int iRefNumber;
};

#endif
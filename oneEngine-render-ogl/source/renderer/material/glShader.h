// When using glShader objects, keep in mind that "delete glShader" is NOT a valid statement!
// Because of how FUCKING ANNOYING THEY ARE, memory management of glShader objects is handled AUTOMATICALLY.
// All glShader objects MUST be created via new, and they must be 'destroyed' with ReleaseReference()!!


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

// Includes
//#include "renderer/system/glMainSystem.h"
//#include "CTexture.h"
//#include "Color.h"
//#include "Vector4d.h"
//#include "Vector3d.h"
//#include "Vector2d.h"
//#include "Quaternion.h"
//#include "CMatrix.h"
#include "core/types/types.h"

//#include <fstream>
//using std::ifstream;
#include <string>
//#include <map>
//using std::map;
#include <unordered_map>

using std::string;


class glShaderManager;

namespace GLE
{
	enum shader_tag_t : uint8_t
	{
		SHADER_TAG_DEFAULT = 0,
		SHADER_TAG_SKINNING
	};
}

class glShader
{
public:
	RENDER_API glShader ( const string& a_sShaderName, const GLE::shader_tag_t a_nShaderTag = GLE::SHADER_TAG_DEFAULT, const bool a_bCompileOnDemand = false );
	RENDER_API ~glShader();
public:
	// Enumeration for the shader type
	enum eShaderType
	{
		None,
		Assembly,
		GLSL,
		HLSL,
		CG
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
	// Get Uniform location
	int	get_uniform_location ( const string & name );
	// Get Uniform Block location
	int get_uniform_block_location ( const string & name );
	// Get Vertex Attribute location
	int get_attrib_location ( const string & name );

	// Memory management functions
	//void AddReference ( void );
	//void DecrementReference ( void );
	void GrabReference ( void );
	void ReleaseReference ( void );
	unsigned int ReferenceCount ( void );
	
	const string& GetFilename ( void ) const {
		return sShaderFilename;
	}
private:
	void AddReference ( void );
	void DecrementReference ( void );
	friend glShaderManager;

protected:
	// System info
	string sShaderFilename;
	string sRawShader;
	GLE::shader_tag_t	stTag;

	// Shader properties
	//string sVertexShader;
	//string sPixelShader;
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
	glShader* pParentShader;

	// Whether or not to compile it on start or on first use
	bool bCompileOnDemand;
	// If the shader has compiled
	bool bIsCompiled;

	bool bHasCompileError;
	unsigned int iCompileResult;

	// Uniform map
	//map<string,int> mUniformMap;
	std::unordered_map<string,int> mUniformMap;

protected:
	// This loads the shader from the file and into the sRawShader string
	void open_shader ( void );
	// This takes the data in sRawShader and outputs proper shaders into the vertex and pixel shader strings
	void parse_shader ( void );
	// This compiles the shaders and adds them to a program object
	void compile_shader ( void );

	// Shader parsers
	void ParseShader_ASM ( void );
	void ParseShader_GLSL ( void );
	void ParseSplit ( void );
	void ParseDefines ( void );
	void ParseIncludes ( void );

private:
	// Memory management
	unsigned int iRefNumber;
};

#endif
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
#include "core/containers/arstring.h"

//#include <fstream>
//using std::ifstream;
#include <string>
//#include <map>
//using std::map;
#include <unordered_map>

class RrShaderManager;

namespace renderer
{
	enum shader_tag_t : uint8_t
	{
		SHADER_TAG_DEFAULT = 0,
		SHADER_TAG_SKINNING
	};
	enum attributeId_t : uint8_t
	{
		ATTRIB_VERTEX		= 0,
		ATTRIB_TEXCOORD0	= 1,
		ATTRIB_COLOR		= 2,
		ATTRIB_NORMAL		= 3,
		ATTRIB_TANGENTS		= 4,
		ATTRIB_BINORMALS	= 5,
		ATTRIB_BONEWEIGHTS	= 6,
		ATTRIB_BONEINDICES	= 7,
		ATTRIB_TEXCOORD2	= 8,
		ATTRIB_TEXCOORD3	= 9,
		ATTRIB_TEXCOORD4	= 10,
	};

	struct attributeReservedName_t
	{
		attributeId_t id;
		const char* token;
	};
	static struct attributeReservedName_t AttributeNames[] =
	{
		{ ATTRIB_VERTEX,	"mdl_Vertex" },
		{ ATTRIB_TEXCOORD0,	"mdl_TexCoord" },
		{ ATTRIB_COLOR,		"mdl_Color" },
		{ ATTRIB_NORMAL,	"mdl_Normal" },
		{ ATTRIB_TANGENTS,	"mdl_Tangents" },
		{ ATTRIB_BINORMALS,	"mdl_Binormals" },
		{ ATTRIB_BONEWEIGHTS, "mdl_BoneWeights" },
		{ ATTRIB_BONEINDICES, "mdl_BoneIndices" },
		{ ATTRIB_TEXCOORD2, "mdl_TexCoord2" },
		{ ATTRIB_TEXCOORD3, "mdl_TexCoord3" },
		{ ATTRIB_TEXCOORD4, "mdl_TexCoord4" },
	};

	enum uniformId_t : uint8_t
	{
		UNI_SURFACE_DIFFUSE_COLOR	= 0,
		UNI_SURFACE_EMISSIVE_COLOR	= 1,
		UNI_SURFACE_SPECULAR_COLOR	= 2,
		UNI_SURFACE_ALPHA_CUTOFF	= 3,
		UNI_SURFACE_LIGHTING_OVERRIDES	= 4,

		UNI_TEXTURE_SCALE			= 5,
		UNI_TEXTURE_OFFSET			= 6,

		UNI_GAME_WIND_DIRECTION		= 7,
		UNI_GAME_FADE_VALUE			= 8,
		UNI_GAME_HALF_SCALE			= 9,

		UNI_LIGHTING_AMBIENT		= 10,
		UNI_LIGHTING_COUNT			= 11,

		UNI_SAMPLER_0				= 20,
		UNI_SAMPLER_1				= 21,
		UNI_SAMPLER_2				= 22,
		UNI_SAMPLER_3				= 23,
		UNI_SAMPLER_4				= 24,
		UNI_SAMPLER_5				= 25,
		UNI_SAMPLER_6				= 26,
		UNI_SAMPLER_7				= 27,
		UNI_SAMPLER_8				= 28,
		UNI_SAMPLER_9				= 29,
		UNI_SAMPLER_10				= 30,
		UNI_SAMPLER_11				= 31,
		UNI_SAMPLER_SHADOW_0		= 32,
		UNI_SAMPLER_SHADOW_1		= 33,
		UNI_SAMPLER_SHADOW_2		= 34,
		UNI_SAMPLER_SHADOW_3		= 35,
		UNI_SAMPLER_SHADOW_4		= 36,
		UNI_SAMPLER_SHADOW_5		= 37,
		UNI_SAMPLER_SHADOW_6		= 38,
		UNI_SAMPLER_SHADOW_7		= 39,
		UNI_SAMPLER_SHADOW_8		= 40,
		UNI_SAMPLER_SHADOW_9		= 41,
		UNI_SAMPLER_SHADOW_10		= 42,
		UNI_SAMPLER_SHADOW_11		= 43,
		UNI_SAMPLER_LIGHT_BUFFER_0	= 44,
		UNI_SAMPLER_LIGHT_BUFFER_1	= 45,
		UNI_SAMPLER_INSTANCE_BUFFER_0	= 46,
		UNI_SAMPLER_INSTANCE_BUFFER_1	= 47,
		UNI_SAMPLER_REFLECTION_0	= 47,
		UNI_SAMPLER_REFLECTION_1	= 48,
		UNI_SAMPLER_REFLECTION_2	= 49,
		UNI_SAMPLER_REFLECTION_3	= 50,
	};
	struct uniformReservedName_t
	{
		uniformId_t id;
		const char* token;
	};
	static struct uniformReservedName_t UniformNames[] =
	{
		{ UNI_SURFACE_DIFFUSE_COLOR,	"sys_DiffuseColor" },
		{ UNI_SURFACE_EMISSIVE_COLOR,	"sys_EmissiveColor" },
		{ UNI_SURFACE_SPECULAR_COLOR,	"sys_SpecularColor" },
		{ UNI_SURFACE_ALPHA_CUTOFF,		"sys_AlphaCutoff" },
		{ UNI_SURFACE_LIGHTING_OVERRIDES,	"sys_LightingOverrides" },

		{ UNI_TEXTURE_SCALE,	"sys_TextureScale" },
		{ UNI_TEXTURE_OFFSET,	"sys_TextureOffset" },

		{ UNI_GAME_WIND_DIRECTION,	"gm_WindDirection" },
		{ UNI_GAME_FADE_VALUE,	"gm_FadeValue" },
		{ UNI_GAME_HALF_SCALE,	"gm_HalfScale" },

		{ UNI_LIGHTING_AMBIENT,	"sys_LightAmbient" },
		{ UNI_LIGHTING_COUNT,	"sys_LightNumber" },

		{ UNI_SAMPLER_0,		"textureSampler0" },
		{ UNI_SAMPLER_SHADOW_0,	"textureShadow0" },
		{ UNI_SAMPLER_LIGHT_BUFFER_0,	"textureLightBuffer" },
		{ UNI_SAMPLER_INSTANCE_BUFFER_0,"textureInstanceBuffer" },
		{ UNI_SAMPLER_REFLECTION_0,		"textureReflection0" },
	};
}

class RrShader
{
public:
	RENDER_API RrShader ( const char* a_sShaderName, const renderer::shader_tag_t a_nShaderTag = renderer::SHADER_TAG_DEFAULT, const bool a_bCompileOnDemand = false );
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
	int	get_uniform_location ( const char* name );
	// get uniform block location
	int get_uniform_block_location ( const char* name );
	// Get Vertex Attribute location
	//int get_attrib_location ( const char* name );

	// Memory management functions
	//void AddReference ( void );
	//void DecrementReference ( void );
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
	renderer::shader_tag_t	stTag;

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
	RrShader* pParentShader;

	// Whether or not to compile it on start or on first use
	bool bCompileOnDemand;
	// If the shader has compiled
	bool bIsCompiled;

	bool bHasCompileError;
	unsigned int iCompileResult;

	// Uniform map
	std::unordered_map<arstring<128>,int> mUniformMap;

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
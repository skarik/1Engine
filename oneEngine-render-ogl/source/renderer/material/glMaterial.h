
#ifndef _GL_MATERIAL_H_
#define _GL_MATERIAL_H_

#include "core/types/types.h"
#include "core/math/Color.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector4d.h"
#include "core/containers/arstring.h"

#include "renderer/types/types.h"

#include "glShader.h"
#include "glPass.h"
#include "glPass_Deferred.h"

#include <vector>

// Prototype
class CRenderableObject;
class CTexture;
class CRenderTexture;
class CRenderState;

class glMaterial
{
	friend CRenderState;
public:
	RENDER_API				glMaterial ( void );
	RENDER_API				~glMaterial ( void );
	
	// Load material from file
	RENDER_API void			loadFromFile ( const char* n_materialfile );
	// Check if file is material
	RENDER_API static bool	isValidFile ( const char* n_materialfile );
	// Duplicate material (starting with 1 reference)
	RENDER_API glMaterial*	copy ( void );

	// Current material active and bound
	RENDER_API static glMaterial*	current;
	RENDER_API static glMaterial*	Default;
	RENDER_API static glMaterial*	Copy;
	RENDER_API static glMaterial*	Fallback;
	// Current pass being bound
	RENDER_API static uchar		current_pass;
	// Current material count
	RENDER_API static uint			current_sampler_slot;

	// Current material special mode
	// The value of this will drastically change the rest of the program. These are hard-coded special effects.
	RENDER_API static uchar		special_mode;
public:
	RENDER_API void			bindPass ( uchar pass );
	RENDER_API void			bindPassAtrribs ( uchar pass );
	RENDER_API void			setShaderConstants ( CRenderableObject* source_object, const bool n_force_identity=false );

	RENDER_API void			bindPassForward ( uchar pass );
	RENDER_API void			bindPassDeferred( uchar pass );

	RENDER_API uchar		getPassCount ( void );
	RENDER_API uchar		getPassCountForward ( void );
	RENDER_API uchar		getPassCountDeferred ( void );

	//	Returns shader currently in use for the current pass
	RENDER_API glShader*	getUsingShader ( void );

	RENDER_API void			setUniform ( const char* sUniformName, float const fInput );
	RENDER_API void			setUniform ( const char* sUniformName, Vector2d const& vInput );
	RENDER_API void			setUniform ( const char* sUniformName, Vector3d const& vInput );
	RENDER_API void			setUniform ( const char* sUniformName, Color const& cInput );
	RENDER_API void			setUniform ( const char* sUniformName, Matrix4x4 const& matxInput );

	RENDER_API void			bindAttribute ( const char* sAttribName, const uint vec_size, const uint vec_type, const bool normalize, const int struct_size, const void* struct_offset ); 
	RENDER_API void			bindAttributeI ( const char* sAttribName, const uint vec_size, const uint vec_type, const int struct_size, const void* struct_offset ); 

private:
	void	shader_bind_world	 ( glShader* shader );

	void	shader_bind_samplers ( glShader* shader );
	void	shader_bind_constants( glShader* shader );
	void	shader_bind_lights	 ( glShader* shader );
	void	shader_bind_nolights ( glShader* shader );

	void	deferred_shader_build( uchar pass );
	void	shader_bind_deferred ( glShader* shader );

public:
	// List of forward rendered passes
	std::vector<glPass>				passinfo;
	// Deferred rendering information
	std::vector<glPass_Deferred>	deferredinfo;

	// Global options
	bool	m_isScreenShader;
	bool	m_isSkinnedShader;

public:
	// Constants (textures and uniforms)
	
	// Constants (Colors)
	Color		m_diffuse;
	Color		m_specular;
	float		m_specularPower;
	Color		m_emissive;

	// Constants (Other)
	Vector4d	gm_WindDirection;
	float		gm_FadeValue;
	float		gm_HalfScale;

public:
	//=========================================//
	// Sampler and Texture get/set interface
	//=========================================//

	RENDER_API void			setTexture ( const uint n_index, CTexture* n_texture );
	RENDER_API void			setSampler ( const uint n_index, const glHandle n_sampler, const glEnum n_sampler_target=0 );
	RENDER_API CTexture*	getTexture ( const uint n_index );

private:
	// Textures (samplers)
	CTexture*	m_highlevel_storage [12];
	glHandle	m_samplers [12];
	glEnum		m_sampler_targets [12];
public:
	// Textures (buffers)
	glHandle	m_bufferMatricesSkinning;
	//GLuint		m_bufferMatricesSoftbody;
	glHandle	m_bufferSkeletonSize;

	// Textures (global properties)
	RENDER_API static CTexture*	m_sampler_reflection;

private:
	// Static UBOs
	static glHandle	m_ubo_foginfo;
	static glHandle	m_ubo_lightinginfo;
	static glHandle	m_ubo_reflectinfo;

	static glHandle	m_ubo_deflightinginfo;
	static glHandle	m_tbo_lightinfo;
	static glHandle	m_tex_lightinfo;
	static glHandle	m_tex_shadowinfo;
	static glHandle	m_lightCount;
public:
	RENDER_API static void	updateStaticUBO ( void );
	RENDER_API static void	updateLightTBO ( void );

private:
	// Current update state
	static int	m_currentShaderState;

public:
	//=========================================//
	// Reference counting
	//=========================================//

	RENDER_API void		addReference ( void ) {
		referenceCount++;
	}
	RENDER_API void		removeReference ( void ) {
		if ( referenceCount <= 0 ) {
			throw std::exception();
		}
		referenceCount--;
	}
	RENDER_API bool		hasReference ( void ) const {
		return referenceCount>0;
	}
private:
	int		referenceCount;

public:
	// Get the name of the material
	RENDER_API const char*	getName ( void ) const {
		return m_name.c_str();
	}
	// Get the filename of the material
	RENDER_API const char*	getFilename ( void ) const {
		return m_filename.c_str();
	}
private:
	// Name of the material
	arstring<256>	m_name;
	arstring<256>	m_filename;
};

#endif//_GL_MATERIAL_H_
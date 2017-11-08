
#ifndef _GL_MATERIAL_H_
#define _GL_MATERIAL_H_

#include "core/types/types.h"
#include "core/math/Color.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector4d.h"
#include "core/containers/arstring.h"
#include "core-ext/transform/Transform.h"

#include "renderer/types/types.h"
#include "renderer/gpuw/Buffers.h"

#include "RrShader.h"
#include "RrPassForward.h"
#include "RrPassDeferred.h"

#include <vector>

// todo: move elsewhere
enum textureslot_t : uint8_t
{
	TEX_MAIN = 0,

	TEX_SLOT0 = 0,
	TEX_SLOT1 = 1,
	TEX_SLOT2 = 2,
	TEX_SLOT3 = 3,
	TEX_SLOT4 = 4,
	TEX_SLOT5 = 5,
	TEX_SLOT6 = 6,
	TEX_SLOT7 = 7,
	TEX_SLOT8 = 8,
	TEX_SLOT9 = 9,
	TEX_SLOT10 = 10,
	TEX_SLOT11 = 11,

	TEX_DIFFUSE = 0,
	TEX_NORMALS = 1,
	TEX_SURFACE = 2,
	TEX_OVERLAY = 3,

	TEX_PALETTE = 0,
};

// Prototype
class CRenderableObject;
class CTexture;
class CRenderTexture;
class CRenderState;

class RrMaterial
{
	friend CRenderState;
public:
	RENDER_API				RrMaterial ( void );
	RENDER_API				~RrMaterial ( void ) throw(...);
	
	// Load material from file
	RENDER_API void			loadFromFile ( const char* n_materialfile );
	// Check if file is material
	RENDER_API static bool	isValidFile ( const char* n_materialfile );
	// Duplicate material (starting with 1 reference)
	RENDER_API RrMaterial*	copy ( void );

	// Current material active and bound
	//RENDER_API static RrMaterial*	current;
	RENDER_API static RrMaterial*	Default;
	RENDER_API static RrMaterial*	Copy;
	RENDER_API static RrMaterial*	Fallback;
	// Current pass being bound
	//RENDER_API static uchar		current_pass;
	// Current material count
	RENDER_API static uint			current_sampler_slot;

	// Current material special mode. These are hard-coded special effects.
	// Controls the current main rendering path of the program.
	// Changing this may completely invalidate the rendering state. Set with caution.
	RENDER_API static uchar		special_mode;
public:
	RENDER_API void			bindPass ( uchar pass );
	RENDER_API void			bindPassAtrribs ( void );

	//	prepareShaderConstants( transform, identity ) : Push constants to GPU
	// Unlike the previous setShaderConstants(), this does not require the material to be bound.
	// Reads current pass and material states and pushes them to the per-object buffer on the GPU.
	// This has a bit of GPU delay, so should be done enough time before rendering to prevent an upload stall. (On consoles, this delay is minimized).
	RENDER_API void			prepareShaderConstants ( void );
	RENDER_API void			prepareShaderConstants ( const core::Transform& n_transform );
	RENDER_API void			prepareShaderConstants ( const XrTransform& n_transform );
	//RENDER_API void			prepareShaderConstants ( CRenderableObject* source_object, const bool n_force_identity=false );
	//RENDER_API void			setShaderConstants ( CRenderableObject* source_object, const bool n_force_identity=false );

	RENDER_API void			bindPassForward ( uchar pass );
	RENDER_API void			bindPassDeferred( uchar pass );

	RENDER_API uchar		getPassCount ( void );
	RENDER_API uchar		getPassCountForward ( void );
	RENDER_API uchar		getPassCountDeferred ( void );

	//	Returns shader currently in use for the current pass
	RENDER_API RrShader*	getUsingShader ( void );

	RENDER_API void			bindAttribute ( int attributeIndex, const uint vec_size, const uint vec_type, const bool normalize, const int struct_size, const void* struct_offset ); 
	RENDER_API void			bindAttributeI ( int attributeIndex, const uint vec_size, const uint vec_type, const int struct_size, const void* struct_offset ); 

private:
	// internal bind states:

	void	shader_bind_samplers ( RrShader* shader );

	void	deferred_shader_build ( uchar pass );

	// new bind:
	void	shader_set_constantbuffers ( RrShader* shader, uchar pass, bool forward );

public:
	// List of forward rendered passes
	std::vector<RrPassForward>	passinfo;
	// Deferred rendering information
	std::vector<RrPassDeferred>	deferredinfo;

	// Global options
	bool	m_isScreenShader;
	bool	m_isSkinnedShader;
	bool	m_isInstancedShader;

public:
	// Constants (textures and uniforms)
	
	// Constants (Colors)
	Color		m_diffuse;
	Color		m_specular;
	Color		m_emissive;

	// Contants (Coordinates)
	Vector4d	m_texcoordScaling;
	Vector4d	m_texcoordOffset;

	// Constants (Other)
	Vector4d	gm_WindDirection;
	Real		gm_FadeValue;
	Real		gm_HalfScale;

public:
	//=========================================//
	// Sampler and Texture get/set interface
	//=========================================//
	
	//	setTexture ( slot, texture ) : Sets material texture.
	// Material is given ownership of the texture (to an extent).
	// Do not delete the texture directly, use RemoveReference.
	RENDER_API void			setTexture ( const textureslot_t n_index, CTexture* n_texture );
	//	setSampler ( slot, texture ) : Sets material texture with raw GPU handles.
	RENDER_API void			setSampler ( const textureslot_t n_index, const glHandle n_sampler, const glEnum n_sampler_target=0 );
	//	getTexture ( slot ) : Returns texture object set to slot
	RENDER_API CTexture*	getTexture ( const textureslot_t n_index );

private:
	// Current state:
	RrShader*	m_currentShader;
	uchar		m_currentPass;
	bool		m_currentPassForward;

	// Command buffer buildstate:
	struct rrCmdBufferBuildstate
	{
		uint	cmdbuffer_index : 4;
		bool	track_state : 1;
	};
	rrCmdBufferBuildstate	m_buildState;

	// Textures (samplers)
	CTexture*	m_highlevel_storage [12];
	glHandle	m_samplers [12];
	glEnum		m_sampler_targets [12];

	// Constant Buffers
	gpu::ConstantBuffer	m_cbufPerObject;

public:
	// Textures (buffers)
	glHandle	m_bufferMatricesSkinning;
	//GLuint		m_bufferMatricesSoftbody;
	glHandle	m_bufferSkeletonSize;

	glHandle	m_tex_instancedinfo;

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

	RENDER_API static void	pushConstantsPerFrame ( void );
	RENDER_API static void	pushConstantsPerPass ( void );
	RENDER_API static void	pushConstantsPerCamera ( void );

	RENDER_API void			pushConstantsPerObject ( const Matrix4x4& modelTRS, const Matrix4x4& modelRS );

//private:
//	// Current update state
//	static int	m_currentShaderState;

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

	RENDER_API void		setStaticResource ( const bool static_state = true ) {
		staticResource = static_state;
	}
	RENDER_API bool		isStatic ( void ) const {
		return staticResource;
	}
private:
	int		referenceCount;
	bool	staticResource;

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
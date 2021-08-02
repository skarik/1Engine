//===============================================================================================//
//	class CRenderable2D
//
// Common class for rendering in the 2D system, which takes care of the deferred shading
//
//
//===============================================================================================//

#ifndef _RENDER2D_C_RENDERABLE_2D_H_
#define _RENDER2D_C_RENDERABLE_2D_H_

#include "core/types/ModelData.h"
#include "core/gfx/textureStructures.h"
#include "renderer/object/RrRenderObject.h"
#include "render2d/types/spriteInfo.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

struct rrSpriteGenParams
{
	Vector3f normal_default;
};

struct rrSpriteSetResult
{
	RrTexture*			textureAlbedo;
	RrTexture*			textureNormals;
	RrTexture*			textureSurface;
	RrTexture*			textureIllumin;
	RrTexture*			texturePalette;

	rrSpriteSetResult ()
		: textureAlbedo(NULL), textureNormals(NULL), textureSurface(NULL), textureIllumin(NULL),
		texturePalette(NULL)
		{}
};

class CRenderable2D : public RrRenderObject
{
public:
	RENDER2D_API explicit	CRenderable2D ( void );
	RENDER2D_API virtual	~CRenderable2D ();

	//	SetSpriteFile ( c-string sprite filename, set result )
	// Sets the sprite filename to load and possibly convert.
	// Arguments:
	//	n_sprite_resname: resource name of the sprite set to load. This does not need to include the extension.
	//	o_set_result : If not NULL, will be populated with the resulting loaded textures.
	RENDER2D_API void		SetSpriteFile ( const char* n_sprite_resname, rrSpriteSetResult* o_set_result );
	//	SetSpriteFileAnimated ( c-string sprite filename, sprite info, set result )
	// Sets the sprite filename to load and possibly convert, but provides returns for additional information.
	// Arguments:
	//	n_sprite_resname: resource name of the sprite set to load. This does not need to include the extension.
	//	o_sprite_info : If not NULL, will be populated with animation and other information of the loaded sprite.
	//	o_set_result : If not NULL, will be populated with the resulting loaded textures.
	RENDER2D_API void		SetSpriteFileAnimated ( const char* n_sprite_resname, core::gfx::tex::arSpriteInfo* o_sprite_info, rrSpriteSetResult* o_set_result );

	//	GetSpriteInfo ()
	// Returns read-only reference to the current sprite information structure.
	RENDER2D_API const render2d::rrSpriteInfo&
							GetSpriteInfo ( void );

	//	SpriteGenParams ()
	// Returns read-write reference to the sprite generation parameters
	RENDER2D_API rrSpriteGenParams&
							SpriteGenParams ( void );

	//	PreRender()
	// Push the uniform properties
	RENDER2D_API bool		PreRender ( rrCameraPass* cameraPass ) override;

	//	Render()
	// Render the model using the 2D engine's style
	RENDER2D_API bool		Render ( const rrRenderParams* params ) override;

protected:

	//	PushModelData()
	// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
	RENDER2D_API void		PushModeldata ( void );

	//	Mesh information
	arModelData			m_modeldata;
	//	GPU information
	rrMeshBuffer		m_meshBuffer;

	//	Sprite information
	render2d::rrSpriteInfo
						m_spriteInfo;
	//	Sprite generation parameters
	rrSpriteGenParams	m_spriteGenerationInfo;

	//	Sprite textures
	RrTexture*			m_textureAlbedo; // Either albedo or lookup
	RrTexture*			m_texturePalette;
	RrTexture*			m_textureNormals;
	RrTexture*			m_textureSurface;
	RrTexture*			m_textureIllumin;
};

#endif//_RENDER2D_C_RENDERABLE_2D_H_
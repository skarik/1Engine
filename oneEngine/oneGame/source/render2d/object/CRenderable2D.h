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
#include "renderer/object/CRenderableObject.h"
#include "render2d/types/spriteInfo.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

struct rrSpriteGenParams
{
	Vector3f normal_default;
};

class CRenderable2D : public CRenderableObject
{
public:
	RENDER2D_API explicit	CRenderable2D ( void );
	RENDER2D_API virtual	~CRenderable2D ();

	//		SetSpriteFile ( c-string sprite filename )
	// Sets the sprite filename to load and possibly convert.
	RENDER2D_API void		SetSpriteFile ( const char* n_sprite_resname );
	//		SetSpriteFileAnimated ( c-string sprite filename )
	// Sets the sprite filename to load and possibly convert, but provides returns for additional information.
	RENDER2D_API void		SetSpriteFileAnimated ( const char* n_sprite_resname, core::gfx::tex::arSpriteInfo* o_sprite_info );

	//		GetSpriteInfo ()
	// Returns read-only reference to the current sprite information structure.
	RENDER2D_API const render2d::rrSpriteInfo&
							GetSpriteInfo ( void );

	//		SpriteGenParams ()
	// Returns read-write reference to the sprite generation parameters
	RENDER2D_API rrSpriteGenParams&
							SpriteGenParams ( void );

	//		PreRender()
	// Push the uniform properties
	RENDER2D_API bool		PreRender ( rrCameraPass* cameraPass ) override;

	//		Render()
	// Render the model using the 2D engine's style
	RENDER2D_API bool		Render ( const rrRenderParams* params ) override;

protected:

	//		PushModelData()
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
};

#endif//_RENDER2D_C_RENDERABLE_2D_H_
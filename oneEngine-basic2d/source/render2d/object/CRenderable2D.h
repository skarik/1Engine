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
#include "renderer/object/CRenderableObject.h"

class CRenderable2D : public CRenderableObject
{
public:
	RENDER2D_API explicit	CRenderable2D ( void );
	RENDER2D_API virtual	~CRenderable2D ();

	//		SetSpriteFile ( c-string sprite filename )
	// Sets the sprite filename to load or convert
	RENDER2D_API void		SetSpriteFile ( const char* n_sprite_filename );

	//		Render()
	// Render the model using the 2D engine's style
	RENDER2D_API bool		Render ( const char pass ) override;

protected:

	//		PushModelData()
	// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
	RENDER2D_API void		PushModeldata ( void );

	//	Mesh information
	ModelData				m_modeldata;
	//	GPU information
	uint					m_buffer_verts;
	uint					m_buffer_tris;
};

#endif//_RENDER2D_C_RENDERABLE_2D_H_
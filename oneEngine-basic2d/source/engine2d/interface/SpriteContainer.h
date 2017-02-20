
#ifndef _ENGINE2D_INTERFACE_SPRITECONTAINER_
#define _ENGINE2D_INTERFACE_SPRITECONTAINER_

#include "core/math/Math2d.h"
#include "core/math/Math3d.h"
#include "core/math/vect2d_template.h"
#include "renderer/logic/CLogicObject.h"

class CStreamedRenderable2D;

namespace Engine2D
{
	class SpriteContainer : CLogicObject
	{
	protected:
		//		SpriteContainer (Constructor)
		ENGINE2D_API explicit		SpriteContainer ( Vector3d* position, Real* angle = NULL, Vector3d* scale = NULL );
		//		SpriteContinaer (Destructor)
		ENGINE2D_API				~SpriteContainer ( void );

	private:
		//		PreStep()
		// Used to update the position of the renderer as well as animation parameters.
		ENGINE2D_API void			PreStep ( void ) override;

		//		PostStepSynchronus()
		// Used to update the geometry generated for rendering.
		ENGINE2D_API void			PostStepSynchronus ( void ) override;

	public:
		//		UpdateSpriteProperties()
		// Pulls sprite info from the m_sprite object
		ENGINE2D_API void			UpdateSpriteProperties ( void );

		//		GetSpriteRect()
		// Pulls untransformed sprite rect, using the m_sprite object and taking into account origin.
		ENGINE2D_API Rect			GetSpriteRect ( void );

	protected:
		// Renderer:

		//	Current sprite renderer.
		CStreamedRenderable2D*		m_sprite;

		// Animation information:

		//	Current frame.
		// The frame will be one frame ahead of the actual display frame
		Real						m_spriteFrame;
		//	Framespeed.
		// In frames per second played.
		Real						m_spriteFramespeed;

		// Sprite information:

		//	Size of one frame of the sprite
		Vector2i					m_spriteSize;
		//	Offset of the sprite to use for centering
		Vector2i					m_spriteOrigin;
		//	Number of frames in the sprite
		int32_t						m_spriteFramecount;
	private:
		// Source transform pointers:

		Vector3d const*				m_sourcePosition;
		Real const*					m_sourceAngle;
		Vector3d const*				m_sourceScale;
	};
}

#endif//_ENGINE2D_INTERFACE_SPRITECONTAINER_
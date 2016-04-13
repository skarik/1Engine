#ifndef _EDITOR_UI_EDITOR_OBJECT_H_
#define _EDITOR_UI_EDITOR_OBJECT_H_

#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

namespace M04
{
	class EditorObject : public CGameBehavior, public Engine2D::SpriteContainer
	{
	public:
		explicit			EditorObject ( const char* object_name );
							~EditorObject ( void );

		void				Update ( void ) override;
	public:
		Engine::ObjectBase*	m_object;

	public:
		Vector3d	position;
		float		angle;
		Vector3d	scale;
	};
}


#endif//_EDITOR_UI_EDITOR_OBJECT_H_
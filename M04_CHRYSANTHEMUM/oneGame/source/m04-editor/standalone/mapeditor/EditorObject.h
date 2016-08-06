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

	public:
		//=========================================//
		// Static listing
		//=========================================//
		
		//		Objects ()
		// Returns read-only list of all editor objects
		GAME_API static const std::vector<EditorObject*>& Objects ( void );
	private:
		static std::vector<EditorObject*>	m_objects;
	};
}


#endif//_EDITOR_UI_EDITOR_OBJECT_H_
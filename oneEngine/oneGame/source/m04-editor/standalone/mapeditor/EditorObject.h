#ifndef _EDITOR_UI_EDITOR_OBJECT_H_
#define _EDITOR_UI_EDITOR_OBJECT_H_

#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

#include <map>

class CLight;

namespace M04
{
	class EditorObject : public CGameBehavior, public Engine2D::SpriteContainer
	{
	public:
		explicit			EditorObject ( const char* object_name );
							~EditorObject ( void );

		void				Update ( void ) override;

		//	WorldToMetadata ( ) : Moves information from the object's properties to the data storage
		// Called every frame or on object changed by editor.
		void				WorldToMetadata ( void );
		//	MetadataToWorld ( ) : Moves information in the current data storage to the object's properties
		// Normally called on level load or properties edit.
		void				MetadataToWorld ( void );
	public:
		arstring128			m_object_name;
		Engine::ObjectBase*	m_object;

	public:
		Vector3d	position;
		float		angle;
		Vector3d	scale;

		uint32_t	m_data_storage_buffer_size;
		char*		m_data_storage_buffer;

	public:
		CLight*		light;

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
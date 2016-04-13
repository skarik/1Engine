
#include "EditorObject.h"
#include "render2d/object/CRenderable2D.h"

using namespace M04;

EditorObject::EditorObject ( const char* object_name )
	: CGameBehavior(), Engine2D::SpriteContainer( &position, &angle, &scale ),
	position( 0,0,0 ), angle( 0 ), scale( 1,1,1 )
{
	auto registration = Engine::BehaviorList::GetRegistration(object_name);
	m_object = registration.editor_inst();

	auto metadata = m_object->GetMetadata();

	// Pull wanted keys all at once
	const Engine::Metadata* mt_display_mode = metadata->Get<METADATA_DISPLAY_MODE>();
	const Engine::Metadata* mt_display_file = metadata->Get<METADATA_DISPLAY_FILENAME>();

	// Check display mode for the editor
	if ( mt_display_mode && mt_display_file )
	{
		arstring128 file_key = Engine::MetadataTo<arstring128>(mt_display_file);
		switch ( Engine::MetadataTo<fielddisplay_t>(mt_display_mode) )
		{
		case DISPLAY_BOX:
			// Set to box
			m_sprite->SetSpriteFile("textures/white");
			// Change scale
			scale *= (Real) atoi( file_key );
			break;
		case DISPLAY_2D_SPRITE:
			m_sprite->SetSpriteFile( file_key );
			break;
		case DISPLAY_3D_MODEL: // TODO someday
			// m_model = new CSkinnedModel( file_key );
			// m_model->SetOwner( this );
			break;
		}
	}
	else
	{
		// Default to a stupidass box
		m_sprite->SetSpriteFile("textures/white");
	}
}

EditorObject::~EditorObject ( void )
{
	delete_safe( m_object );
}

void EditorObject::Update ( void )
{
	; // Nothing.
}
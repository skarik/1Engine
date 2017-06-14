
#include "EditorObject.h"
#include "render2d/object/sprite/CEditableRenderable2D.h"

#include "renderer/light/CLight.h"

using namespace M04;

std::vector<EditorObject*>	EditorObject::m_objects;
const std::vector<EditorObject*>& EditorObject::Objects ( void )
{
	return m_objects;
}

EditorObject::EditorObject ( const char* object_name )
	: CGameBehavior(), Engine2D::SpriteContainer( &position, &angle, &scale ),
	m_object_name( object_name ),
	position( 0,0,0 ), angle( 0 ), scale( 1,1,1 ),
	light(NULL)
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
			m_spriteOrigin = m_sprite->GetSpriteInfo().fullsize / 2;
			break;
		case DISPLAY_3D_MODEL: // TODO someday
			// m_model = new CSkinnedModel( file_key );
			// m_model->SetOwner( this );
			break;
		case DISPLAY_LIGHT:
			m_sprite->SetSpriteFile( file_key );
			m_spriteOrigin = m_sprite->GetSpriteInfo().fullsize / 2;
			light = new CLight;
			light->range = 128.0F;
			break;
		}
	}
	else
	{
		// Default to a stupidass box
		m_sprite->SetSpriteFile("textures/white");
	}

	// Generate the data we need to store
	m_data_storage_buffer_size = 0;
	for ( uint32_t i = 0; i < m_object->GetMetadata()->data.size(); ++i )
	{
		uint32_t offset = m_object->GetMetadata()->data[i].first;
		uint32_t size = m_object->GetMetadata()->data[i].second->source_size;

		m_data_storage_buffer_size = std::max<uint32_t>( m_data_storage_buffer_size, offset + size );
	}
	m_data_storage_buffer = new char[m_data_storage_buffer_size];

	// Add this object to the list
	m_objects.push_back( this );
}

EditorObject::~EditorObject ( void )
{
	delete_safe( m_object );
	delete_safe( light );

	m_objects.erase( std::find( m_objects.begin(), m_objects.end(), this ) );

	delete [] m_data_storage_buffer;
	m_data_storage_buffer = NULL;
}

void EditorObject::Update ( void )
{
	// Update child objects the engine could use
	if ( light != NULL )
	{
		light->position = position;
	}
	position.z = -495;
	
	// Update metadata entries if we have the entries to pull/save them from
	WorldToMetadata();
}


//	WorldToMetadata ( ) : Moves information from the object's properties to the data storage
// Called every frame or on object changed by editor.
void EditorObject::WorldToMetadata ( void )
{
	auto metadata = m_object->GetMetadata();
	for ( uint32_t i = 0; i < metadata->data.size(); ++i )
	{
		size_t kv_offset = (size_t)metadata->data[i].second->source;

		// First do field matching
		switch ( metadata->data_field[i].second )
		{
		case FIELD_POSITION:
			memcpy( m_data_storage_buffer + kv_offset, &position, sizeof(Vector3d) );
			break;

		case FIELD_ROTATION:
			throw core::NotYetImplementedException();
			break;

		case FIELD_SCALE:
			memcpy( m_data_storage_buffer + kv_offset, &scale, sizeof(Vector3d) );
			break;

		case FIELD_COLOR:
			if (light != NULL)
				memcpy( m_data_storage_buffer + kv_offset, &light->diffuseColor, sizeof(Color) );
			break;

			// Second do name matching
		case FIELD_DEFAULT:
			if ( metadata->data_name[i].second.compare("range") )
			{
				if (light != NULL)
					memcpy( m_data_storage_buffer + kv_offset, &light->range, sizeof(float) );
			}
			else if ( metadata->data_name[i].second.compare("power") )
			{
				if (light != NULL)
					memcpy( m_data_storage_buffer + kv_offset, &light->falloff, sizeof(float) );
			}
			break;
		}
	}
}
//	MetadataToWorld ( ) : Moves information in the current data storage to the object's properties
// Normally called on level load or properties edit.
void EditorObject::MetadataToWorld ( void )
{
	auto metadata = m_object->GetMetadata();
	for ( uint32_t i = 0; i < metadata->data.size(); ++i )
	{
		size_t kv_offset = (size_t)metadata->data[i].second->source;

		// First do field matching
		switch ( metadata->data_field[i].second )
		{
		case FIELD_POSITION:
			memcpy( &position, m_data_storage_buffer + kv_offset, sizeof(Vector3d) );
			break;

		case FIELD_ROTATION:
			throw core::NotYetImplementedException();
			break;

		case FIELD_SCALE:
			memcpy( &scale, m_data_storage_buffer + kv_offset, sizeof(Vector3d) );
			break;

		case FIELD_COLOR:
			if (light != NULL)
				memcpy( &light->diffuseColor, m_data_storage_buffer + kv_offset, sizeof(Color) );
			break;

			// Second do name matching
		case FIELD_DEFAULT:
			if ( metadata->data_name[i].second.compare("range") )
			{
				if (light != NULL)
					memcpy( &light->range, m_data_storage_buffer + kv_offset, sizeof(float) );
			}
			else if ( metadata->data_name[i].second.compare("power") )
			{
				if (light != NULL)
					memcpy( &light->falloff, m_data_storage_buffer + kv_offset, sizeof(float) );
			}
			break;
		}
	}
}
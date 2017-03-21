
#include "AnimationContainer.h"
#include "renderer/material/glMaterial.h"
#include "render2d/object/sprite/CStreamedRenderable2D.h"

#include "core/time/Time.h"
#include "core/math/Math.h"

using namespace Engine2D;


//		PreStep()
// Updates SpriteContainer properties, and then updates texture transformation uniforms used for animation.
void AnimationContainer::PreStep ( void )
{
	SpriteContainer::PreStep();

	// Update material transformations:

	animation_entry_t* entry = &m_animations[m_current_animation].first;
	AnimationContainerSubstate* state = m_animations[m_current_animation].second;

	m_sprite->GetMaterial()->m_texcoordOffset = Vector4d(
		(state->m_frame % entry->texture.frame_count.x) / (Real)entry->texture.frame_count.x,
		(state->m_frame / entry->texture.frame_count.y) / (Real)entry->texture.frame_count.y,
		0,0);
	m_sprite->GetMaterial()->m_texcoordScaling = Vector4d(
		1.0F / (Real)entry->texture.frame_count.x,
		1.0F / (Real)entry->texture.frame_count.y,
		0,0);

	// Update material textures:

	m_sprite->GetMaterial()->setTexture(TEX_DIFFUSE, entry->texture.diffuse);
	m_sprite->GetMaterial()->setTexture(TEX_NORMALS, entry->texture.normals);
	m_sprite->GetMaterial()->setTexture(TEX_SURFACE, entry->texture.surface);
	m_sprite->GetMaterial()->setTexture(TEX_OVERLAY, entry->texture.overlay);
	
}

//		PostStepSynchronus()
// Updates SpriteContainer properties, then updates animation events.
void AnimationContainer::PostStepSynchronus ( void )
{
	SpriteContainer::PostStepSynchronus();

	// Run through the animation system:

	animation_entry_t* entry = &m_animations[m_current_animation].first;
	AnimationContainerSubstate* state = m_animations[m_current_animation].second;
	{
		// Increment time
		state->time += Time::deltaTime * state->timescale;

		// Caclulate the current frame
		Real frametime = fmod( state->time, state->m_length );
		uint32_t frame = 0;
		while ( frametime > 0 )
		{
			frametime -= entry->frame_times[frame];
			frame += 1;
		}

		// Set calculated frame
		state->m_frame = frame;
	}
}


//		operator[]
// Array access operator for accessing animation states
AnimationContainerSubstate& AnimationContainer::operator[](const uint32_t index)
{
	if ( index >= m_animations.size() ) {
		throw Core::InvalidArgumentException();
	}
	return *m_animations[index].second;
}


//		Play
// Plays animation with given index
void AnimationContainer::Play ( const uint32_t animation_index )
{
	if ( m_current_animation != animation_index )
	{
		m_current_animation = animation_index;
		(*this)[animation_index].time = 0.0F;
	}
}

//		IsPlaying
// Is the animation currently active?
bool AnimationContainer::IsPlaying ( const uint32_t animation_index ) const
{
	return m_current_animation == animation_index;
}


//		AddFromFile
// Adds a new animation to this object, either reading from file or using cached texture info
uint32_t AnimationContainer::AddFromFile ( const Animation::types_t n_anim_type, const uint8_t n_userid, const char* n_filename )
{


	return (uint32_t)(m_animations.size() - 1);
}
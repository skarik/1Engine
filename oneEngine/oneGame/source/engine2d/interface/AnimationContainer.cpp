#include "AnimationContainer.h"

#include "core/time/Time.h"
#include "core/math/Math.h"
#include "core/utils/string.h"

#include "core-ext/system/io/Resources.h"

//#include "renderer/material/RrShader.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/texture/RrTexture.h"
//#include "renderer/resource/CResourceManager.h"

#include "render2d/object/sprite/CEditableRenderable2D.h"
//#include "render2d/texture/TextureIO.h"

using namespace Engine2D;


AnimationContainer::AnimationContainer ( Vector3f* position, Real* angle, Vector3f* scale )
	: SpriteContainer(position, angle, scale), 
	m_current_animation(0)
{
	m_sprite = new CEditableRenderable2D();
}
AnimationContainer::~AnimationContainer ( void )
{
	// TODO:
}

//		PreStep()
// Updates SpriteContainer properties, and then updates texture transformation uniforms used for animation.
void AnimationContainer::PreStep ( void )
{
	SpriteContainer::PreStep();
	if (m_animations.empty()) return;

	// Update material transformations:

	animation_entry_t* entry = m_animations[m_current_animation].first;
	AnimationContainerSubstate* state = m_animations[m_current_animation].second;

	m_sprite->PassGetSurface(0).textureOffset = Vector4f(
		(state->m_frame % entry->texture.frame_count.x) / (Real)entry->texture.frame_count.x,
		(state->m_frame / entry->texture.frame_count.x) / (Real)entry->texture.frame_count.y,
		0,0);
	m_sprite->PassGetSurface(0).textureScale = Vector4f(
		1.0F / (Real)entry->texture.frame_count.x,
		1.0F / (Real)entry->texture.frame_count.y,
		0,0);

	// Update material textures:

	m_sprite->PassAccess(0).setTexture(TEX_DIFFUSE, entry->texture.diffuse);
	m_sprite->PassAccess(0).setTexture(TEX_NORMALS, entry->texture.normals);
	m_sprite->PassAccess(0).setTexture(TEX_SURFACE, entry->texture.surface);
	m_sprite->PassAccess(0).setTexture(TEX_OVERLAY, entry->texture.overlay);
}

//		PostStepSynchronus()
// Updates SpriteContainer properties, then updates animation events.
void AnimationContainer::PostStepSynchronus ( void )
{
	if (m_animations.empty()) return;

	// Update render mesh:

	animation_entry_t* entry = m_animations[m_current_animation].first;
	SetSpriteSize( entry->texture.frame_size );

	// Update base mesh:

	SpriteContainer::PostStepSynchronus();

	// Run through the animation system:

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
		state->m_frame = frame - 1;
	}
}


//		operator[]
// Array access operator for accessing animation states
AnimationContainerSubstate& AnimationContainer::operator[](const uint32_t index)
{
	if ( index >= m_animations.size() ) {
		throw core::InvalidArgumentException();
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
// Returns the index of the new animation, which can be used to play it manually.
uint32_t AnimationContainer::AddFromFile ( const animation::arAnimType n_anim_type, const uint8_t n_userid, const char* n_sprite_filename, const char* n_palette_filename )
{
	// Create empty entry:
	animation_entry_t* entry = new animation_entry_t();
	memset( entry, 0, sizeof(animation_entry_t) );

	// Load the sprite
	//Textures::timgInfo img_info;
	//Real* img_frametimes = NULL;
	//m_sprite->SetSpriteFileAnimated( n_sprite_filename, n_palette_filename, &img_info, &img_frametimes );
	core::gfx::tex::arSpriteInfo sprite_info;
	rrSpriteSetResult set_info;
	m_sprite->SetSpriteFileAnimated( n_sprite_filename, &sprite_info, &set_info );
	
	// Create the entry frame size needed for streaming texture offsets
	entry->frame_count = sprite_info.animationInfo.framecount;
	entry->texture.frame_count = Vector2i(sprite_info.animationInfo.xdivs, sprite_info.animationInfo.ydivs);
	entry->texture.frame_size = Vector2i(sprite_info.info.width / sprite_info.animationInfo.xdivs, sprite_info.info.height / sprite_info.animationInfo.ydivs);

	entry->frame_times = new Real [entry->frame_count];
	for (uint i = 0; i < entry->frame_count; ++i)
		entry->frame_times[i] = sprite_info.frame_times[i] / (Real)sprite_info.animationInfo.framerate;
	
	// Pull the needed sprite properties from the sprite
	entry->texture.diffuse = set_info.textureAlbedo;
	entry->texture.normals = set_info.textureNormals;
	entry->texture.surface = set_info.textureSurface;
	entry->texture.overlay = set_info.textureIllumin;

	// Update the sprite rect
	m_spriteSize = entry->texture.frame_size;

	// Create animation state using the set entry:
	AnimationContainerSubstate* state = new AnimationContainerSubstate((uint32_t)m_animations.size(), this, entry );

	// Add the result state now
	m_animations.push_back(std::pair<animation_entry_t*, AnimationContainerSubstate*>(entry, state));
	return (uint32_t)(m_animations.size() - 1);
}
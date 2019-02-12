
#ifndef _ENGINE2D_INTERFACE_ANIMATIONCONTAINER_
#define _ENGINE2D_INTERFACE_ANIMATIONCONTAINER_

#include "core-ext/animation/Types.h"
#include "core-ext/animation/AnimationEvents.h"
#include "SpriteContainer.h"

class RrTexture;

namespace Engine2D
{
	class AnimationContainer;
	class AnimationContainerSubstate;

	struct animation_texture_info_t
	{
		RrTexture*	diffuse;
		RrTexture*	normals;
		RrTexture*	surface;
		RrTexture*	overlay;

		Vector2i	frame_size;
		Vector2i	frame_count;
	};

	struct animation_entry_t
	{
		// User management values:

		uint8_t					userid;		// User set ID for additional management
		animation::arAnimType	type;		// Animation type

		// Animation storage:

		animation_texture_info_t	texture;		// Animation's texture reference
		Real*						frame_times;	// Time in seconds of each frame
		uint16_t					frame_count;	// Number of frames

		// Animation Event system:

		animation::ActionEvent*	events;			// Array of animation events
		uint32_t				events_count;	// Size of array
	};


	class AnimationContainer : public SpriteContainer
	{
	protected:
		//		SpriteContainer (Constructor)
		ENGINE2D_API explicit		AnimationContainer ( Vector3f* position, Real* angle = NULL, Vector3f* scale = NULL );
		//		SpriteContinaer (Destructor)
		ENGINE2D_API				~AnimationContainer ( void );

	private:
		//		PreStep()
		// Updates SpriteContainer properties, and then updates texture transformation uniforms used for animation.
		ENGINE2D_API void			PreStep ( void ) override;

		//		PostStepSynchronus()
		// Updates SpriteContainer properties, then updates animation events.
		ENGINE2D_API void			PostStepSynchronus ( void ) override;

	public:
		//		AddFromFile
		// Adds a new animation to this object, either reading from file or using cached texture info.
		// Returns the index of the new animation, which can be used to play it manually.
		ENGINE2D_API uint32_t AddFromFile ( const animation::arAnimType n_anim_type, const uint8_t n_userid, const char* n_filename, const char* n_palette_filename = NULL );


		//		operator[]
		// Array access operator for accessing animation states
		ENGINE2D_API AnimationContainerSubstate&	operator[](const uint32_t index);


		//		Play
		// Plays animation with given index
		ENGINE2D_API void Play ( const uint32_t animation_index );

		//		IsPlaying
		// Is the animation currently active?
		ENGINE2D_API bool IsPlaying ( const uint32_t animation_index ) const;

	private:

		//	Animation listing.
		std::vector< std::pair<animation_entry_t*, AnimationContainerSubstate*> > m_animations;
		// Current playing animation
		uint32_t m_current_animation;

	};


	class AnimationContainerSubstate
	{
	public:
		// Public settable current state:

		Real time;			// Current animation time
		Real timescale;		// Multipler for animation playback speed

							// Public nonsettable current state:

		ENGINE2D_API Real		GetLength ( void )	{ return m_length; }
		ENGINE2D_API uint32_t	GetFrame ( void )	{ return m_frame; }
		ENGINE2D_API bool		IsPlaying ( void )	{ return m_owner->IsPlaying(m_index); }

	private:
		friend AnimationContainer;

		uint32_t			m_index;		// Index in the container object
		AnimationContainer*	m_owner;		// Animation object that contains this
		animation_entry_t*	m_entry;		// Animation content and information
		Real				m_length;		// Total animation length in seconds
		uint32_t			m_frame;		// Current animation frame

											//		Private constructor
		explicit AnimationContainerSubstate ( const uint32_t n_index, AnimationContainer* n_owner, animation_entry_t* n_entry )
			: m_index(n_index), m_owner(n_owner), m_entry(n_entry), time(0), timescale(1.0F), m_frame(0)
		{
			// Create length of the animation now
			m_length = 0;
			for ( uint16_t f = 0; f < m_entry->frame_count; ++f )
			{
				m_length += m_entry->frame_times[f];
			}
		}

	public:
		// Public interface:

		ENGINE2D_API void Play ( void )		{ m_owner->Play(m_index); }
	};
}

#endif//_ENGINE2D_INTERFACE_ANIMATIONCONTAINER_
#ifndef AUDIO_EFFECT_H_
#define AUDIO_EFFECT_H_

#include "core/math/Vector3.h"
#include "audio/types/Handles.h"
#include "audio/mixing/Channels.h"
#include "audio/mixing/Workbuffer.h"
#include "audio/mixing/Mixer.h"

namespace audio
{
	struct EffectState
	{
	public:
		explicit				EffectState ( void )
			{}
		virtual					~EffectState ( void )
			{}
	};

	class Effect
	{
	protected:
		friend Manager;
		AUDIO_API virtual		~Effect ( void ); // Cannot be deleted by outsiders

	public:
		AUDIO_API explicit		Effect ( MixChannel targetChannel );
		
		//	Destroy() : Request this effect to be destroyed and freed when no longer in use
		AUDIO_API void			Destroy ( void );

		//	AllocateState() : Allocates delete-safe state for the mixer thread.
		AUDIO_API virtual EffectState*
								AllocateState ( void ) =0;
		
		//	UpdateStateForMixerThread( state ) : Grabs current game-side state and updates the given state in the mixer thread.
		AUDIO_API virtual void	UpdateStateForMixerThread ( EffectState* state ) =0;
		
		//	Evaluate( input, output, state ) : Performs the actual work in the mixer thread.
		AUDIO_API virtual void	Evaluate ( WorkbufferStereo<audio::kWorkbufferSize>& input, WorkbufferStereo<audio::kWorkbufferSize>& output, EffectState* effectState ) =0;

	public:
		// Marked for deletion?
		bool				queue_destruction = false;
		// For derived classes to use in UpdateStateForMixerThread
		std::mutex			mixer_read_lock; 
	};
};

#endif//AUDIO_EFFECT_H_
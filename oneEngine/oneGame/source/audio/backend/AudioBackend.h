#ifndef AUDIO_THREAD_H_
#define AUDIO_THREAD_H_

#include "core/os.h"
#include "core/types.h"

namespace audio
{
	class AudioBackend
	{
	public:
		virtual void			Start ( void )
			{}
		virtual void			Stop ( void ) 
			{}

		virtual int32_t			SampleRate ( void ) = 0;
		virtual int32_t			ChannelCount ( void ) = 0;

		virtual uint32_t		AvailableFrames ( void ) = 0;
		virtual void			SubmitFrames ( uint32_t frameCount, float* pcmFrames ) = 0;
	};
} 

#endif//AUDIO_THREAD_H_
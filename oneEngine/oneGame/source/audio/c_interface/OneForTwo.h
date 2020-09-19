//===============================================================================================//
//
//	OneForTwo.cpp
//
// Straightforward C interface, targeting Game Maker Studio 2.
//
//===============================================================================================//
#ifndef AUDIO_C_INTERFACE_ONE_FOR_TWO_H_
#define AUDIO_C_INTERFACE_ONE_FOR_TWO_H_

#include "config.h"
#include "core/types.h"

#if ONEFORTWO_ENABLED

#define AR_CEXPORT extern "C" AR_EXPORT

//	AudioInitialize() : Starts up the job system & audio engine
AR_CEXPORT double AR_CALL AudioInitialize ( double startupMask );
//	AudioFree() : Stops the job system & audio engine, frees stored memory
AR_CEXPORT double AR_CALL AudioFree ( void );
//	AudioGameTick() : Ticks the system forward.
AR_CEXPORT double AR_CALL AudioUpdate ( double deltatime );

AR_CEXPORT double AR_CALL AudioSetChannelGain ( double channel, double gain );
AR_CEXPORT double AR_CALL AudioSetSoundSpeed ( double speed_of_sound );

//
// Listener management:
//

AR_CEXPORT double AR_CALL AudioListenerCreate ( void );
AR_CEXPORT double AR_CALL AudioListenerDestroy ( double listener );

AR_CEXPORT double AR_CALL AudioListenerSetPosition ( double listener, double x, double y, double z );
AR_CEXPORT double AR_CALL AudioListenerSetVelocity ( double listener, double x, double y, double z );
AR_CEXPORT double AR_CALL AudioListenerSetOrientation ( double listener, double x_forward, double y_forward, double z_forward, double x_up, double y_up, double z_up );

//
// Buffer management:
//

//	AudioBufferLoad( filename ) : Loads the given file as a buffer.
AR_CEXPORT double AR_CALL AudioBufferLoad ( const char* filename );
AR_CEXPORT double AR_CALL AudioBufferFree ( double buffer );

//
// Source management:
//

//	AudioSourceCreate( buffer ) : Creates a new audio source.
AR_CEXPORT double AR_CALL AudioSourceCreate ( double buffer );
//	AudioSourceFree( source ) : Requests given source to be freed. Depending on DSP, this may be delayed.
AR_CEXPORT double AR_CALL AudioSourceDestroy ( double source );

AR_CEXPORT double AR_CALL AudioSourcePlay ( double source, double reset );
AR_CEXPORT double AR_CALL AudioSourcePause ( double source );
AR_CEXPORT double AR_CALL AudioSourceStop ( double source );

AR_CEXPORT double AR_CALL AudioSourceIsPlaying ( double source );
AR_CEXPORT double AR_CALL AudioSourcePlayed ( double source );

AR_CEXPORT double AR_CALL AudioSourceSetPlaybackTime ( double source, double time );
AR_CEXPORT double AR_CALL AudioSourceGetPlaybackTime ( double source );
AR_CEXPORT double AR_CALL AudioSourceGetSoundLength ( double source );

AR_CEXPORT double AR_CALL AudioSourceGetCurrentMagnitude ( double source );

AR_CEXPORT double AR_CALL AudioSourceSetPosition ( double source, double x, double y, double z );
AR_CEXPORT double AR_CALL AudioSourceSetVelocity ( double source, double x, double y, double z );
AR_CEXPORT double AR_CALL AudioSourceSetLooped ( double source, double looped );
AR_CEXPORT double AR_CALL AudioSourceSetPitch ( double source, double pitch );
AR_CEXPORT double AR_CALL AudioSourceSetGain ( double source, double gain );
AR_CEXPORT double AR_CALL AudioSourceSetSpatial ( double source, double spatial );
AR_CEXPORT double AR_CALL AudioSourceSetChannel ( double source, double channel );

AR_CEXPORT double AR_CALL AudioSourceSetFalloff ( double source, double min_distance, double max_distance );
AR_CEXPORT double AR_CALL AudioSourceSetFalloffModel ( double source, double model, double falloff );

#endif

#endif//AUDIO_C_INTERFACE_INITIALIZATION_H_
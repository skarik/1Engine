#include "OneForTwo.h"

#if ONEFORTWO_ENABLED

#include "core/settings/CGameSettings.h"
#include "core-ext/threads/Jobs.h"
#include "core/debug/console.h"
#include "core/math/Math.h"

#include "audio/Manager.h"
#include "audio/Buffer.h"
#include "audio/BufferManager.h"
#include "audio/Source.h"
#include "audio/Listener.h"

#include "core/os.h"
#include "core-ext/system/shell/Message.h"

static CGameSettings*		m_settings = NULL;
static core::jobs::System*	m_jobSystem = NULL;
static audio::Manager*		m_manager = NULL;

template <typename Referenced>
static double HandleFromReference ( const Referenced* ref )
{
	// For now, we just do something INCREDIBLY unsafe, which is cast the pointer to an integer, then to a double.
	// Yeah, that's not problematic at all.
	return (double)((uint64_t)(ref));
}

template <typename Referenced>
static Referenced* ReferenceFromHandle ( const double handle )
{
	return (Referenced*)((uint64_t)(handle));
}

static double BoolToDouble ( const bool boolean )
{
	return boolean ? 1.0 : 0.0;
}


double AR_CALL AudioInitialize ( void )
{
	// Load window settings
	m_settings = new CGameSettings(string(""));

	// Create jobs system first
	m_jobSystem = new core::jobs::System (4);

	// Create debug console
	debug::CDebugConsole::Init();

	// Create the manager
	m_manager = new audio::Manager();

	return NIL;
}

double AR_CALL AudioFree ( void )
{
	delete m_manager;

	// Job system is removed last
	delete m_jobSystem;

	// Settings freed very last
	delete m_settings;

	return NIL;
}

double AR_CALL AudioUpdate ( double deltatime )
{
	m_manager->Update((float)deltatime);
	return NIL;
}

double AR_CALL AudioSetChannelGain ( double channel, double gain )
{
	m_manager->m_channelGain[(uint)channel] = (float)gain;
	return NIL;
}
double AR_CALL AudioSetSoundSpeed ( double speed_of_sound )
{
	m_manager->m_speedOfSound = (float)speed_of_sound;
	return NIL;
}


//
// Listener management:
//

double AR_CALL AudioListenerCreate ( void )
{
	audio::Listener* listenerRef = new audio::Listener();
	return HandleFromReference(listenerRef);
}

double AR_CALL AudioListenerDestroy ( double listener )
{
	audio::Listener* listenerRef = ReferenceFromHandle<audio::Listener>(listener);
	listenerRef->Destroy();
	return NIL;
}

double AR_CALL AudioListenerSetPosition ( double listener, double x, double y, double z )
{
	audio::Listener* listenerRef = ReferenceFromHandle<audio::Listener>(listener);
	listenerRef->position = Vector3f((Real32)x, (Real32)y, (Real32)z);
	return NIL;
}

double AR_CALL AudioListenerSetVelocity ( double listener, double x, double y, double z )
{
	audio::Listener* listenerRef = ReferenceFromHandle<audio::Listener>(listener);
	listenerRef->velocity = Vector3f((Real32)x, (Real32)y, (Real32)z);
	return NIL;
}

double AR_CALL AudioListenerSetOrientation ( double listener, double x_forward, double y_forward, double z_forward, double x_up, double y_up, double z_up )
{
	audio::Listener* listenerRef = ReferenceFromHandle<audio::Listener>(listener);
	listenerRef->orient_forward = Vector3f((Real32)x_forward, (Real32)y_forward, (Real32)z_forward);
	listenerRef->orient_up = Vector3f((Real32)x_up, (Real32)y_up, (Real32)z_up);
	return NIL;
}


//
// Buffer management:
//

double AR_CALL AudioBufferLoad ( const char* filename )
{
	audio::Buffer* bufferRef = audio::BufferManager::Active()->GetSound(filename);
	bufferRef->AddReference();
	return HandleFromReference(bufferRef);
}

double AR_CALL AudioBufferFree ( double buffer )
{
	audio::Buffer* bufferRef = ReferenceFromHandle<audio::Buffer>(buffer);
	bufferRef->RemoveReference();
	return NIL;
}


//
// Source management:
//

double AR_CALL AudioSourceCreate ( double buffer )
{
	audio::Buffer* bufferRef = ReferenceFromHandle<audio::Buffer>(buffer);
	audio::Source* sourceRef = new audio::Source( bufferRef );
	return HandleFromReference(sourceRef);
}

double AR_CALL AudioSourceDestroy ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->Destroy();
	return NIL;
}

double AR_CALL AudioSourcePlay ( double source, double reset )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->Play(reset > 0.0);
	return NIL;
}

double AR_CALL AudioSourcePause ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->Pause();
	return NIL;
}

double AR_CALL AudioSourceStop ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->Stop();
	return NIL;
}

double AR_CALL AudioSourceIsPlaying ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	return BoolToDouble(sourceRef->IsPlaying());
}

double AR_CALL AudioSourcePlayed ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	return BoolToDouble(sourceRef->Played());
}

double AR_CALL AudioSourceSetPlaybackTime ( double source, double time )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->SetPlaybackTime( time );
	return NIL;
}

double AR_CALL AudioSourceGetPlaybackTime ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	return sourceRef->GetPlaybackTime();
}

double AR_CALL AudioSourceGetSoundLength ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	return sourceRef->GetSoundLength();
}

double AR_CALL AudioSourceGetCurrentMagnitude ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	return sourceRef->GetCurrentMagnitude();
}

double AR_CALL AudioSourceSetPosition ( double source, double x, double y, double z )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.position = Vector3f((Real32)x, (Real32)y, (Real32)z);
	return NIL;
}

double AR_CALL AudioSourceSetVelocity ( double source, double x, double y, double z )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.velocity = Vector3f((Real32)x, (Real32)y, (Real32)z);
	return NIL;
}

double AR_CALL AudioSourceSetLooped ( double source, double looped )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.looped = looped > 0.0;
	return NIL;
}

double AR_CALL AudioSourceSetPitch ( double source, double pitch )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.pitch = (float)pitch;
	return NIL;
}

double AR_CALL AudioSourceSetGain ( double source, double gain )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.gain = (float)gain;
	return NIL;
}

double AR_CALL AudioSourceSetSpatial ( double source, double spatial )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.spatial = (float)math::saturate(spatial);
	return NIL;
}

double AR_CALL AudioSourceSetChannel ( double source, double channel )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.channel = (audio::MixChannel)((uint)channel);
	return NIL;
}

double AR_CALL AudioSourceSetFalloff ( double source, double min_distance, double max_distance )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.min_dist = (float)min_distance;
	sourceRef->state.max_dist = (float)max_distance;
	return NIL;
}

double AR_CALL AudioSourceSetFalloffModel ( double source, double model, double falloff )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.falloffStyle = (audio::Falloff)((uint)model);
	sourceRef->state.falloff = (float)falloff;
	return NIL;
}

#endif//ONEFORTWO_ENABLED
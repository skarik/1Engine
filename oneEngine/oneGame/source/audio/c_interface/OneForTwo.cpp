#include "OneForTwo.h"

#if ONEFORTWO_ENABLED

#include "core/settings/CGameSettings.h"
#include "core-ext/threads/Jobs.h"
#include "core/debug/console.h"
#include "audio/Manager.h"
#include "audio/Buffer.h"
#include "audio/BufferManager.h"
#include "audio/Source.h"

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

double AR_CALL AudioSourceSetChannel ( double source, double channel )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	sourceRef->state.channel = (audio::MixChannel)((uint)channel);
	return NIL;
}


#endif//ONEFORTWO_ENABLED
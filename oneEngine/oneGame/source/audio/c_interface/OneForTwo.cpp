#include "OneForTwo.h"

#if ONEFORTWO_ENABLED

#include "core/settings/CGameSettings.h"
#include "core/debug/console.h"
#include "core/math/Math.h"
#include "core/os.h"

#include "core-ext/threads/Jobs.h"
#include "core-ext/system/shell/Message.h"

#include "audio/Manager.h"
#include "audio/Buffer.h"
#include "audio/BufferManager.h"
#include "audio/Source.h"
#include "audio/Listener.h"

#include <unordered_map>

// Settings system required for the job system settings
static CGameSettings*		m_settings = NULL;
// Job system required for mixer threads
static core::jobs::System*	m_jobSystem = NULL;
// Audio manager
static audio::Manager*		m_manager = NULL;
// Map of instantiated buffers, used for safety. TODO: This leaks a bit, consider clearing on buffer delete.
static std::unordered_map<uint64_t, audio::Buffer*>*
							m_refmapBuffers = NULL;
// Map of instantiated sources, used for safety. TODO: This leaks a bit, consider cleraing on source delete.
static std::unordered_map<uint64_t, audio::Source*>*
							m_refmapSources = NULL;

template <class Referenced>
static std::unordered_map<uint64_t, Referenced*>* GetHandleReferenceMap()
	{ return NULL; }

template <>
static std::unordered_map<uint64_t, audio::Buffer*>* GetHandleReferenceMap<audio::Buffer>()
{
	return m_refmapBuffers;
}

template <>
static std::unordered_map<uint64_t, audio::Source*>* GetHandleReferenceMap<audio::Source>()
{
	return m_refmapSources;
}


template <class Referenced>
static double HandleFromReference ( const Referenced* ref )
{
	// For now, we just do something INCREDIBLY unsafe, which is cast the pointer to an integer.
	// There could be some very big issues with this...
	uint64_t handle = (uint64_t)ref;

	auto referenceMap = GetHandleReferenceMap<Referenced>();
	if (referenceMap != NULL)
	{
		referenceMap->insert_or_assign((uint64_t)ref, (Referenced*)ref);
	}

	return (double)handle;
}

template <class Referenced>
static Referenced* ReferenceFromHandle ( const double handle )
{
	uint64_t intHandle = (uint64_t)handle;

	// Check if this is a valid handle to a correct instance type.
	// This prevents the extremely common case where passing in a buffer for a source or vice-versa
	auto referenceMap = GetHandleReferenceMap<Referenced>();
	if (referenceMap != NULL)
	{
		auto result = referenceMap->find(intHandle);
		if (result != referenceMap->end())
		{
			return result->second;
		}
	}

	return NULL;
}

static inline double BoolToDouble ( const bool boolean )
{
	return boolean ? 1.0 : 0.0;
}


double AR_CALL AudioInitialize ( double startupMask )
{
	uint32_t l_startupMask = (uint32_t)startupMask;

	// Load window settings
	m_settings = new CGameSettings(string(""), true);

	// Create jobs system first
	m_jobSystem = new core::jobs::System (4);

	// Create debug console
	debug::ConsoleWindow::Init(l_startupMask != 0);

	// Create the manager
	m_manager = new audio::Manager();

	// Create the refmaps
	m_refmapBuffers = new std::unordered_map<uint64_t, audio::Buffer*>;
	m_refmapSources = new std::unordered_map<uint64_t, audio::Source*>;

	return NIL;
}

double AR_CALL AudioFree ( void )
{
	delete m_manager;

	// Job system is removed last
	delete m_jobSystem;

	// Settings freed very last
	delete m_settings;

	// Free the refs now
	delete m_refmapBuffers;
	delete m_refmapSources;

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
	if (listenerRef != NULL)
	{
		listenerRef->Destroy();
	}
	return NIL;
}

double AR_CALL AudioListenerSetPosition ( double listener, double x, double y, double z )
{
	audio::Listener* listenerRef = ReferenceFromHandle<audio::Listener>(listener);
	if (listenerRef != NULL)
	{
		listenerRef->position = Vector3f((Real32)x, (Real32)y, (Real32)z);
	}
	return NIL;
}

double AR_CALL AudioListenerSetVelocity ( double listener, double x, double y, double z )
{
	audio::Listener* listenerRef = ReferenceFromHandle<audio::Listener>(listener);
	if (listenerRef != NULL)
	{
		listenerRef->velocity = Vector3f((Real32)x, (Real32)y, (Real32)z);
	}
	return NIL;
}

double AR_CALL AudioListenerSetOrientation ( double listener, double x_forward, double y_forward, double z_forward, double x_up, double y_up, double z_up )
{
	audio::Listener* listenerRef = ReferenceFromHandle<audio::Listener>(listener);
	if (listenerRef != NULL)
	{
		listenerRef->orient_forward = Vector3f((Real32)x_forward, (Real32)y_forward, (Real32)z_forward);
		listenerRef->orient_up = Vector3f((Real32)x_up, (Real32)y_up, (Real32)z_up);
	}
	return NIL;
}


//
// Buffer management:
//

double AR_CALL AudioBufferLoad ( const char* filename )
{
	audio::Buffer* bufferRef = audio::BufferManager::Active()->GetSound(filename);
	if (bufferRef != NULL)
	{
		bufferRef->AddReference();
	}
	return HandleFromReference(bufferRef);
}

double AR_CALL AudioBufferFree ( double buffer )
{
	audio::Buffer* bufferRef = ReferenceFromHandle<audio::Buffer>(buffer);
	if (bufferRef != NULL)
	{
		bufferRef->RemoveReference();
	}
	return NIL;
}

double AR_CALL AudioBufferGetLength ( double buffer )
{
	audio::Buffer* bufferRef = ReferenceFromHandle<audio::Buffer>(buffer);
	if (bufferRef != NULL)
	{
		return bufferRef->GetLength();
	}
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
	if (sourceRef != NULL)
	{
		sourceRef->Destroy();
	}
	return NIL;
}

double AR_CALL AudioSourcePlay ( double source, double reset )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->Play(reset > 0.0);
	}
	return NIL;
}

double AR_CALL AudioSourcePause ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->Pause();
	}
	return NIL;
}

double AR_CALL AudioSourceStop ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->Stop();
	}
	return NIL;
}

double AR_CALL AudioSourceIsPlaying ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		return BoolToDouble(sourceRef->IsPlaying());
	}
	return NIL;
}

double AR_CALL AudioSourcePlayed ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		return BoolToDouble(sourceRef->Played());
	}
	return NIL;
}

double AR_CALL AudioSourceSetPlaybackTime ( double source, double time )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->SetPlaybackTime( time );
	}
	return NIL;
}

double AR_CALL AudioSourceGetPlaybackTime ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		return sourceRef->GetPlaybackTime();
	}
	return NIL;
}

double AR_CALL AudioSourceGetSoundLength ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		return sourceRef->GetSoundLength();
	}
	return NIL;
}

double AR_CALL AudioSourceGetCurrentMagnitude ( double source )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		return sourceRef->GetCurrentMagnitude();
	}
	return NIL;
}

double AR_CALL AudioSourceSetPosition ( double source, double x, double y, double z )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.position = Vector3f((Real32)x, (Real32)y, (Real32)z);
	}
	return NIL;
}

double AR_CALL AudioSourceSetVelocity ( double source, double x, double y, double z )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.velocity = Vector3f((Real32)x, (Real32)y, (Real32)z);
	}
	return NIL;
}

double AR_CALL AudioSourceSetLooped ( double source, double looped )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.looped = looped > 0.0;
	}
	return NIL;
}

double AR_CALL AudioSourceSetPitch ( double source, double pitch )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.pitch = (float)pitch;
	}
	return NIL;
}

double AR_CALL AudioSourceSetGain ( double source, double gain )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.gain = (float)gain;
	}
	return NIL;
}

double AR_CALL AudioSourceSetSpatial ( double source, double spatial )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.spatial = (float)math::saturate(spatial);
	}
	return NIL;
}

double AR_CALL AudioSourceSetChannel ( double source, double channel )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.channel = (audio::MixChannel)((uint)channel);
	}
	return NIL;
}

double AR_CALL AudioSourceSetFalloff ( double source, double min_distance, double max_distance )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.min_dist = (float)min_distance;
		sourceRef->state.max_dist = (float)max_distance;
	}
	return NIL;
}

double AR_CALL AudioSourceSetFalloffModel ( double source, double model, double falloff )
{
	audio::Source* sourceRef = ReferenceFromHandle<audio::Source>(source);
	if (sourceRef != NULL)
	{
		sourceRef->state.falloffStyle = (audio::Falloff)((uint)model);
		sourceRef->state.falloff = (float)falloff;
	}
	return NIL;
}

#endif//ONEFORTWO_ENABLED
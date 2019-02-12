#ifndef _RENDERER_PARTICLE_PARTICLE_SYSTEM_H_
#define _RENDERER_PARTICLE_PARTICLE_SYSTEM_H_

#include "core/types/types.h"
#include "renderer/logic/RrLogicObject.h"

#define PARTICLE_CONFIG_MAX_SIZE 256
#define PARTICLE_CONFIG_VERIFY(config) static_assert(sizeof(config) < PARTICLE_CONFIG_MAX_SIZE, "Configuration out of size range");
#define PARTICLE_UPDATE_FUNCTION(name) void name (char* particle_data, const size_t particle_stride, const size_t particle_count, const void* update_config)

namespace renderer
{
	namespace particle
	{
		typedef void (*emit_function)(char* particle_data, const size_t particle_stride, const size_t particle_count, const void* update_config);
		typedef void (*update_function)(char* particle_data, const size_t particle_stride, const size_t particle_count, const void* update_config);
		typedef void (*render_function)(char* particle_data, const size_t particle_stride, const size_t particle_count, const void* update_config);
	}
}

class RrParticleSystem : public RrLogicObject
{
public:
	RENDER_API explicit		RrParticleSystem ( void );
	RENDER_API				~RrParticleSystem ( void );

	//		SetParticleSize ( size ) : sets size of a single particle in floats (4 bytes)
	// sets the size of a particle in floats (4 bytes). actual size is input parameter * 4
	RENDER_API void			SetParticleSize ( const size_t size );

	//      AddEmit( function, config ) : adds emit function to list of functions to run
	// returns id of new emit entry
	RENDER_API uint8_t		AddEmit( renderer::particle::emit_function function, void* config );
	//      AddUpdate( function, config ) : adds update function to list of functions to run
	// returns id of new update entry
	RENDER_API uint8_t		AddUpdate( renderer::particle::update_function function, void* config );
	//      AddRender( function, config ) : adds render function to list of functions to run
	// returns id of new render entry
	RENDER_API uint8_t		AddRender( renderer::particle::render_function function, void* config );

	//      Finalize () : locks particle system layout and prepares it for rendering
	// none of the previous add* or set* functions may be used after this.
	// this must be called before the particle system can be used
	RENDER_API void			Finalize ( void );
	
protected:

	//	PreStep()
	// Executed before the renderer starts. Particle system GPU uploader.
	// Lags a frame behind to give a chance to upload.
	RENDER_API void			PreStep ( void ) override;

	//	PostStepSynchronus()
	// Executed after PostStep jobs requests are started.
	// Threaded particle system updater and mesher.
	RENDER_API void			PostStepSynchronus ( void ) override;

private:
	
	struct update_function_entry_t
	{
		uint8_t id;
		char config [PARTICLE_CONFIG_MAX_SIZE];
		renderer::particle::update_function function;
	};

	//	Raw particle information
	char*   particle_data;
	//	Size of particle structure
	// actual stride is particle_stride padded to a power of 2
	size_t  particle_stride;	

};

#endif//_RENDERER_PARTICLE_PARTICLE_SYSTEM_H_
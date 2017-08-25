
#ifndef _C_PARTICLE_UPDATER_H_
#define _C_PARTICLE_UPDATER_H_

// Include gamebehavior
//#include "CGameBehavior.h"
#include "renderer/logic/CLogicObject.h"
// Include particle emitter
#include "CParticleEmitter.h"
// Include particle modifier
#include "modifiers/CParticleModifier.h"

// Include Vector Array
#include <vector>

// Class Definition
class CParticleUpdater : public CLogicObject
{
	RegisterRenderBaseClass( "CParticleUpdater" );
	RegisterRenderClassName( "CParticleUpdater" );
public:
	RENDER_API explicit CParticleUpdater ( CParticleEmitter* );
	RENDER_API ~CParticleUpdater ( void );

	RENDER_API void PreStepSynchronus ( void ) override;

	RENDER_API virtual void	serialize ( Serializer &, const uint );

	// Adds a modfier to the particle updater. Particle updater takes ownership of this modifier.
	RENDER_API void AddModifier ( CParticleModifier* );
	RENDER_API void RemoveModifier ( CParticleModifier* );

public:
	CParticleEmitter* myEmitter;

protected:
	std::vector<CParticleModifier*>	vMyModifiers;
};

#endif
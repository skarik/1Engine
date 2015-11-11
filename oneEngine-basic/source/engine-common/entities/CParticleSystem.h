
#ifndef _C_PARTICLE_SYSTEM_H_
#define _C_PARTICLE_SYSTEM_H_

#include "engine/behavior/CExtendableGameObject.h"

class CParticleEmitter;

class CParticleSystem : public CExtendableGameObject
{
	ClassName( "CParticleSystem" );
public:
	ENGCOM_API explicit			CParticleSystem ( const string& s_ps, const bool b_hasMeshOverride=false );
	ENGCOM_API explicit			CParticleSystem ( const string& s_ps, const string& s_mat );
	ENGCOM_API 					~CParticleSystem ( void );

	CRenderableObject*	GetRenderable ( int index=0 )
	{
		if ( vpRComponents.empty() )
			return NULL;
		else
			return vpRComponents[index];
	}
	CParticleEmitter*	GetEmitter ( int index=0 )
	{
		if ( vpEmitters.empty() )
			return NULL;
		else
			return vpEmitters[index];
	}

	// Is this particle system emitting?
	bool	enabled;

	ENGCOM_API void				PostUpdate ( void );
protected:
	void				Init ( const string&, const bool );

	std::vector<CParticleEmitter*> vpEmitters;

public:
	bool	bAutoDestroy;
};

#endif
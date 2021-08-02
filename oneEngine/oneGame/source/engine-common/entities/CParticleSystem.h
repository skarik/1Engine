// Old class, no longer used.
/*#ifndef _C_PARTICLE_SYSTEM_H_
#define _C_PARTICLE_SYSTEM_H_

#include "engine/behavior/CGameObject.h"

class RrRenderObject;
class RrLogicObject;
class CParticleEmitter;

class CParticleSystem : public CGameObject
{
	ClassName( "CParticleSystem" );
public:
	ENGCOM_API explicit			CParticleSystem ( const string& s_ps, const bool b_hasMeshOverride=false );
	ENGCOM_API explicit			CParticleSystem ( const string& s_ps, const string& s_mat );
	ENGCOM_API 					~CParticleSystem ( void );

	RrRenderObject*	GetRenderable ( int index=0 )
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

protected:
	// List of the components
	std::vector<CGameBehavior*> vpComponents;
	std::vector<RrRenderObject*> vpRComponents;
	std::vector<RrLogicObject*> vpLComponents;
	std::vector<CCollider*> vpCComponents;
	std::vector<CParticleEmitter*> vpEmitters;

public:
	bool	bAutoDestroy;
};

#endif*/
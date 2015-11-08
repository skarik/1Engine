
#ifndef _C_PARTICLE_EMITTER_H_
#define _C_PARTICLE_EMITTER_H_

// Include gameobject
//#include "CGameObject.h"
#include "renderer/logic/CLogicObject.h"
// Include particle
#include "CParticle.h"	// Which includes all attributes

// Include Vector Arrays
#include <vector>

// Include all attributes 
/*#include "standard_types.h"
#include "floattype.h"
#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Color.h"
#include "RangeValue.h"*/

// Include saving and loading
class CBaseSerializer;
typedef CBaseSerializer Serializer;

// Updater and Renderer Prototype for friending
class CParticleUpdater;
class CParticleRenderer;
class CParticleRenderer_Animated;
class CParticleEditor;

// Class Definition
class CParticleEmitter : public CLogicObject
{
	RegisterRenderBaseClass( "CParticleEmitter" );
	RegisterRenderClassName( "CParticleEmitter" );
private:
	friend CParticleUpdater;
	friend CParticleRenderer;
	friend CParticleRenderer_Animated;

	friend CParticleEditor;

public:
	RENDER_API explicit		CParticleEmitter ( void );
	RENDER_API explicit		CParticleEmitter ( const string & );
	RENDER_API 				~CParticleEmitter ( void );
	
	RENDER_API void			PreStepSynchronus ( void ) override;

	// Emit particles
	RENDER_API virtual void	Emit ( void );

	RENDER_API unsigned int	GetParticleCount ( void );
	RENDER_API bool			HasEmitted ( void );

	// Load system from file
	RENDER_API void			LoadFromFile ( const string & );
	// Serialization
	RENDER_API virtual void	serialize ( Serializer &, const uint );

protected:
	
	RENDER_API void CreateParticle ( const Vector3d & );

	// Emitter states
	bool		bHasEmitted;
	ftype		fEmitCount;

	Vector3d	m_lastPosition;
	Vector3d	m_worldVelocity;

public:
	// Emitter Shape Data
	Vector3d	vEmitterSize;

protected:
	// Common Particle Emitter Data
	std::vector<CParticle>	vParticles;

public:
	bool				bOneShot;
	bool				bSimulateInWorldspace;
	bool				bSmoothEmitter;

	ftype				fVelocityScale;

	RangeValue<ftype>	rfParticlesSpawned;
	ftype				fMaxParticles;
	
	RangeValue<ftype>	rfLifetime;

	RangeValue<ftype>	rfStartSize;
	RangeValue<ftype>	rfEndSize;

	RangeValue<ftype>	rfStartAngle;
	RangeValue<ftype>	rfAngularVelocity;
	RangeValue<ftype>	rfAngularAcceleration;
	RangeValue<ftype>	rfAngularDamping;

	RangeValue<Vector3d>	rvVelocity;
	RangeValue<Vector3d>	rvAcceleration;
	RangeValue<Vector3d>	rvLinearDamping;

	std::vector<Color>		vcColors;
};

#endif

#ifndef _C_PARTICLE_RENDERER_ANIMATED_H_
#define _C_PARTICLE_RENDERER_ANIMATED_H_

// Include old renderer
#include "CParticleRenderer.h"

// Class Definition
class CParticleRenderer_Animated : public CParticleRenderer
{
	RegisterRenderClassName( "ParticleRenderer_Animated" );
public:
	RENDER_API CParticleRenderer_Animated ( CParticleEmitter* );
	RENDER_API ~CParticleRenderer_Animated ( void );

	RENDER_API virtual void	serialize ( Serializer &, const uint );

	ftype	iFrameCount;
	ftype	iHorizontalDivs;
	ftype	iVerticalDivs;
	ftype	fFramesPerSecond;
	
	bool	bStretchAnimationToLifetime;
	bool	bClampToFrameCount;

protected:
	virtual void CreateMesh ( uint32_t& out_vertCount, uint32_t& out_triCount );

};

#endif//_C_PARTICLE_RENDERER_ANIMATED_H_
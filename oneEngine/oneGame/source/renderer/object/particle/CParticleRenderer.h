
#ifndef _C_PARTICLE_RENDERER_H_
#define _C_PARTICLE_RENDERER_H_

// Include renderable object
#include "renderer/object/CRenderableObject.h"
// Include particle emitter
#include "renderer/logic/particle/CParticleEmitter.h"
// Include particle updater
#include "renderer/logic/particle/CParticleUpdater.h"

#include "core/types/ModelData.h"

// Class Definition
class CParticleRenderer : public CRenderableObject
{
	RegisterRenderClassName( "ParticleRenderer" );
public:
	RENDER_API CParticleRenderer ( CParticleEmitter* );
	RENDER_API ~CParticleRenderer ( void );

	RENDER_API bool BeginRender ( void );

	RENDER_API bool PreRender ( const char pass );
	RENDER_API bool Render ( const char pass );

	RENDER_API virtual void	serialize ( Serializer &, const uint );

	enum eParticleRenderMethod
	{
		P_FLAT_BILLBOARD,
		P_FLAT_BILLBOARD_PRECISE,
		P_SHADED_BILLBOARD,
		P_STRETCHED_BILLBOARD,
		P_AXIS_ALIGNED_Z,
		P_FLAT_TRAILS,
		P_POINTS
	};
	int	 iRenderMethod;

	ftype fR_SpeedScale;

public:
	CParticleEmitter* myEmitter;

protected:
	uint	hVertices;
	uint	hTriangles;

	uint	iTriangleCount;

	static CParticleVertex*		pVertices;
	static CModelTriangle*		pTriangles;
	static unsigned int			iRendererCount;

	virtual void CreateMesh ( uint32_t& out_vertCount, uint32_t& out_triCount );
};

#endif
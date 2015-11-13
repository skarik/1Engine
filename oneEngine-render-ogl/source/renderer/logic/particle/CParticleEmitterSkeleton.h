// Skeleton particle emitter. Emits particles along set of given bones.

#ifndef _C_PARTICLE_EMITTER_SKELETON_H_
#define _C_PARTICLE_EMITTER_SKELETON_H_

// Includes
#include "CParticleEmitter.h"
#include "core/math/random/random_vector.h"

class CSkinnedModel;
class skeletonBone_t;

// Class definition
class CParticleEmitterSkeleton : public CParticleEmitter
{
private:
	friend CParticleUpdater;
	friend CParticleRenderer;
	friend CParticleEditor;

public:
	RENDER_API explicit CParticleEmitterSkeleton ( void );
	RENDER_API explicit CParticleEmitterSkeleton ( const string & );

	RENDER_API void Emit ( void ) override;

	RENDER_API void AddBone ( skeletonBone_t* n_bone, CSkinnedModel* n_model );
	RENDER_API void RemoveBone ( skeletonBone_t* n_bone, CSkinnedModel* n_model );

	RENDER_API void AddSkeleton ( const CSkinnedModel* n_model );
private:
	//std::vector<std::pair<skeletonBone_t*,CSkinnedModel*>>	m_emitList;
	random_vector<std::pair<skeletonBone_t*,CSkinnedModel*>> m_emitList;
};

#endif
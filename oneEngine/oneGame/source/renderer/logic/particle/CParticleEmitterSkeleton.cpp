
#include "core/time/time.h"
#include "core/math/random/Random.h"

#include "physical/skeleton/skeletonBone.h"

#include "CParticleEmitterSkeleton.h"

#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"

// Constructor
CParticleEmitterSkeleton::CParticleEmitterSkeleton ( void )
	: CParticleEmitter()
{
	; // Nothing baby
}
/*
CParticleEmitterSkeleton::CParticleEmitterSkeleton ( const string & n_fn )
	: CParticleEmitter( n_fn )
{
	; // Nothin here either, babe
}
*/
void CParticleEmitterSkeleton::AddBone ( skeletonBone_t* n_bone, CSkinnedModel* n_model )
{
	double t_weight = 0.1;

	// Find all children, average the distance to them
	if ( !n_bone->transform.children.empty() )
	{
		for ( auto it = n_bone->transform.children.begin(); it != n_bone->transform.children.end(); ++it )
		{
			t_weight += (n_bone->transform.position - (*it)->position).magnitude();
		}
		t_weight /= n_bone->transform.children.size();
	}

	t_weight = std::max<double>( t_weight, 0.2 );
	m_emitList.push_back( std::pair<skeletonBone_t*,CSkinnedModel*>( n_bone, n_model ), t_weight );
}
void CParticleEmitterSkeleton::RemoveBone ( skeletonBone_t* n_bone, CSkinnedModel* n_model )
{
	/*auto findResult = std::find( m_emitList.begin(), m_emitList.end(), std::pair<skeletonBone_t*,CSkinnedModel*>( n_bone, n_model ) );
	if ( findResult != m_emitList.end() ) {
		m_emitList.erase( findResult );
	}*/
	m_emitList.find_and_erase( std::pair<skeletonBone_t*,CSkinnedModel*>( n_bone, n_model ) );
}

void CParticleEmitterSkeleton::AddSkeleton ( const CSkinnedModel* n_model )
{
	// Grab the model to use
	/*CSkinnedModel* model = (CSkinnedModel*)n_model;
	std::vector<skeletonBone_t*>* skellyList = model->GetSkeletonList();
	// LOop through the mesh's skeleton, and add it to the emit list
	for ( auto it = skellyList->begin(); it != skellyList->end(); ++it )
	{
		AddBone( *it, model );
	}*/
	throw core::NotYetImplementedException();
}


void CParticleEmitterSkeleton::Emit ( void )
{
	if ( m_emitList.empty() ) {
		CParticleEmitter::Emit();
		return;
	}

	if ( m_particle_count < m_particle_array_size )
	{
		if ( !bOneShot )
		{
			ftype fReleaseCount = 0;
			ftype fTargetReleaseCount = rfParticlesSpawned.GetRandom() * CTime::deltaTime;

			fEmitCount += fTargetReleaseCount;
			if ( fEmitCount >= 1.0f )
			{
				while ( fReleaseCount < fTargetReleaseCount )
				{
					fReleaseCount += 1.0f;

					// Start by selecting a random bone
					//int emitTarget = Random.Next() % m_emitList.size();
					auto emitTarget = m_emitList.get_random_value();
					skeletonBone_t*	bone	= emitTarget.first;
					CSkinnedModel*		model	= emitTarget.second;

					Vector3d targetPosition;
					Quaternion boneRotation;
					targetPosition = model->transform.rotation*bone->transform.position + model->transform.position;
					boneRotation = ( model->transform.rotation * bone->transform.rotation ).getQuaternion();
					// Add the emitter's properties
					targetPosition += Vector3d( Random.Range( -vEmitterSize.x, vEmitterSize.x ), Random.Range( -vEmitterSize.y, vEmitterSize.y ), Random.Range( -vEmitterSize.z, vEmitterSize.z ) );
					// Either way, the position gets added to the particle. We need to subtract the emitter's position
					targetPosition -= transform.position;

					CreateParticle ( targetPosition );
				}
				fEmitCount -= 1.0f;
			}
		}
		else
		{
			ftype fReleaseCount = 0;
			ftype fTargetReleaseCount = rfParticlesSpawned.GetRandom();

			while (( fReleaseCount < fTargetReleaseCount )&&( fReleaseCount < fMaxParticles ))
			{
				fReleaseCount += 1.0f;

				// Start by selecting a random bone
				auto emitTarget = m_emitList.get_random_value();
				skeletonBone_t*	bone	= emitTarget.first;
				CSkinnedModel*		model	= emitTarget.second;

				Vector3d targetPosition;
				Quaternion boneRotation;
				targetPosition = model->transform.rotation*bone->transform.position + model->transform.position;
				boneRotation = ( model->transform.rotation * bone->transform.rotation ).getQuaternion();
				// Add the emitter's properties
				targetPosition += Vector3d( Random.Range( -vEmitterSize.x, vEmitterSize.x ), Random.Range( -vEmitterSize.y, vEmitterSize.y ), Random.Range( -vEmitterSize.z, vEmitterSize.z ) );
				// Either way, the position gets added to the particle. We need to subtract the emitter's position
				targetPosition -= transform.position;

				CreateParticle ( targetPosition );
			}
			fEmitCount = 0.0f;
		}
	}
}
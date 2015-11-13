
#include "core/time/time.h"

/*#include "CCloudMaster.h"
#include "CWeatherCloud.h"*/

#include "renderer/camera/CCamera.h"

#include "CParticleEmitterCloud.h"


// Constructor
CParticleEmitterCloud::CParticleEmitterCloud ( void )
	: CParticleEmitter()
{
	current_cloud = 0;
}

void CParticleEmitterCloud::Emit ( void )
{
	throw Core::NotYetImplementedException();
	/*if ( CCloudMaster::pActive->cloudList.size() <= 0 )
		return;

	current_cloud = current_cloud % CCloudMaster::pActive->cloudList.size();

	if ( vParticles.size() < fMaxParticles )
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
					Vector3d targetPosition;
					//int cloudIndex = rand() % CCloudMaster::pActive->cloudList.size();
					int cloudIndex = (++current_cloud) % CCloudMaster::pActive->cloudList.size();
					CWeatherCloud* targetCloud = CCloudMaster::pActive->cloudList[cloudIndex];
					targetPosition = targetCloud->position + 
						Vector3d(	random_range( -targetCloud->size.x/2, targetCloud->size.x/2 ),
									random_range( -targetCloud->size.y/2, targetCloud->size.y/2 ),
									random_range( -targetCloud->size.z/2, targetCloud->size.z/2 ) );
					
					Vector3d posToCamera = (targetPosition-CCamera::activeCamera->transform.position);
					if ( posToCamera.sqrMagnitude() < sqr(CCamera::activeCamera->zFar)*0.82f )
					{
						//fReleaseCount += 1.0f;
						fReleaseCount += 34.64f / targetCloud->size.magnitude();
						CreateParticle ( targetPosition - transform.position );
						vParticles.back().fSize *= std::max<Real>( 0.1f, targetCloud->size.magnitude()/45.0f );
					}
					else
					{
						fReleaseCount += 0.05f;
					}
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
				Vector3d targetPosition;
				//int cloudIndex = rand() % CCloudMaster::pActive->cloudList.size();
				int cloudIndex = (++current_cloud) % CCloudMaster::pActive->cloudList.size();
				CWeatherCloud* targetCloud = CCloudMaster::pActive->cloudList[cloudIndex];
				targetPosition = targetCloud->position + 
					Vector3d(	random_range( -targetCloud->size.x/2, targetCloud->size.x/2 ),
								random_range( -targetCloud->size.y/2, targetCloud->size.y/2 ),
								random_range( -targetCloud->size.z/2, targetCloud->size.z/2 ) );
				
				Vector3d posToCamera = (targetPosition-CCamera::activeCamera->transform.position);
				if ( posToCamera.sqrMagnitude() < sqr(CCamera::activeCamera->zFar)*0.9f )
				{
					//fReleaseCount += 1.0f;
					fReleaseCount += 34.64f / targetCloud->size.magnitude();
					CreateParticle ( targetPosition - transform.position );
				}
				else
				{
					fReleaseCount += 0.05f;
				}
			}
			fEmitCount = 0.0f;
		}
	}*/
}
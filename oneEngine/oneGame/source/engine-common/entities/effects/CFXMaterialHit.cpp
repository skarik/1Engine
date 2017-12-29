#include "CFXMaterialHit.h"

#include "engine/audio/AudioInterface.h"
#include "engine-common/entities/CParticleSystem.h"
#include "renderer/logic/particle/CParticleEmitter.h"

CFXMaterialHit::CFXMaterialHit ( const physMaterial& hitMat, const RaycastHit& hitResult, const EHitType hitType )
	: CGameBehavior()
{
	arstring<128> particleSystemName;
	arstring<128> soundSystemName;
	arstring<64>  materialName; // ( "particle_dirt_chunk" );
	// Do behavior based on the input
	switch ( hitType )
	{
		case HT_HIT:
			particleSystemName = hitMat.pt_hit;
			soundSystemName = hitMat.snd_hit;
			switch ( (uint32_t)hitMat )
			{
			case 2:
			case 3:	materialName = "particle_dirt_chunk"; break;
			case 4:
			case 6: materialName = "particle_rock_chunk"; break;
			case 12:
			case 7: materialName = "particle_dirt_dust"; break;
			case 9: materialName = "particle_wood_chip"; break;
			case 13: materialName = "particle/dust_anim01"; break;
			}
			break;
		case HT_COLLIDE:
			particleSystemName = hitMat.pt_collide;
			soundSystemName = hitMat.snd_collide;
			switch ( (uint32_t)hitMat )
			{
			case 2:
			case 3:
			case 12:
			case 7: materialName = "particle_dirt_dust"; break;
			}
			break;
		case HT_STEP:
			particleSystemName = hitMat.pt_step;
			soundSystemName = hitMat.snd_step;
			switch ( (uint32_t)hitMat )
			{
			case 2:
			case 3:
			//case 12:
			case 7: materialName = "particle_dirt_dust"; break;
			case 12: materialName = "particle/dust_anim01"; break;
			}
			break;
			break;
	};

	// Create particle system
	if ( particleSystemName.length() > 0 )
	{
		// Create material hit with given particle effect and normal
		CParticleSystem* ps_hit_effect;
		ps_hit_effect = new CParticleSystem ( string(particleSystemName), string(materialName) );
		ps_hit_effect->transform.world.position = hitResult.hitPos;
		// Update velocity based on the hit normal
		if ( hitType == HT_HIT )
		{
			ps_hit_effect->GetEmitter()->rvVelocity.mMinVal += hitResult.hitNormal*3.2f;
			ps_hit_effect->GetEmitter()->rvVelocity.mMaxVal += hitResult.hitNormal*6.0f;
		}
		else if ( hitType == HT_COLLIDE )
		{
			ps_hit_effect->GetEmitter()->rvVelocity.mMinVal += hitResult.hitNormal*1.3f;
			ps_hit_effect->GetEmitter()->rvVelocity.mMaxVal += hitResult.hitNormal*3.3f;
		}
		else if ( hitType == HT_STEP )
		{
			ps_hit_effect->GetEmitter()->rvVelocity.mMinVal += hitResult.hitNormal*0.4f;
			ps_hit_effect->GetEmitter()->rvVelocity.mMaxVal += hitResult.hitNormal*0.8f;
		}
		// Update position and remove ref
		ps_hit_effect->PostUpdate();
		ps_hit_effect->bAutoDestroy = true;
		ps_hit_effect->RemoveReference();
	}

	// Play sound
	if ( soundSystemName.length() > 0 )
	{
		engine::Sound* snd_hit_effect = engine::Audio.PlaySound( soundSystemName.c_str() );
		if ( snd_hit_effect )
		{
			snd_hit_effect->position = hitResult.hitPos;
			snd_hit_effect->deleteWhenDone = true;
			snd_hit_effect->Update();
			snd_hit_effect->RemoveReference();
		}
	}

	DeleteObject( this ); // Delete this spawner immediately
}


void CFXMaterialHit::Update ( void ) {
	;; // Nothing
}
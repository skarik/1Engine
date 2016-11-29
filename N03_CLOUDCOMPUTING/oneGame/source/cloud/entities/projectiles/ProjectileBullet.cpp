
#include "ProjectileBullet.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/object/particle/CParticleRenderer_Animated.h"

ProjectileBullet::ProjectileBullet ( const Ray & direction, Real speed )
	:CProjectile( direction, speed, 0.05F )
{
	dDamage.amount = 20.0F;

	pTrailParticle = new CParticleSystem( "particlesystems/arrowtrail01.pcf" );
	pTrailParticle->transform.position = transform.position;
	pTrailParticle->transform.SetDirty();
	pTrailParticle->transform.LateUpdate();

	CParticleRenderer* renderer = (CParticleRenderer*)pTrailParticle->GetRenderable();
	if ( renderer != NULL )
	{
		renderer->GetMaterial()->loadFromFile("particle/sparks");
		renderer->GetMaterial()->setTexture( 0, new CTexture("textures/white.jpg") );
		renderer->GetMaterial()->m_diffuse = Color( 1.0F, 0.3F, 0.0F );
		renderer->GetMaterial()->passinfo[0].m_hint = RL_WORLD | RL_FOG;
		renderer->GetMaterial()->passinfo[0].b_depthmask = false;
	}
}
ProjectileBullet::~ProjectileBullet ( void )
{
	pTrailParticle->enabled = false;
	pTrailParticle->RemoveReference();
}

void ProjectileBullet::Update ( void )
{
	CProjectile::Update();
	pTrailParticle->transform.position = transform.position;
}

void ProjectileBullet::OnHit ( CGameObject* go, Item::HitType hittype )
{
	// Create hit effect
	if ( rhLastHit.hit )
	{
		CParticleSystem* ps = new CParticleSystem("particlesystems/spark.pcf");

		CParticleRenderer_Animated* renderer = (CParticleRenderer_Animated*)ps->GetRenderable();
		if ( renderer )
		{
			renderer->GetMaterial()->loadFromFile("particle/sparks");
			renderer->GetMaterial()->passinfo[0].m_hint = RL_WORLD | RL_FOG;
			renderer->GetMaterial()->passinfo[0].b_depthmask = false;

			// following is a hack. currently these values are not serialized properly (corrupted) so set them here to fix
			renderer->fFramesPerSecond = 30.0F;
			renderer->bStretchAnimationToLifetime = false;
			renderer->bClampToFrameCount = false;
			renderer->iFrameCount = 16;
			renderer->iHorizontalDivs = 4;
			renderer->iVerticalDivs = 4;
		}
		ps->transform.position = rhLastHit.hitPos;
		ps->transform.rotation = Quaternion::CreateRotationTo( Vector3d(1,0,0), rhLastHit.hitNormal );
		ps->PostUpdate(); ps->PostUpdate(); // Update position

		ps->RemoveReference();
	}

	// Call parent shit
	CProjectile::OnHit(go,hittype);
}

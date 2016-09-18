
#include "ProjectileMissile.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/object/particle/CParticleRenderer_Animated.h"

#include "core/math/random/Random.h"

ProjectileMissile::ProjectileMissile ( const Ray & direction, Real speed )
	:CProjectile( direction, speed, 0.5F )
{
	dDamage.amount = 150.0F;

	pTrailParticle = new CParticleSystem( "particlesystems/arrowtrail01.pcf" );
	pTrailParticle->transform.position = transform.position;
	pTrailParticle->transform.SetDirty();
	pTrailParticle->transform.LateUpdate();

	CParticleRenderer* renderer = (CParticleRenderer*)pTrailParticle->GetRenderable();
	renderer->GetMaterial()->loadFromFile("particle/sparks");
	renderer->GetMaterial()->setTexture( 0, new CTexture("textures/white.jpg") );
	renderer->GetMaterial()->m_diffuse = Color( 1.0F, 1.0F, 1.0F );
	renderer->GetMaterial()->passinfo[0].m_hint = RL_WORLD | RL_FOG;
	renderer->GetMaterial()->passinfo[0].b_depthmask = false;

	randomOffset = Random.PointOnUnitSphere();
}
ProjectileMissile::~ProjectileMissile ( void )
{
	pTrailParticle->enabled = false;
	pTrailParticle->RemoveReference();
}

void ProjectileMissile::Update ( void )
{
	// Turn over time
	Real speed = vVelocity.magnitude();
	vVelocity += randomOffset * Time::deltaTime * fStartSpeed * 0.75F;
	vVelocity = vVelocity.normal() * speed; // And make sure it's in the right position

	CProjectile::Update();
	pTrailParticle->transform.position = transform.position;
}

void ProjectileMissile::OnHit ( CGameObject* go, Item::HitType hittype )
{
	// Create hit effect
	if ( rhLastHit.hit )
	{
		CParticleSystem* ps = new CParticleSystem("particlesystems/spark.pcf");
		ps->transform.position = rhLastHit.hitPos;
		ps->transform.rotation = Quaternion::CreateRotationTo( Vector3d(1,0,0), rhLastHit.hitNormal );
		ps->RemoveReference();
	}

	// Call parent shit
	CProjectile::OnHit(go,hittype);
}

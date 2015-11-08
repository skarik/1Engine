
#include "ProjectileFiretoss.h"
#include "core/math/random/Random.h"

#include "renderer/light/CLight.h"

#include "engine-common/entities/CParticleSystem.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/material/glMaterial.h"
#include "engine/physics/raycast/Raycaster.h"

ProjectileFiretoss::ProjectileFiretoss ( const Ray & rnInRay, ftype fnInSpeed )
	: ProjectileMagicalBase( rnInRay, fnInSpeed, 0.8f )
{
	//pModel = new CModel( string(".res/models/world/pebble.FBX") );
	//transform.scale = Vector3d( 2.0f,2.0f,2.0f );
	pModel = new CModel( ".res/models/effects/magic_ball.fbx" );
	{
		glMaterial* newMat = new glMaterial;
		newMat->loadFromFile( "effects/spell_fireball" );
		pModel->SetMaterial( newMat );
		newMat->removeReference();
		pModel->transform.scale = Vector3d(1,1,1) * 2.2f;
	}

	//CParticleSystem
	//cout << "ADD PARTICLE SYSTEM HERE: PEB00";
	pTrailParticle = new CParticleSystem( ".res/particlesystems/spells/firetoss_projectile.pcf" );
	pTrailParticle->transform.position = transform.position;

	/*RaycastHit hitInfo;
	if ( Raytracer.Raycast( rStartDirection, 200.0f, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), mOwner ) ) {
		vTargetPosition = hitInfo.hitPos;
	}
	else {
		vTargetPosition = rStartDirection.pos + rStartDirection.dir.normal()*200.0f;
	}

	vVelocity = (rStartDirection.dir.normal() + Random.PointInUnitSphere()*0.5f ).normal() * fStartSpeed;
	vRandomRotation = Random.PointInUnitSphere()*80.0f + Random.PointOnUnitSphere()*120.0f;*/

	Vector3d vTargetPosition;
	RaycastHit hitInfo;
	if ( Raycaster.Raycast( rStartDirection, 200.0f, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE,0,31), mOwner ) ) {
		vTargetPosition = hitInfo.hitPos;
	}
	else {
		vTargetPosition = rStartDirection.pos + rStartDirection.dir.normal()*200.0f;
	}
	vVelocity = (vTargetPosition - transform.position).normal() * vVelocity.magnitude();

	vTargetVelocity = (vVelocity + Vector3d( 0,0,vVelocity.magnitude()*0.18f )).normal() * vVelocity.magnitude();

	pLight = new CLight();
	pLight->transform.position = transform.position;
	pLight->diffuseColor = Color( 1.0f, 0.6f, 0.15f );
	pLight->range = 11.0f;
	pLight->drawHalo = true;
}

ProjectileFiretoss::~ProjectileFiretoss ( void )
{
	delete_safe( pModel );
	delete_safe( pLight );

	pTrailParticle->enabled = false;
	pTrailParticle->RemoveReference();
}

void ProjectileFiretoss::Update ( void )
{
	CProjectile::Update();

	pTrailParticle->transform.position = transform.position;

	// Now slightly change direction of velocity
	//vVelocity = ( vVelocity.normal()+(vTargetPosition-transform.position).normal()*15.0f*Time::deltaTime ).normal() * vVelocity.magnitude();
	vVelocity = vTargetVelocity;
	vTargetVelocity = (vTargetVelocity - Vector3d(0,0,Time::deltaTime*24)).normal() * vTargetVelocity.magnitude();

	// Muck with the model
	pModel->transform.position = transform.position;
	//pModel->transform.rotation.Euler( pModel->transform.rotation.getEulerAngles() + (vRandomRotation*Time::deltaTime) );
	// Muck with the light
	pLight->transform.position = transform.position;
}

#include "after/states/debuffs/standard/DebuffBurningFlametoss.h"

void ProjectileFiretoss::OnHit ( CGameObject* pHitGameobject, Item::HitType nHitType )
{
	transform.position = rhLastHit.hitPos;

	// Only spawn pebble on chance (realisitcally, obliterates it)
/*	if ( Random.Next() % 4 == 0 )
	{
		if ( pHitGameobject->GetBaseClassName() != "CActor_Character" ) {
			ItemPebble* newBolt = new ItemPebble();
			newBolt->transform.Get( transform );
			newBolt->transform.SetDirty();
			newBolt->SetHoldState( CWeaponItem::Hover );
		}
	}*/
	if ( pHitGameobject->GetBaseClassName() == "CActor_Character" )
	{
		/*cout << "Projectile damage: " << dDamage.amount << " (d" << dDamage.type << ")" << endl;

		// If it is, then hurt it. Bah. HURT IT.
		CCharacter*	pCharacter = (CCharacter*)pHitObject;
		if ( mOwner ) mOwner->OnDealDamage( dDamage, pCharacter );
		pCharacter->OnDamaged( dDamage );*/
		CCharacter*	pCharacter = (CCharacter*)pHitGameobject;
		//pCharacter->AddBuff( 100 ); // burning debuff, see list
		pCharacter->AddBuff<DebuffBurningFlametoss>(); // burning debuff, see list
	}

	CProjectile::OnHit( pHitGameobject, nHitType );
}
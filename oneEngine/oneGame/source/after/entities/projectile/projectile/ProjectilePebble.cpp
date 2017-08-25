
#include "ProjectilePebble.h"
#include "after/entities/item/ammo/ItemPebble.h"
#include "core/math/random/Random.h"

#include "engine-common/entities/CParticleSystem.h"
#include "engine/physics/raycast/Raycaster.h"

ProjectilePebble::ProjectilePebble ( const Ray & rnInRay, ftype fnInSpeed )
	: CProjectile( rnInRay, fnInSpeed, 0.3f )
{
	pModel = new CModel( string("models/world/pebble.FBX") );
	//transform.scale = Vector3d( 2.0f,2.0f,2.0f );

	//CParticleSystem
	//cout << "ADD PARTICLE SYSTEM HERE: PEB00";
	pTrailParticle = new CParticleSystem( ".res/particlesystems/smoketrail01.pcf" );
	pTrailParticle->transform.position = transform.position;

	RaycastHit hitInfo;
	if ( Raycaster.Raycast( rStartDirection, 200.0f, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), mOwner ) ) {
		vTargetPosition = hitInfo.hitPos;
	}
	else {
		vTargetPosition = rStartDirection.pos + rStartDirection.dir.normal()*200.0f;
	}

	vVelocity = (rStartDirection.dir.normal() + Random.PointInUnitSphere()*0.5f ).normal() * fStartSpeed;
	vRandomRotation = Random.PointInUnitSphere()*80.0f + Random.PointOnUnitSphere()*120.0f;
}

ProjectilePebble::~ProjectilePebble ( void )
{
	if ( pModel )
		delete pModel;
	pModel = NULL;

	pTrailParticle->enabled = false;
	pTrailParticle->RemoveReference();
}

void ProjectilePebble::Update ( void )
{
	CProjectile::Update();

	pTrailParticle->transform.position = transform.position;

	// Now slightly change direction of velocity
	vVelocity = ( vVelocity.normal()+(vTargetPosition-transform.position).normal()*15.0f*Time::deltaTime ).normal() * vVelocity.magnitude();

	// Muck with the model
	pModel->transform.position = transform.position;
	pModel->transform.rotation.Euler( pModel->transform.rotation.getEulerAngles() + (vRandomRotation*Time::deltaTime) );
}

void ProjectilePebble::OnHit ( CGameObject* pHitGameobject, Item::HitType nHitType )
{
	transform.position = rhLastHit.hitPos;

	// Only spawn pebble on chance (realisitcally, obliterates it)
	if ( Random.Next() % 4 == 0 )
	{
		//if ( pHitGameobject->GetBaseClassName() != "CActor_Character" ) {
		if ( nHitType != Item::HIT_CHARACTER && nHitType != Item::HIT_ACTOR ) {
			ItemPebble* newBolt = new ItemPebble();
			newBolt->transform.Get( transform );
			newBolt->transform.SetDirty();
			newBolt->SetHoldState( Item::Hover );
		}
	}

	CProjectile::OnHit( pHitGameobject, nHitType );
}
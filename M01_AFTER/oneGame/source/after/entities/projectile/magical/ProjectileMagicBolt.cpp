
#include "ProjectileMagicBolt.h"
#include "core/math/random/Random.h"

#include "renderer/light/CLight.h"

#include "engine-common/entities/CParticleSystem.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/material/glMaterial.h"
#include "engine/physics/raycast/Raycaster.h"

ProjectileMagicBolt::ProjectileMagicBolt ( const Ray & rnInRay, ftype fnInSpeed )
	: ProjectileMagicalBase( rnInRay, fnInSpeed, 0.8f )
{
	//pModel = new CModel( string("models/world/pebble.FBX") );
	//transform.scale = Vector3d( 2.0f,2.0f,2.0f );
	pModel = new CModel( "models/effects/magic_ball.fbx" );
	{
		glMaterial* newMat = new glMaterial;
		newMat->loadFromFile( "effects/spell_fireball" );
		pModel->SetMaterial( newMat );
		newMat->removeReference();
		pModel->transform.scale = Vector3d(1,1,1) * 1.1f;
	}

	//CParticleSystem
	//cout << "ADD PARTICLE SYSTEM HERE: PEB00";
	pTrailParticle = new CParticleSystem( ".res/particlesystems/spells/magicbolt_projectile.pcf" );
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

	//vTargetVelocity = (vVelocity); //+ Vector3d( 0,0,vVelocity.magnitude()*0.18f )).normal() * vVelocity.magnitude();
	vTargetVelocity = (vVelocity + Random.PointInUnitSphere()*vVelocity.magnitude()*0.09f ).normal() * vVelocity.magnitude();

	pLight = new CLight();
	pLight->transform.position = transform.position;
	pLight->diffuseColor = Color( 1.0f, 1.0f, 1.0f );
	pLight->range = 4.0f;
	pLight->drawHalo = true;
}

ProjectileMagicBolt::~ProjectileMagicBolt ( void )
{
	delete_safe( pModel );
	delete_safe( pLight );

	pTrailParticle->enabled = false;
	pTrailParticle->RemoveReference();
}

#include "after/states/CRacialStats.h"
void ProjectileMagicBolt::Update ( void )
{
	CProjectile::Update();

	pTrailParticle->transform.position = transform.position;

	// Now slightly change direction of velocity (gravity drop)
	//vVelocity = ( vVelocity.normal()+(vTargetPosition-transform.position).normal()*15.0f*Time::deltaTime ).normal() * vVelocity.magnitude();
	vVelocity = vTargetVelocity;
	vTargetVelocity = (vTargetVelocity - Vector3d(0,0,Time::deltaTime*4)).normal() * vTargetVelocity.magnitude();

	// Muck with the model
	pModel->transform.position = transform.position;
	//pModel->transform.rotation.Euler( pModel->transform.rotation.getEulerAngles() + (vRandomRotation*Time::deltaTime) );
	// Muck with the light
	pLight->transform.position = transform.position;

	if ( mOwner && mOwner->IsCharacter() ) {
		CCharacter* mChar = (CCharacter*)mOwner;
		//Color baseColor = mChar->GetRacialStats()->cFocusColor;
		CRacialStats* race_stats = mChar->GetRacialStats();
		if ( race_stats != NULL )
		{
			Color baseColor = race_stats->cFocusColor;	// STRENGTHEN THE COLOR
			ftype centerPoint = (baseColor.red*0.299f + baseColor.green*0.587f + baseColor.blue*0.114f) * 0.5f + 0.25f;
			baseColor.red += (baseColor.red - centerPoint)*3.0f;
			baseColor.green += (baseColor.green - centerPoint)*3.0f;
			baseColor.blue += (baseColor.blue - centerPoint)*3.0f;
			baseColor.alpha = 1.0f;
			baseColor = baseColor - Color( 0.5,0.5,0.5,0 );
			pLight->diffuseColor = baseColor;
		}
	}
}

void ProjectileMagicBolt::OnHit ( CGameObject* pHitGameobject, Item::HitType nHitType )
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
		//CCharacter*	pCharacter = (CCharacter*)pHitGameobject;
		//pCharacter->AddBuff( 100 ); // burning debuff, see list
	}

	CProjectile::OnHit( pHitGameobject, nHitType );
}
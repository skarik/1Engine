
#include "ProjectileFrostball.h"
#include "core/math/random/Random.h"

#include "renderer/light/CLight.h"

#include "engine-common/entities/CParticleSystem.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/material/glMaterial.h"
#include "engine/physics/raycast/Raycaster.h"

ProjectileFrostball::ProjectileFrostball ( const Ray & rnInRay, ftype fnInSpeed )
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

	Vector3d vTargetPosition;
	RaycastHit hitInfo;
	if ( Raycaster.Raycast( rStartDirection, 200.0f, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE,0,31), mOwner ) ) {
		vTargetPosition = hitInfo.hitPos;
	}
	else {
		vTargetPosition = rStartDirection.pos + rStartDirection.dir.normal()*200.0f;
	}
	vVelocity = (vTargetPosition - transform.position).normal() * vVelocity.magnitude();
	vTargetVelocity = (vVelocity + Random.PointInUnitSphere()*vVelocity.magnitude()*0.01f ).normal() * vVelocity.magnitude();

	pLight = new CLight();
	pLight->transform.position = transform.position;
	pLight->diffuseColor = Color( 0.4f, 0.45f, 0.6f );
	pLight->range = 11.0f;
	pLight->drawHalo = true;
}

ProjectileFrostball::~ProjectileFrostball ( void )
{
	delete_safe( pModel );
	delete_safe( pLight );

	pTrailParticle->enabled = false;
	pTrailParticle->RemoveReference();
}

void ProjectileFrostball::Update ( void )
{
	CProjectile::Update();

	pTrailParticle->transform.position = transform.position;

	// Now slightly change direction of velocity
	//vVelocity = ( vVelocity.normal()+(vTargetPosition-transform.position).normal()*15.0f*Time::deltaTime ).normal() * vVelocity.magnitude();
	vVelocity = vTargetVelocity;
	vTargetVelocity = (vTargetVelocity - Vector3d(0,0,Time::deltaTime*1)).normal() * vTargetVelocity.magnitude();

	// Muck with the model
	pModel->transform.position = transform.position;
	//pModel->transform.rotation.Euler( pModel->transform.rotation.getEulerAngles() + (vRandomRotation*Time::deltaTime) );
	// Muck with the light
	pLight->transform.position = transform.position;
}

#include "after/states/debuffs/standard/DebuffSlowBurnFrostball.h"

void ProjectileFrostball::OnHit ( CGameObject* pHitGameobject, Item::HitType nHitType )
{
	transform.position = rhLastHit.hitPos;

	if ( pHitGameobject->GetBaseClassName() == "CActor_Character" )
	{
		// If it is, then hurt it. Bah. HURT IT.
		CCharacter*	pCharacter = (CCharacter*)pHitGameobject;
		//pCharacter->AddBuff( 101 ); // slowburn debuff, see list
		pCharacter->AddBuff<DebuffSlowBurnFrostball>(); // slowburn debuff, see list
	}

	CProjectile::OnHit( pHitGameobject, nHitType );
}
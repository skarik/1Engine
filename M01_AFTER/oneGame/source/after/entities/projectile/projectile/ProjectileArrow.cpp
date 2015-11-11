
#include "ProjectileArrow.h"
#include "after/entities/item/ammo/AmmoArrow.h"

#include "engine-common/entities/CParticleSystem.h"

#include "renderer/logic/model/CModel.h"


ProjectileArrow::ProjectileArrow ( const Ray & rnInRay, ftype fnInSpeed )
	: AfterBaseProjectile( rnInRay, fnInSpeed )
{
	pModel = new CModel( string("models/weapons/cb_arrow_beta.FBX") );
	//transform.scale = Vector3d( 2.0f,2.0f,2.0f );

	//CParticleSystem
	//cout << "ADD PARTICLE SYSTEM HERE: HFX32";
	pTrailParticle = new CParticleSystem( ".res/particlesystems/arrowtrail01.pcf" );
	pTrailParticle->transform.position = transform.position;
	pTrailParticle->transform.SetDirty();
	pTrailParticle->transform.LateUpdate();
}

ProjectileArrow::~ProjectileArrow ( void )
{
	if ( pModel )
		delete pModel;
	pModel = NULL;

	pTrailParticle->enabled = false;
	pTrailParticle->RemoveReference();
}

void ProjectileArrow::Update ( void )
{
	CProjectile::Update();

	pModel->transform.Get( transform );

	pTrailParticle->transform.position = transform.position;
}

void ProjectileArrow::OnHit ( CGameObject* pHitGameobject, Item::HitType nHitType )
{
	transform.position = rhLastHit.hitPos; // No longer works with transform.Get (need to update matricies first)

	//if ( pHitGameobject->GetBaseClassName() != "CActor_Character" ) {
	if ( nHitType != Item::HIT_CHARACTER && nHitType != Item::HIT_ACTOR ) {
		AmmoArrow* newBolt = new AmmoArrow();
		//newBolt->transform.Get( transform );
		newBolt->transform.position = transform.position;
		newBolt->transform.rotation = transform.rotation;
		newBolt->transform.scale	= transform.scale;
		newBolt->transform.SetDirty();
		newBolt->SetHoldState( Item::Hover );
	}

	CProjectile::OnHit( pHitGameobject, nHitType );
}
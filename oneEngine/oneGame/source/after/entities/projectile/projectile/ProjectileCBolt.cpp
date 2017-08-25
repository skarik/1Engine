
#include "ProjectileCBolt.h"
#include "after/entities/item/ammo/AmmoCrossbowBolt.h"
#include "renderer/logic/model/CModel.h"

ProjectileCBolt::ProjectileCBolt ( const Ray & rnInRay, ftype fnInSpeed )
	: AfterBaseProjectile( rnInRay, fnInSpeed )
{
	pModel = new CModel( string("models\\weapons\\cb_bolt_beta.FBX") );
	//transform.scale = Vector3d( 2.0f,2.0f,2.0f );
}

ProjectileCBolt::~ProjectileCBolt ( void )
{
	if ( pModel )
		delete pModel;
	pModel = NULL;
}

void ProjectileCBolt::Update ( void )
{
	CProjectile::Update();

	pModel->transform.Get( transform );
}

void ProjectileCBolt::OnHit ( CGameObject* pHitGameobject, Item::HitType hittype )
{
	transform.position = rhLastHit.hitPos;

	AmmoCrossbowBolt* newBolt = new AmmoCrossbowBolt();
	newBolt->transform.Get( transform );
	newBolt->transform.SetDirty();
	newBolt->SetHoldState( Item::Hover );

	CProjectile::OnHit( pHitGameobject, hittype );
}
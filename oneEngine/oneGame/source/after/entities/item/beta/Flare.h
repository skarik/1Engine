#ifndef _FLARE_H_
#define _FLARE_H_

#include "after/entities/item/CWeaponItem.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"
#include "engine/physics/motion/CRigidbody.h"
#include "renderer/light/CLight.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/logic/model/CModel.h"

class Flare : public CWeaponItem
{
	ClassName( "Flare" );
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;

		wpdata.iItemId = 1;

		wpdata.sInfo = "Flare";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 50;
		wpdata.fWeight = 2.0f;

		return wpdata;
	}
public:
	// == Constructor ==
	Flare ( void ) : CWeaponItem( ItemData() )
	{
		pModel = new CModel ( string("models/flare01.FBX") );
		flareMaterial = new glMaterial;
		flareMaterial->m_emissive = Color( 0,0,0.0f );
		flareMaterial->m_emissive = Color( 1,1,1.0f );
		flareMaterial->setTexture( 0, new CTexture(".res/textures/flare01.tga") );
		pModel->SetMaterial( flareMaterial );
		//flareMaterial->releaseOwnership();

		pCollider = new CCapsuleCollider( 1.2f,0.2f, true );

		iCurrentState = 0;

		pLight = NULL;
	}
	// == Destructor ==
	~Flare ( void )
	{
		flareMaterial->removeReference();
		if ( pLight )
			delete pLight;
	}
	
	// Update function
	void Update ( void )
	{
		if ( pLight )
		{
			pLight->transform.position = transform.position;
			pLight->diffuseColor = Color( (unsigned char)190, 50, 50 ) * random_range( 2.2f,2.4f );
			//pLight->attenuation		= Color( 0.01f,0,1,1 );
			pLight->range = 40.0f;
			pLight->falloff = 3.0f;
			pLight->pass = 0.2f;
			flareMaterial->m_emissive = Color( (unsigned char)190, 50, 50 ) * random_range( 0.6f, 0.8f );
		}
		if ( pBody )
		{
			pBody->SetPenetrationDepth( 0.01f );
			pBody->SetLinearDamping( 0.2f );
			pBody->SetAngularDamping( 0.35f );
		}
	}

	// Use function
	bool Use( int x )
	{
		if ( !CanUse( x ) )
			return true;
		if ( x == Item::UPrimary )
		{
			if( iCurrentState == 0 )
			{
				// Turn on
				iCurrentState = 1;

				pLight = new CLight();
				SetCooldown( x, 1.0f );
			}
			else if ( iCurrentState == 1 )
			{
				// Throw
				iCurrentState = 2;

				SetCooldown( x, 1.0f );
				transform.position += pOwner->GetEyeRay().dir * 0.5f;
				Toss( pOwner->GetEyeRay().dir * 210.0f );
			}
		}
		else if ( x == Item::USecondary )
		{
			if ( iCurrentState == 0 )
			{
				// Turn on
				pLight = new CLight();
			}
			// Throw it
			SetCooldown( x, 1.0f );
			iCurrentState = 2;
			transform.position += pOwner->GetEyeRay().dir * 0.5f;
			Toss( pOwner->GetEyeRay().dir * 210.0f );
		}
		return true;
	}

	/*void Update( void )
	{
		pModel->transform.position = this->transform.position;
	}*/
	

private:
	int iCurrentState;
	CLight* pLight;
	glMaterial* flareMaterial;
};

#endif
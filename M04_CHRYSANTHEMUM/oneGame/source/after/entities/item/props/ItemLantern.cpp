
#include "ItemLantern.h"
#include "after/entities/props/props/PropLantern.h"
#include "after/types/terrain/BlockTracker.h"

#include "renderer/light/CLight.h"
#include "engine-common/entities/CParticleSystem.h"

#include "engine/physics/raycast/Raycaster.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

#include "after/terrain/edit/CTerrainAccessor.h"

ItemLantern::ItemLantern ( void )
	: CWeaponItem( ItemData() )
{
	pModel = new CModel ( string(".res\\models\\props\\lantern.FBX") );
	//pModel->transform.scale = Vector3d( 1.3f,1.3f,1.3f );

	//pCollider = new CCapsuleCollider( 1.2f,0.2f, true );

	pFireSystem = NULL;
	pLight = NULL;

	holdType = Item::LightSourceMelee;

	//vHoldingOffset = Vector3d( -0.09f, 0.02f, 0.75f );
	vHoldingOffset = Vector3d( 0.0f, 0.0f, -0.00f );

	degradeTimer = 0;
}

ItemLantern::~ItemLantern ( void )
{
	if ( pLight )
		delete pLight;
	pLight = NULL;
	if ( pFireSystem )
		delete pFireSystem;
	pFireSystem = NULL;
}


// Update function
void ItemLantern::Update ( void )
{
	if (( pModel->GetVisibility() )&&( weaponItemState.iCurrentDurability > 0 ))
	{
		if ( !pLight )
		{
			pLight = new CLight();
		}
		if ( !pFireSystem )
		{
			pFireSystem = new CParticleSystem ( ".res/particlesystems/flame03.pcf" );
			glMaterial* mat = new glMaterial();
			mat->passinfo.push_back( glPass() );
			mat->passinfo[0].m_blend_mode = Renderer::BM_ADD;
			mat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
			mat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
			mat->setTexture( 0, new CTexture(".res/textures/particles/fluxflame1.png") );
			mat->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
			mat->removeReference();
			pFireSystem->GetRenderable()->SetMaterial( mat );
		}
		// Degrade the "durability" of the torch over time
		degradeTimer += Time::deltaTime;
		if ( degradeTimer >= 1.0f ) {
			weaponItemState.iCurrentDurability -= 1;
			degradeTimer = 0;
			/*if ( weaponItemState.iCurrentDurability <= 0 ) {	// Decrement stack when it runs out of time
				weaponItemState.iCurrentDurability = weaponItemData.iMaxDurability;
				weaponItemState.iCurrentStack -= 1;
			}*/
		}
	}
	else
	{
		if ( pLight ) {
			delete pLight;
			pLight = NULL;
		}
		if ( pFireSystem ) {
			pFireSystem->RemoveReference();
			delete pFireSystem;
			pFireSystem = NULL;
		}
	}

	if ( pLight )
	{
		pLight->transform.position = transform.position;
		pLight->diffuseColor = Color( 0.71f, 0.55f, 0.24f ) * random_range( 1.2f,1.3f );
		pLight->range = 40.0f * random_range( 1.2f,1.3f );
		pLight->falloff = 3.0f;
		pLight->pass = 0.2f;
		pLight->drawHalo = true;
	}
	if ( pFireSystem )
	{
		pFireSystem->transform.position = transform.position + !Quaternion(-transform.rotation.getEulerAngles())*Vector3d( 0,0,-0.41f );
	}
	/*if ( pBody )
	{
		pBody->SetPenetrationDepth( 0.01f );
		pBody->SetLinearDamping( 0.2f );
		pBody->SetAngularDamping( 0.35f );
	}*/
}

// Use function
bool ItemLantern::Use( int x )
{
	if ( x == Item::USecondary )
	{
		if ( !CanUse( x ) )
		{
			SetCooldown( x, Time::deltaTime*3.0f );
			return true;
		}
		SetCooldown( x, Time::deltaTime*3.0f );

		bool placed = false;

		// Remove cube that player is looking at.
		Ray viewRay = pOwner->GetEyeRay();
		RaycastHit		result;
		BlockTrackInfo	block;
		if ( Raycaster.Raycast( viewRay, 8.0f, &result, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), pOwner ) )
		{
			TerrainAccess.GetBlockAtPosition( result, block );
			if ( block.valid )
			{
				PropLantern* newComponent = new PropLantern( block );
				Vector3d targetPos = result.hitPos + result.hitNormal*0.5f;
				/*targetPos.x -= fmod( targetPos.x, 2 );
				targetPos.y -= fmod( targetPos.y, 2 );
				targetPos.z -= fmod( targetPos.z, 2 );*/
				targetPos.x = floor( targetPos.x / 2 ) * 2;
				targetPos.y = floor( targetPos.y / 2 ) * 2;
				targetPos.z = floor( targetPos.z / 2 ) * 2;
				targetPos += Vector3d( 1,1,1 );
				if ( result.hitNormal.z > 0.5f ) {
					targetPos.z -= 0.4f;
				}
				else if ( result.hitNormal.z < -0.5f ) {
					targetPos.z += 0.2f;
				}
				else
				{
					targetPos -= result.hitNormal * 0.5f;
				}
				newComponent->transform.position = targetPos;
				//AddToTerrain();
				placed = true;
			}
		}

		if ( placed )
		{
			weaponItemState.iCurrentStack -= 1;
			if ( weaponItemState.iCurrentStack == 0 )
			{
				//CGameState::pActive->DeleteObject( this );
				// Delete this object in the inventory
			}
		}
		/*if ( placed )
		{
			holdState = None;
			pOwner = NULL;
			if ( pBody == NULL )
				CreatePhysics();
		}*/

		return true;
	}
	else
	{
		return false;
	}
}
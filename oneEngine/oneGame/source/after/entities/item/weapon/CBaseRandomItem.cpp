//
//
// CBaseRandomItem
// 
// The base class for all randomized weapons and items in the game.
//
//


#include "CBaseRandomItem.h"
#include "engine/physics/collider/types/CBoxCollider.h"
#include "engine/physics/motion/CRigidbody.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/state/Settings.h"

// Constructor (set default values and set weapon data)
CBaseRandomItem::CBaseRandomItem ( const WItemData& wdat )
	: CWeaponItem( wdat )
{
	bGenerated = false;
}
// Destructor (free modellist)
CBaseRandomItem::~CBaseRandomItem ( void )
{
	// Reset the generation flag
	bGenerated = false;
	
	// Free all the models
	while ( !vModels.empty() )
	{
		CModel* model = vModels.back();
		delete model;
		vModels.pop_back();
	}
}
CWeaponItem& CBaseRandomItem::operator= ( const CWeaponItem * original )
{
	if ( !bGenerated ) {
		Generate();
	}
	return CWeaponItem::operator= ( original );
}

// Serializer
void CBaseRandomItem::serialize ( Serializer & ser, const uint ver )
{
	bGenerated = false;
	ser & part_info.brand;
	ser & part_info.componentMain;
	ser & part_info.componentSecondary;
	ser & part_info.component3;
	ser & part_info.component4;

	ser & ((uint16_t&)part_info.materialMain);
	ser & ((uint16_t&)part_info.materialSecondary);
	ser & ((uint16_t&)part_info.material3);
	ser & ((uint16_t&)part_info.material4);
	CWeaponItem::serialize(ser,ver);

	arstring<128> tempBrand = part_info.brand;
	char* tok = strtok(tempBrand," ");
	strcpy( part_info.brand, tok );
}

void CBaseRandomItem::CreatePhysics ( void )
{
	if ( pModel == NULL )
	{
		std::cout << "Problem in CBaseRandomItem (" << GetItemName() << ") mem(" << (void*)(this) << ") " << std::endl;
		std::cout << "Cannot create a physics object without a modellist attached!" << std::endl;
	}
	else
	{
		// Calculated bounding box
		BoundingBox lCalculatedBbox;
		if ( pModel != NULL )
			lCalculatedBbox = pModel->GetBoundingBox();
		else
			lCalculatedBbox = vModels[0]->GetBoundingBox();
		for ( uint32_t i = 0; i < vModels.size(); ++i )
		{
			lCalculatedBbox = lCalculatedBbox.Expand( vModels[0]->GetBoundingBox() );
		}

		// Create a box collider based on the model's bounding box,
		//  but only create it if there's no collider already
		if ( pCollider == NULL )
			pCollider = new CBoxCollider( lCalculatedBbox, Vector3d(0,0,0) );

		// Create a rigidbody based on the collider
		pBody = new CRigidBody( pCollider, this );

		// Update the body's position
		pBody->SetPosition( transform.position );
		//pBody->SetRotation( transform.rotation );
		transform.SetDirty();

		// Change the body's friction
		pBody->SetFriction( 1.3f );

		// Set the collision type
		pBody->SetQualityType( HK_COLLIDABLE_QUALITY_DEBRIS );

		// Change collision type to item
		pBody->SetCollisionLayer( Layers::PHYS_ITEM );
	}
}


// Generates item if the item hasn't been created yet
void CBaseRandomItem::Update ( void )
{
	if ( !bGenerated ) {
		Generate();
	}
}

// Post update updates the positions of the subparts
void CBaseRandomItem::PostUpdate ( void )
{
	if ( pModel )
	{
		if ( pModel->GetVisibility() )
		{
			// Update all the model stuff
			for ( std::vector<CModel*>::iterator it = vModels.begin(); it != vModels.end(); ++it )
			{
				CModel* model = *it;
				//model->visible = true;
				model->SetVisibility( true );
				//model->transform.Get( pModel->transform );
				model->transform.position = pModel->transform.position;
				model->transform.rotation = pModel->transform.rotation;
				model->transform.localPosition = pModel->transform.localPosition;
				model->transform.localRotation = pModel->transform.localRotation;
			}
		}
		else
		{
			// Update all the model stuff
			for ( std::vector<CModel*>::iterator it = vModels.begin(); it != vModels.end(); ++it )
			{
				//(*it)->visible = false;
				(*it)->SetVisibility( false );
			}
		}
	}
}

// Generate refreshes the models
void CBaseRandomItem::Generate ( void )
{
	// Free main model
	if ( pModel ) {
		delete pModel;
		pModel = NULL;
	}
	// Free all the models
	while ( !vModels.empty() )
	{
		CModel* model = vModels.back();
		delete model;
		vModels.pop_back();
	}

	bGenerated = true;
}

// Lookat calls
// Lookats create some outlines
void	CBaseRandomItem::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		if ( pModel )
		{
			//pModel->renderSettings.cOutlineColor = Color( 0.9f,0.9f,0.9f );
			//pModel->renderSettings.fOutlineWidth = 0.06f;
			for ( std::vector<CModel*>::iterator it = vModels.begin(); it != vModels.end(); ++it )
			{
				CModel* model = *it;
				//model->renderSettings.cOutlineColor = Color( 0.9f,0.9f,0.9f );
				//model->renderSettings.fOutlineWidth = 0.06f;
			}
		}
	}
}
void	CBaseRandomItem::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		if ( pModel )
		{
			//pModel->renderSettings.fOutlineWidth = -1.0f;
			for ( std::vector<CModel*>::iterator it = vModels.begin(); it != vModels.end(); ++it )
			{
				CModel* model = *it;
				//model->renderSettings.fOutlineWidth = -1.0f;
			}
		}
	}
}



// ================================================
// Icon rendering code
// ================================================
#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/state/CRenderState.h"
#include "core/system/Screen.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/light/CLight.h"


// Sets the transform to set up a camera
void CBaseRandomItem::SetInventoryCameraTransform ( XTransform & inTransform )
{
	//inTransform.rotation = Rotator( 90,0,0 ) * Rotator( -45,-45,0 );
	inTransform.rotation = Rotator( 90,0,-50+160 ) * Rotator( -45,-45,0 );
	// Start position at center of model bounding box
	if ( pModel ) {
		BoundingBox bbox = pModel->GetBoundingBox();
		for ( auto l_model = vModels.begin(); l_model != vModels.end(); ++l_model )
		{
			bbox.Expand( (*l_model)->GetBoundingBox() );
		}
		inTransform.position = bbox.GetCenterPoint();
	}
	else {
		inTransform.position = Vector3d(0,0,0);
	}
}
void CBaseRandomItem::SetInventoryCameraWidth ( ftype & inWidth )
{
	if ( pModel ) {
		BoundingBox bbox = pModel->GetBoundingBox();
		for ( auto l_model = vModels.begin(); l_model != vModels.end(); ++l_model )
		{
			bbox.Expand( (*l_model)->GetBoundingBox() );
		}
		Vector3d bbox_size = bbox.GetSize();
		inWidth = std::max<ftype>( bbox_size.x, bbox_size.y );
		inWidth = std::max<ftype>( inWidth, bbox_size.z );
		inWidth *= 0.577f;
	}
	else {
		inWidth = 1.5f;
	}
}

void CBaseRandomItem::InventoryIconRenderModel ( void )
{
	if ( pModel )
	{
		pModel->SetVisibility( true );
		pModel->SetForcedDraw();

		pModel->transform.position = Vector3d( 0,0,0 );
		pModel->transform.rotation = Vector3d( 0,0,0 );
		pModel->transform.scale = Vector3d( 1,1,1 );

		// Create light list
		static CLight* light = NULL;
		if ( light == NULL ) { 
			light = new CLight();
			light->transform.position = Vector3d( 3,3,3 );
			light->diffuseColor = Color( 0.8f, 0.8f, 0.8f );
			light->range = 20.0f;
			light->falloff = 3.0f;
		}
		light->SetActive( true );
		std::vector<CLight*> lightList;
		lightList.push_back( light );

		// Set render settings
		Renderer::renderSettings_t oldRenderSettings = Renderer::Settings;
		Renderer::Settings.ambientColor = Color( 0.6f,0.6f,0.6f );
		Renderer::Settings.fogStart = 10.0f;
		Renderer::Settings.fogEnd = 100.0f;
		Renderer::Settings.fogColor = Color( 1.0f,1.0f,1.0f );
		Renderer::Settings.clearColor = Color( 0.7f,0.7f,0.8f,0.0f );

		// Render item
		SceneRenderer->PreRenderSetLighting( lightList );
		pModel->SetForcedDraw();
		for ( uint i = 0; i < pModel->GetMeshCount(); ++i ) {
			SceneRenderer->RenderSingleObject( (CRenderableObject*)pModel->GetMesh(i) );
		}
		for ( auto l_model = vModels.begin(); l_model != vModels.end(); ++l_model )
		{
			(*l_model)->SetForcedDraw();
			for ( uint i = 0; i < (*l_model)->GetMeshCount(); ++i ) {
				SceneRenderer->RenderSingleObject( (CRenderableObject*)(*l_model)->GetMesh(i) );
			}
		}

		// Clear light
		light->SetActive( false );

		// Reset render setings
		Renderer::Settings = oldRenderSettings;
		GLd_ACCESS;
		GLd.BeginPrimitive( GL_POINTS );
		// Loop through all the particles and render
		for ( float x = 0; x < 3; x+=0.05f )
		{
			GLd.P_PushColor( 1.0f,1.0f, 0.0f, 1.0f );
			GLd.P_AddVertex( x,0,0 );
			GLd.P_AddVertex( 0,x,0 );
			GLd.P_AddVertex( 0,0,x );
		}
		GLd.EndPrimitive();
	}
}
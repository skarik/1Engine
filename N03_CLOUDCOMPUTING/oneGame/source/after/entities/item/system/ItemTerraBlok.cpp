
// Includes
#include "ItemTerraBlok.h"
#include "core/time/time.h"
#include "engine/physics/raycast/Raycaster.h"
//#include "CCamera.h" //using for debugging
//#include "CDebugDrawer.h"
//#include "CVoxelTerrain.h"
//#include "COctreeTerrain.h"
//#include "COctreeRenderer.h"
#include "after/entities/character/CCharacter.h"
#include "renderer/material/glMaterial.h"
#include "renderer/logic/model/CModel.h"
#include "after/types/terrain/BlockType.h"
#include "after/types/terrain/BlockTracker.h"
#include "engine/physics/motion/CRigidbody.h"
#include "after/physics/Caster.h"
#include "after/types/terrain/DirectionFlags.h"
#include "after/terrain/VoxelTerrain.h"

// == Constructor ==
ItemTerraBlok::ItemTerraBlok ( glMaterial* p_mat, ushort i_ebblock )
	: blokType( i_ebblock ), CWeaponItem( ItemData() )
{
	blokMaterial= p_mat;
	//blokMaterial->useVertexColors = true;
	GenerateMaterial();
	blokType	= i_ebblock;

	pCollider	= new CBoxCollider( Vector3d( 1.1f,1.1f,1.1f ) );
	
	blokRender = NULL;
	GenerateModel();

	fHoverTimer = 3.0f;

	// Set the name based on the type
	weaponItemData.sInfo = Terrain::blockName[i_ebblock];
	weaponItemState.sItemName = weaponItemData.sInfo;
}
void ItemTerraBlok::GenerateMaterial ( void )
{
	blokMaterial = glMaterial::Default;
	/*
	if ( blokMaterial == NULL ) {
		if ( CVoxelTerrain::GetActive() ) {
			blokMaterial = CVoxelTerrain::GetActive()->GetBlockMaterial();
		}
		else if ( COctreeTerrain::GetActive() ) {
			blokMaterial = COctreeTerrain::GetActive()->GetRenderer()->GetMaterial();
		}
	}*/
}
void ItemTerraBlok::GenerateModel ( void )
{
	if ( blokRender ) {
		delete blokRender;
		blokRender = NULL;
	}

	CModelData md;
	md.vertices = SetVertices();
	md.vertexNum = 24;
	md.triangleNum = 12;
	md.triangles = new CModelTriangle [12];
	for ( int i = 0; i < 12; i += 2 ) {
		md.triangles[i+0].vert[0] = i*2+0;
		md.triangles[i+0].vert[1] = i*2+1;
		md.triangles[i+0].vert[2] = i*2+2;
		md.triangles[i+1].vert[0] = i*2+2;
		md.triangles[i+1].vert[1] = i*2+3;
		md.triangles[i+1].vert[2] = i*2+0;
	}
	blokRender	= new CModel( md, "_sys_override_" );
	blokRender->SetMaterial( blokMaterial );
}


// == Destructor ==
ItemTerraBlok::~ItemTerraBlok ( void )
{
	if ( pCollider )
		delete pCollider;
	pCollider = NULL;
	if ( blokRender )
		delete blokRender;
	blokRender = NULL;
}
// == Copier ==
ItemTerraBlok::ItemTerraBlok ( void ) : CWeaponItem( ItemData() )
{
	blokMaterial= NULL;
	GenerateMaterial();
	blokType	= Terrain::EB_DIRT;

	pCollider	= new CBoxCollider( Vector3d( 1.1f,1.1f,1.1f ) );

	blokRender = NULL;
	GenerateModel();

	fHoverTimer = 3.0f;
}
CWeaponItem& ItemTerraBlok::operator= ( const CWeaponItem * original )
{
	std::cout << "EQU" << std::endl;
	ItemTerraBlok* oitem = (ItemTerraBlok*)original;
	blokMaterial	= oitem->blokMaterial;
	blokType		= oitem->blokType;

	fHoverTimer		= oitem->fHoverTimer;

	delete_safe( blokRender );
	GenerateModel();
	/*if ( blokRender ) {
		blokRender->SetMaterial( blokMaterial );
	}*/

	// Set the name based on the type
	weaponItemData.sInfo = Terrain::blockName[blokType];
	weaponItemState.sItemName = weaponItemData.sInfo;

	return CWeaponItem::operator=( original );
}

// Comparison
bool ItemTerraBlok::IsEqual ( CWeaponItem* compare )
{
	if ( CWeaponItem::IsEqual( compare ) ) {
		return ( blokType == ((ItemTerraBlok*)compare)->blokType );
	}
	return false;
}

// Serialization
void ItemTerraBlok::serialize ( Serializer & ser, const uint ver )
{
	CWeaponItem::serialize( ser,ver );
	ser & blokType;
	SetType(blokType);
}

// Lookats create some outlines
void	ItemTerraBlok::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//blokRender->renderSettings.cOutlineColor = Color( 1.0f,1,1 );
		//blokRender->renderSettings.fOutlineWidth = 0.03f;
	}
}
void	ItemTerraBlok::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//blokRender->renderSettings.fOutlineWidth = -1.0f;
	}
}

// Initialize the physics object
void ItemTerraBlok::CreatePhysics ( void )
{
	fHoverTimer = 3.0f;

	// Create a rigidbody based on the collider
	pBody = new CRigidBody( pCollider, this );

	// Update the body's position
	pBody->SetPosition( transform.position );
	//pBody->SetRotation( transform.rotation );
	transform.SetDirty();

	// Change the body's friction
	pBody->SetFriction( 1.6f );

	// Change the body's motion type
	//pBody->SetQualityType( HK_COLLIDABLE_QUALITY_CRITICAL );
	pBody->SetQualityType( HK_COLLIDABLE_QUALITY_BULLET );
	pBody->SetLinearDamping( 0.8f );
	pBody->SetVelocity( Vector3d( 0,0,-30 ) );

	pBody->SetCollisionLayer( Layers::PHYS_ITEM );
	// Wait, I think that's it
	// hmmm, this is too simple....
}

// Update function
void ItemTerraBlok::Update ( void )
{
	//DebugD::DrawLine( transform.position, CCamera::activeCamera->transform.position + CCamera::activeCamera->transform.Forward() );
	//weaponItemState.iCurrentStack = 10.0f;
	if ( pBody )
	{
		if ( fHoverTimer > 1.0f )
		{
			fHoverTimer -= Time::smoothDeltaTime;
			if ( fHoverTimer > 2.5f )
				pBody->SetVelocity( Vector3d( 0,0,0 ) );
			pBody->SetLinearDamping( 0.8f );
		}
		else
		{
			pBody->SetQualityType( HK_COLLIDABLE_QUALITY_DEBRIS );
			pBody->SetLinearDamping( 0.1f );
		}
	}
}

// Lateupdate function
void ItemTerraBlok::LateUpdate ( void )
{
	// Call inherited lateUpdate
	CWeaponItem::LateUpdate();

	if ( !blokRender ) {
		GenerateModel();
	}

	// Update render object
	if ( blokRender )
	{
		blokRender->transform.Get( transform );
		if (( holdState == Item::None )||( holdState == Item::Hover ))
		{

		}
		else
		{
			blokRender->transform.scale = Vector3d( 0.7f,0.7f,0.7f );
		}
	}

	// Depending on the hold state, toggle the visibility of the model
	if ( blokRender )
	{
		switch ( holdState )
		{
			case Item::Hidden:
			case Item::SystemHidden:
				blokRender->SetVisibility( false );
			break;
			default:
				blokRender->SetVisibility( true );
			break;
		}
	}
}

// Use function
bool ItemTerraBlok::Use( int x )
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

		// Add to cube that player is looking at.
		Ray viewRay = pOwner->GetEyeRay();
		RaycastHit		result;
		BlockTrackInfo	block;
		CGameBehavior*	hitBehavior;
		//if ( Raytracer.Raycast( viewRay, 8.0f, &result, &block, 1|2|4 ) )
		Item::HitType hittype = Caster::Raycast( viewRay, 8.0f, &result, &block, &hitBehavior, NULL );
		if ( hittype == Item::HIT_TERRAIN )
		{
			// Now, do a check for other objects near position
			//CVoxelTerrain::terrainList[0]->SetBlock(block, 0);
			//CVoxelTerrain::terrainList[0]->ItemizeBlock(block);
			if ( CVoxelTerrain::GetActive() )
			{
				if ( TerrainAccess.SetBlockAtPosition( result.hitPos + result.hitNormal, blokType ) ) {
					placed = true;
				}
			}
			/*else if ( COctreeTerrain::GetActive() )
			{
				if ( COctreeTerrain::GetActive()->CSG_SetBlock( result.hitPos + result.hitNormal, blokType ) ) {
					placed = true;
				}
			}*/
		}

		if ( placed )
		{
			weaponItemState.iCurrentStack -= 1;
			if ( weaponItemState.iCurrentStack == 0 )
			{
				//CGameState::pActive->DeleteObject( this );
				// Delete this object in the inventory
			}

			if ( pOwner ) {
				// Play toss animation
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Toss, 0, mHand, 6 );

				// Give experience for building (small amount)
				if ( pOwner->layer & Layers::Character ) {
					//((CCharacter*)pOwner)->OnGainExperience( Experience(1,DscBuilding) );
					((CCharacter*)pOwner)->OnGainExperience( Experience(1,DscCrafting,SubDscBuilding) );
				}
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

// Set the object vertices
CModelVertex* ItemTerraBlok::SetVertices ( void )
{
	CModelVertex* verts = new CModelVertex [24];

	int ofs;
	// TOP
	ofs = 0;
	{
		verts[ofs+0].x = 0;
		verts[ofs+0].y = 0;
		verts[ofs+0].z = 1;//0
		verts[ofs+1].x = 1;
		verts[ofs+1].y = 0;
		verts[ofs+1].z = 1;//1
		verts[ofs+2].x = 1;
		verts[ofs+2].y = 1;
		verts[ofs+2].z = 1;//2

		/*verts[ofs+3].x = 1;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 1;//2 */
		verts[ofs+3].x = 0;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 1;//3
		/*verts[ofs+5].x = 0;
		verts[ofs+5].y = 0;
		verts[ofs+5].z = 1;//0 */

		for ( char i = 0; i < 4; i += 1 )
		{
			verts[ofs+i].nx = 0;
			verts[ofs+i].ny = 0;
			verts[ofs+i].nz = 1;
		}

		SetUVs( &(verts[ofs]), Terrain::TOP );
		SetColors( &(verts[ofs]) );
	}
	// BOTTOM
	ofs = 4;
	{
		verts[ofs+0].x = 0;
		verts[ofs+0].y = 0;
		verts[ofs+0].z = 0;//
		verts[ofs+1].x = 0;
		verts[ofs+1].y = 1;
		verts[ofs+1].z = 0;//
		verts[ofs+2].x = 1;
		verts[ofs+2].y = 1;
		verts[ofs+2].z = 0;//

		/*verts[ofs+3].x = 1;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 0;// */
		verts[ofs+3].x = 1;
		verts[ofs+3].y = 0;
		verts[ofs+3].z = 0;//
		/*verts[ofs+5].x = 0;
		verts[ofs+5].y = 0;
		verts[ofs+5].z = 0;// */

		for ( char i = 0; i < 4; i += 1 )
		{
			verts[ofs+i].nx = 0;
			verts[ofs+i].ny = 0;
			verts[ofs+i].nz = -1;
		}

		SetUVs( &(verts[ofs]), Terrain::BOTTOM );
		SetColors( &(verts[ofs]) );
	}
	// FRONT
	ofs = 8;
	{
		verts[ofs+0].x = 1;
		verts[ofs+0].y = 0;
		verts[ofs+0].z = 0;//
		verts[ofs+1].x = 1;
		verts[ofs+1].y = 1;
		verts[ofs+1].z = 0;//
		verts[ofs+2].x = 1;
		verts[ofs+2].y = 1;
		verts[ofs+2].z = 1;//

		/*verts[ofs+3].x = 1;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 1;// */
		verts[ofs+3].x = 1;
		verts[ofs+3].y = 0;
		verts[ofs+3].z = 1;//
		/*verts[ofs+5].x = 1;
		verts[ofs+5].y = 0;
		verts[ofs+5].z = 0;// */

		for ( char i = 0; i < 4; i += 1 )
		{
			verts[ofs+i].nx = 1;
			verts[ofs+i].ny = 0;
			verts[ofs+i].nz = 0;
		}

		SetUVs( &(verts[ofs]), Terrain::FRONT );
		SetColors( &(verts[ofs]) );
	}
	// BACK
	ofs = 12;
	{
		verts[ofs+0].x = 0;
		verts[ofs+0].y = 0;
		verts[ofs+0].z = 0;//
		verts[ofs+1].x = 0;
		verts[ofs+1].y = 0;
		verts[ofs+1].z = 1;//
		verts[ofs+2].x = 0;
		verts[ofs+2].y = 1;
		verts[ofs+2].z = 1;//

		/*verts[ofs+3].x = 0;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 1;// */
		verts[ofs+3].x = 0;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 0;//
		/*verts[ofs+5].x = 0;
		verts[ofs+5].y = 0;
		verts[ofs+5].z = 0;// */

		for ( char i = 0; i < 4; i += 1 )
		{
			verts[ofs+i].nx = -1;
			verts[ofs+i].ny = 0;
			verts[ofs+i].nz = 0;
		}

		SetUVs( &(verts[ofs]), Terrain::BACK );
		SetColors( &(verts[ofs]) );
	}
	// LEFT
	ofs = 16;
	{
		verts[ofs+0].x = 0;
		verts[ofs+0].y = 1;
		verts[ofs+0].z = 0;//
		verts[ofs+1].x = 0;
		verts[ofs+1].y = 1;
		verts[ofs+1].z = 1;//
		verts[ofs+2].x = 1;
		verts[ofs+2].y = 1;
		verts[ofs+2].z = 1;//

		/*verts[ofs+3].x = 1;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 1;// */
		verts[ofs+3].x = 1;
		verts[ofs+3].y = 1;
		verts[ofs+3].z = 0;//
		/*verts[ofs+5].x = 0;
		verts[ofs+5].y = 1;
		verts[ofs+5].z = 0;// */

		for ( char i = 0; i < 4; i += 1 )
		{
			verts[ofs+i].nx = 0;
			verts[ofs+i].ny = 1;
			verts[ofs+i].nz = 0;
		}

		SetUVs( &(verts[ofs]), Terrain::LEFT );
		SetColors( &(verts[ofs]) );
	}
	// RIGHT
	ofs = 20;
	{
		verts[ofs+0].x = 0;
		verts[ofs+0].y = 0;
		verts[ofs+0].z = 0;//
		verts[ofs+1].x = 1;
		verts[ofs+1].y = 0;
		verts[ofs+1].z = 0;//
		verts[ofs+2].x = 1;
		verts[ofs+2].y = 0;
		verts[ofs+2].z = 1;//

		/*verts[ofs+3].x = 1;
		verts[ofs+3].y = 0;
		verts[ofs+3].z = 1;// */
		verts[ofs+3].x = 0;
		verts[ofs+3].y = 0;
		verts[ofs+3].z = 1;//
		/*verts[ofs+5].x = 0;
		verts[ofs+5].y = 0;
		verts[ofs+5].z = 0;// */

		for ( char i = 0; i < 4; i += 1 )
		{
			verts[ofs+i].nx = 0;
			verts[ofs+i].ny = -1;
			verts[ofs+i].nz = 0;
		}

		SetUVs( &(verts[ofs]), Terrain::RIGHT );
		SetColors( &(verts[ofs]) );
	}

	for ( char i = 0; i < 24; i++ )
	{
		verts[i].x -= 0.5f;
		verts[i].y -= 0.5f;
		verts[i].z -= 0.5f;
	}

	//blokRender->SetVertices( verts, 36 );
	return verts;
}

void ItemTerraBlok::SetColors( CModelVertex* vertices )
{
	ushort data = blokType;

	float lightLevel = 1.0f;
	float reflectLevel = 0.0f;
	float satLevel = 1.0f;
	float invGlowLevel = 1.0f;

	if ( data == Terrain::EB_MUD ) {
		satLevel = 0.5f;
		reflectLevel = 0.6f;
	}
	else if ( data == Terrain::EB_CRYSTAL ) {
		reflectLevel = 0.8f;
	}
	else if ( data == Terrain::EB_XPLO_CRYSTAL ) {
		satLevel = 0.3f;
		reflectLevel = 0.8f;
	}
	else if ( data == Terrain::EB_ASH ) {
		satLevel = 0.1f;
	}
	else if ( data == Terrain::EB_SNOW || data == Terrain::EB_TOP_SNOW ) {
		satLevel = 0.0f;
		reflectLevel = 0.3f;
	}
	else if ( data == Terrain::EB_DEADSTONE ) {
		reflectLevel = 0.3f;
	}
	else if ( data == Terrain::EB_CURSED_DEADSTONE ) {
		reflectLevel = 0.3f;
		invGlowLevel = 0.0f;
	}
	else if ( data == Terrain::EB_ICE ) {
		reflectLevel = 0.9f;
		satLevel = 0.8f;
	}

	vertices[0].r = lightLevel;
	vertices[0].g = reflectLevel;
	vertices[0].b = satLevel;
	vertices[0].a = invGlowLevel;

	vertices[1].r = lightLevel;
	vertices[1].g = reflectLevel;
	vertices[1].b = satLevel;
	vertices[1].a = invGlowLevel;

	vertices[2].r = lightLevel;
	vertices[2].g = reflectLevel;
	vertices[2].b = satLevel;
	vertices[2].a = invGlowLevel;

	vertices[3].r = lightLevel;
	vertices[3].g = reflectLevel;
	vertices[3].b = satLevel;
	vertices[3].a = invGlowLevel;
}
void ItemTerraBlok::SetUVs ( CModelVertex* vertices, Terrain::EFaceDir dir )
{
	ushort data = blokType;

	vertices[0].u = 0;
	vertices[0].v = 0;
	vertices[1].u = 0;
	vertices[1].v = 1;
	vertices[2].u = 1;
	vertices[2].v = 1;
	vertices[3].u = 1;
	vertices[3].v = 0;

	float texScale = 1.0f;
	if (( data == Terrain::EB_DIRT )||( data == Terrain::EB_GRASS )||( data == Terrain::EB_SAND )
		||( data == Terrain::EB_CLAY )||( data == Terrain::EB_MUD )||( data == Terrain::EB_GRAVEL )
		||( data == Terrain::EB_ROAD_GRAVEL )||( data == Terrain::EB_ASH )
		||( data == Terrain::EB_DEADSTONE )||( data == Terrain::EB_CURSED_DEADSTONE )
		||( data == Terrain::EB_SNOW )||( data == Terrain::EB_TOP_SNOW )||( data == Terrain::EB_HEMATITE ))
	{
		texScale = 1/4.0f;
	}
	else if (( data == Terrain::EB_STONE )||( data == Terrain::EB_STONEBRICK ))
	{
		texScale = 1/2.0f;
	}
	else if ( data == Terrain::EB_WOOD )
	{
		//texScale = 1/1.5f;
		texScale = 1/2.0f;
	}
	else if (( data == Terrain::EB_CRYSTAL )||( data == Terrain::EB_XPLO_CRYSTAL )||( data == Terrain::EB_ICE ))
	{
		texScale = 1/4.0f;
	}

	if (( data == Terrain::EB_GRASS )&&( dir != Terrain::TOP )&&( dir != Terrain::BOTTOM ))
	{
		texScale = 0.5f;
	}

	//atlas is 4x4
	float border = 1/1024.0f;
	for ( unsigned int i = 0; i < 4; i += 1 )
	{
		vertices[i].u *= 0.25f*texScale-(border*2);
		vertices[i].v *= 0.25f*texScale-(border*2);

		switch ( data )
		{
		case Terrain::EB_GRASS:
			if ( dir == Terrain::TOP ) {
				vertices[i].u += 0.0;
				//vertices[vertexCount+i].v += 0.0;
			}
			else if ( dir == Terrain::BOTTOM ) {
				vertices[i].u += 0.50; //Dirt on bottom
				//vertices[vertexCount+i].v += 0.0;
			}
			else {
				vertices[i].u += 0.25;
				vertices[i].v += 0.25;
			}
			break;
		case Terrain::EB_DIRT:
		case Terrain::EB_MUD:
			vertices[i].u += 0.50;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case Terrain::EB_STONE:
			vertices[i].u += 0.25;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case Terrain::EB_STONEBRICK:
			vertices[i].u += 0.50;
			vertices[i].v += 0.25;
			break;
		case Terrain::EB_SAND:
		case Terrain::EB_ASH:
		case Terrain::EB_SNOW:
		case Terrain::EB_TOP_SNOW:
			vertices[i].u += 0.75;
			break;
		case Terrain::EB_CLAY:
			//vertices[vertexCount+i].u += 0.0;
			vertices[i].v += 0.25;
			break;
		case Terrain::EB_WATER: // DTerrain::EBUGGGGG TODO
			vertices[i].u += 0.75;
			vertices[i].v += 0.75;
			break;
		case Terrain::EB_WOOD:
			vertices[i].u += 0.25;
			vertices[i].v += 0.50;
			break;
		case Terrain::EB_CRYSTAL:
		case Terrain::EB_XPLO_CRYSTAL:
		case Terrain::EB_ICE:
			//vertices[vertexCount+i].u += 0.00;
			vertices[i].v += 0.50;
			break;
		case Terrain::EB_GRAVEL:
		case Terrain::EB_ROAD_GRAVEL:
			vertices[i].u += 0.50;
			vertices[i].v += 0.50;
			break;
		case Terrain::EB_DEADSTONE:
		case Terrain::EB_CURSED_DEADSTONE:
			vertices[i].u += 0.75;
			vertices[i].v += 0.50;
		case Terrain::EB_ROOF_0:
			vertices[i].u += 0.25;
			vertices[i].v += 0.75;
			break;
		case Terrain::EB_ROOF_1:
			vertices[i].u += 0.75;
			vertices[i].v += 0.25;
			break;
		case Terrain::EB_HEMATITE:
			vertices[i].u += 0.50;
			vertices[i].v += 0.75;
			break;
		}

		vertices[i].u += border;
		vertices[i].v += border;
	}
}

//Returns the blok type
ushort ItemTerraBlok::GetType (void)
{
	return blokType;
}
// Sets the blok type
void ItemTerraBlok::SetType (ushort newType)
{
	blokType = newType;
	// Set the name based on the type
	weaponItemData.sInfo = Terrain::blockName[blokType];
	weaponItemState.sItemName = weaponItemData.sInfo;
	
	// Regen block
	GenerateModel();
}


// Sets the transform to set up a camera
void ItemTerraBlok::SetInventoryCameraTransform ( XTransform & inTransform )
{
	// Set camera
	inTransform.rotation = Rotator( 90,0,0 ) * Rotator( -45,-26,0 );
	//inTransform.rotation = Vector3d( 0,26,45 );
	//inTransform.position = inTransform.rotation * Vector3d( -3,0,0 );
}
void ItemTerraBlok::SetInventoryCameraWidth ( ftype & inWidth )
{
	inWidth = 0.9f;
}
// Grabs the icon for this object
CTexture* ItemTerraBlok::GetInventoryIcon ( void )
{
	pModel = blokRender;
	CTexture* result = CWeaponItem::GetInventoryIcon();
	pModel = NULL;
	return result;
	/*if ( icon.bGenerated )
	{
		return icon.pRenderTex;
	}
	else
	{
		{	// Create render texture
			icon.pRenderTex = new CRenderTexture( RGBA8, 64,64, Clamp,Clamp, Texture2D );
		}

		glMaterial* prevMat = glMaterial::current;

		{	// Set up the rendering thing
			GL.setupViewport( 0,0,64,64 ); // DirectX
			//GL.setupViewport( 0,64,64,-64 ); // OpenGL
		}

		{	// First, set up the projection matrix
			// Select The Projection Matrix
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

			// Reset The Projection Matrix
			glLoadIdentity();
			// Change the perspective
			//gluPerspective(fov,Screen::Info.aspect,zNear,zFar);
			//gluPerspective( 80.0f, 1.0f, 0.2f, 20.0f );
			ftype vsize;// = 1.0f;
			SetInventoryCameraWidth( vsize );
			//glOrtho( -vsize,vsize, -vsize,vsize, 0.2f, 20.0f ); 
			glOrtho( -vsize,vsize, vsize,-vsize, 0.2f, 20.0f ); 

			glPushMatrix(); // Save
		}

		{	// Next setup modelview matrix
			// Switch to model view
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
		
			// Reset the model view matrix
			glLoadIdentity();
			// Rotate camera into default position
			glRotatef( -90, 1,0,0 );
			glRotatef( 90, 0,0,1 );

			// Get target camera transform
			CTransform camtrans;
			SetInventoryCameraTransform( camtrans );
			// Rotate camera
			Matrix4x4 temp;
			temp.setRotation( !camtrans.rotation );
			glMultMatrixf( temp.pData );
			// Translate camera
			//glTranslatef( -transform.position.x, -transform.position.y, -transform.position.z );
			glTranslatef( -camtrans.position.x, -camtrans.position.y, -camtrans.position.z );

			glPushMatrix(); // Save
		}

		{	// Bind buffer
			icon.pRenderTex->BindBuffer();
		}

		// Draw stuff
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			glDepthMask( GL_TRUE );
			glClear( GL_COLOR_BUFFER_BIT );
			glClear( GL_DEPTH_BUFFER_BIT );

			pModel = blokRender;
			if ( pModel )
			{
				pModel->visible = true;
				pModel->SetForcedDraw();

				pModel->transform.position = Vector3d( 0,0,0 );
				pModel->transform.rotation = Vector3d( 0,0,0 );
				pModel->transform.scale = Vector3d( 1,1,1 );

				char maxPass = pModel->GetPassNumber();
				for ( char pass = 0; pass < maxPass; ++pass )
				{
					GL.prepareDraw();
					pModel->Render( pass );
					GL.cleanupDraw();
				}
			}
			pModel = NULL;
		}

		{	// Unbind buffer
			icon.pRenderTex->UnbindBuffer();
		}

		{	// Select The Projection Matrix
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glPopMatrix();
		}
		{	// Switch to model view
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glPopMatrix();
		}

		{	// Viewport
			GL.setupViewport( 0,0,Screen::Info.width,Screen::Info.height );
		}

		{	// And the icon is ready
			icon.bGenerated = true;
		}

		if ( prevMat )
			prevMat->bind();

		return icon.pRenderTex;
	}*/
}
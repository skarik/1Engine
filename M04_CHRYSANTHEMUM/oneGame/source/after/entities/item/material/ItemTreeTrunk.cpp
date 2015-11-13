
#include "ItemTreeTrunk.h"
#include "after/entities/foliage/CTreeBase.h"

ItemTreeTrunk::ItemTreeTrunk ( glMaterial* pInMat, const WItemData & wdat )
	: CWeaponItem( wdat )
{
	pTreeMat = pInMat;

	rShapeCurrent.dir = Vector3d( 0,0,2 );
	rShapeCurrent.pos = Vector3d( 0,0,0 );
	rShapeParent.dir = Vector3d( 0,0,2 );
	rShapeParent.pos = Vector3d( 0,0,0 );

	fSizeCurrent = 0.9f;
	fSizeParent = 1;
	GenerateMesh();
}
ItemTreeTrunk::ItemTreeTrunk ( glMaterial* pInMat )
	: CWeaponItem( ItemData() )
{
	pTreeMat = pInMat;

	rShapeCurrent.dir = Vector3d( 0,0,2 );
	rShapeCurrent.pos = Vector3d( 0,0,0 );
	rShapeParent.dir = Vector3d( 0,0,2 );
	rShapeParent.pos = Vector3d( 0,0,0 );

	fSizeCurrent = 0.9f;
	fSizeParent = 1;
	GenerateMesh();
}
ItemTreeTrunk::ItemTreeTrunk ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat, const WItemData & wdat )
	: CWeaponItem( wdat )
{
	pTreeMat = pInMat;

	rShapeCurrent	= rInShape;
	fSizeCurrent	= fabs(fInSize);

	rShapeParent	= rInPShape;
	fSizeParent		= fabs(fInPSize);

	GenerateMesh();
}
ItemTreeTrunk::ItemTreeTrunk ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat )
	: CWeaponItem( ItemData() )
{
	pTreeMat = pInMat;

	rShapeCurrent	= rInShape;
	fSizeCurrent	= fabs(fInSize);

	rShapeParent	= rInPShape;
	fSizeParent		= fabs(fInPSize);

	GenerateMesh();
}

ItemTreeTrunk::~ItemTreeTrunk ( void )
{
	
}

ItemTreeTrunk::ItemTreeTrunk ( void ) : CWeaponItem( ItemData() )
{
	pTreeMat = CTreeBase::pTreeBarkMaterial;
	if ( pTreeMat == NULL ) {
		CTreeBase::LoadTreeMaterials();
		pTreeMat = CTreeBase::pTreeBarkMaterial;
	}

	rShapeCurrent.dir = Vector3d( 0,0,2 );
	rShapeCurrent.pos = Vector3d( 0,0,0 );
	rShapeParent.dir = Vector3d( 0,0,2 );
	rShapeParent.pos = Vector3d( 0,0,0 );

	fSizeCurrent = 0.9f;
	fSizeParent = 1;
	GenerateMesh();
}
CWeaponItem& ItemTreeTrunk::operator= ( const CWeaponItem * original )
{
	ItemTreeTrunk* oitem = (ItemTreeTrunk*)original;

	pTreeMat = oitem->pTreeMat;
	if ( pTreeMat ) {
		pModel->SetMaterial( pTreeMat );
	}
	else {
		pTreeMat = CTreeBase::pTreeBarkMaterial;
		if ( pTreeMat == NULL ) {
			CTreeBase::LoadTreeMaterials();
			pTreeMat = CTreeBase::pTreeBarkMaterial;
		}
		pModel->SetMaterial( pTreeMat );
	}

	return CWeaponItem::operator=( original );
}


void ItemTreeTrunk::GenerateMesh ( void )
{
	// First, free the old mesh
	if ( pModel != NULL ) {
		delete pModel;
	}
	pModel = NULL;

	// Prepare new mesh.
	CModelData mdNewMesh;

	// Check for invalid values on the sizes
	if ( fSizeParent < fSizeCurrent ) {
		fSizeParent = fSizeCurrent;
	}
	if ( fSizeCurrent < 0.05f ) {
		fSizeCurrent = fSizeParent*0.93f;
	}
	
	// For now, generate a temporary mesh.
	mdNewMesh.vertexNum = 12 + (7*2);
	mdNewMesh.vertices = new CModelVertex [ mdNewMesh.vertexNum ];
	mdNewMesh.triangleNum = 12 + (6*2);
	mdNewMesh.triangles = new CModelTriangle [ mdNewMesh.triangleNum ];

	{
		/*Ray rShapeCurrent;
		ftype fSizeCurrent;
		Ray rShapeParent;
		ftype fSizeParent;

		rShapeCurrent.dir = Vector3d( 0,0,2 );
		rShapeCurrent.pos = Vector3d( 0,0,0 );
		rShapeParent.dir = Vector3d( 0,0,2 );
		rShapeParent.pos = Vector3d( 0,0,0 );

		fSizeCurrent = 0.9f;
		fSizeParent = 1;*/

		// Piece walls
		for ( int j = 0; j < 6; j++ )
		{
			Vector3d gOffset;

			Vector3d vAngleVector = Vector3d( (ftype)sin(degtorad(j*60)), (ftype)cos(degtorad(j*60)), 1 );

			gOffset = rShapeParent.dir.cross( vAngleVector );
			gOffset = gOffset.normal() * fSizeParent;

			Vector3d tOffset;
			tOffset = rShapeCurrent.dir.cross( vAngleVector );
			tOffset = tOffset.normal() * fSizeCurrent;

			Vector3d pBottom = rShapeCurrent.pos + gOffset;
			Vector3d pTop = rShapeCurrent.pos+rShapeCurrent.dir + tOffset;

			unsigned int vertIndex = j*2;
			mdNewMesh.vertices[ vertIndex ].x = pBottom.x;
			mdNewMesh.vertices[ vertIndex ].y = pBottom.y;
			mdNewMesh.vertices[ vertIndex ].z = pBottom.z;
			mdNewMesh.vertices[ vertIndex ].v = pBottom.z*0.25f;
			mdNewMesh.vertices[ vertIndex ].u = fabs(1.0f - j/3.0f);
			mdNewMesh.vertices[ vertIndex ].nx = gOffset.normal().x;
			mdNewMesh.vertices[ vertIndex ].ny = gOffset.normal().y;
			mdNewMesh.vertices[ vertIndex ].nz = gOffset.normal().z;
			vertIndex++;
			mdNewMesh.vertices[ vertIndex ].x = pTop.x;
			mdNewMesh.vertices[ vertIndex ].y = pTop.y;
			mdNewMesh.vertices[ vertIndex ].z = pTop.z;
			mdNewMesh.vertices[ vertIndex ].v = pTop.z*0.25f;
			mdNewMesh.vertices[ vertIndex ].u = fabs(1.0f - j/3.0f);
			mdNewMesh.vertices[ vertIndex ].nx = tOffset.normal().x;
			mdNewMesh.vertices[ vertIndex ].ny = tOffset.normal().y;
			mdNewMesh.vertices[ vertIndex ].nz = tOffset.normal().z;

			unsigned int triIndex = j*2;
			mdNewMesh.triangles[ triIndex ].vert[0] = triIndex;
			mdNewMesh.triangles[ triIndex ].vert[1] = triIndex+1;
			mdNewMesh.triangles[ triIndex ].vert[2] = triIndex+2;
			if ( j == 5 )
			{
				mdNewMesh.triangles[ triIndex ].vert[2] -= 12;
			}
			mdNewMesh.triangles[ triIndex+1 ].vert[0] = triIndex+1;
			mdNewMesh.triangles[ triIndex+1 ].vert[1] = triIndex+3;
			mdNewMesh.triangles[ triIndex+1 ].vert[2] = triIndex+2;
			if ( j == 5 )
			{
				mdNewMesh.triangles[ triIndex+1 ].vert[1] -= 12;
				mdNewMesh.triangles[ triIndex+1 ].vert[2] -= 12;
			}
		}

		// Place top
		{
			unsigned int iVertIndex = 12;
			unsigned int iTriIndex	= 12;
			// get the part normal
			Vector3d dir = rShapeCurrent.dir.normal();
			for ( int j = 0; j < 6; ++j )
			{
				// create the edge point
				Vector3d gOffset;

				Vector3d vAngleVector = Vector3d( (ftype)sin(degtorad(j*60)), (ftype)cos(degtorad(j*60)), 1 );

				gOffset = rShapeParent.dir.cross( vAngleVector );
				gOffset = gOffset.normal() * fSizeParent;

				Vector3d tOffset;
				tOffset = rShapeCurrent.dir.cross( vAngleVector );
				tOffset = tOffset.normal() * fSizeCurrent;

				Vector3d pTop = rShapeCurrent.pos+rShapeCurrent.dir + tOffset;

				// create the vertices
				mdNewMesh.vertices[iVertIndex+j].x = pTop.x;
				mdNewMesh.vertices[iVertIndex+j].y = pTop.y;
				mdNewMesh.vertices[iVertIndex+j].z = pTop.z;
				mdNewMesh.vertices[iVertIndex+j].nx = dir.x;
				mdNewMesh.vertices[iVertIndex+j].ny = dir.y;
				mdNewMesh.vertices[iVertIndex+j].nz = dir.z;
				mdNewMesh.vertices[iVertIndex+j].u = pTop.z*0.011f;
				mdNewMesh.vertices[iVertIndex+j].v = fabs( 1.0f - j/3.0f )*0.1f + 0.45f;

				// create the triangles
				if ( j != 5 )
				{
					mdNewMesh.triangles[ iTriIndex+j ].vert[1] = iVertIndex+j;
					mdNewMesh.triangles[ iTriIndex+j ].vert[0] = iVertIndex+j+1;
					mdNewMesh.triangles[ iTriIndex+j ].vert[2] = iVertIndex+6;
				}
				else
				{
					mdNewMesh.triangles[ iTriIndex+j ].vert[1] = iVertIndex+j;
					mdNewMesh.triangles[ iTriIndex+j ].vert[0] = iVertIndex;
					mdNewMesh.triangles[ iTriIndex+j ].vert[2] = iVertIndex+6;
				}

			}
			// create the center vertex
			mdNewMesh.vertices[iVertIndex+6].x = rShapeCurrent.pos.x+rShapeCurrent.dir.x;
			mdNewMesh.vertices[iVertIndex+6].y = rShapeCurrent.pos.y+rShapeCurrent.dir.y;
			mdNewMesh.vertices[iVertIndex+6].z = rShapeCurrent.pos.z+rShapeCurrent.dir.z;
			mdNewMesh.vertices[iVertIndex+6].nx = dir.x;
			mdNewMesh.vertices[iVertIndex+6].ny = dir.y;
			mdNewMesh.vertices[iVertIndex+6].nz = dir.z;
			mdNewMesh.vertices[iVertIndex+6].u = sqrtf( fabs( fSizeCurrent ) );
			mdNewMesh.vertices[iVertIndex+6].v = 0.5f;
		}

		// Place bottom
		{
			unsigned int iVertIndex = 12+7;
			unsigned int iTriIndex	= 12+6;
			// get the part normal
			Vector3d dir = -rShapeCurrent.dir.normal();
			for ( int j = 0; j < 6; ++j )
			{
				// create the edge point
				Vector3d gOffset;

				Vector3d vAngleVector = Vector3d( (ftype)sin(degtorad(j*60)), (ftype)cos(degtorad(j*60)), 1 );

				gOffset = rShapeParent.dir.cross( vAngleVector );
				gOffset = gOffset.normal() * fSizeParent;

				Vector3d tOffset;
				tOffset = rShapeCurrent.dir.cross( vAngleVector );
				tOffset = tOffset.normal() * fSizeCurrent;

				//Vector3d pTop = rShapeCurrent.pos+rShapeCurrent.dir + tOffset;
				Vector3d pTop = rShapeCurrent.pos + gOffset;

				// create the vertices
				mdNewMesh.vertices[iVertIndex+j].x = pTop.x;
				mdNewMesh.vertices[iVertIndex+j].y = pTop.y;
				mdNewMesh.vertices[iVertIndex+j].z = pTop.z;
				mdNewMesh.vertices[iVertIndex+j].nx = dir.x;
				mdNewMesh.vertices[iVertIndex+j].ny = dir.y;
				mdNewMesh.vertices[iVertIndex+j].nz = dir.z;
				mdNewMesh.vertices[iVertIndex+j].u = pTop.z*0.011f;
				mdNewMesh.vertices[iVertIndex+j].v = fabs( 1.0f - j/3.0f )*0.1f + 0.45f;

				// create the triangles
				if ( j != 5 )
				{
					mdNewMesh.triangles[ iTriIndex+j ].vert[0] = iVertIndex+j;
					mdNewMesh.triangles[ iTriIndex+j ].vert[1] = iVertIndex+j+1;
					mdNewMesh.triangles[ iTriIndex+j ].vert[2] = iVertIndex+6;
				}
				else
				{
					mdNewMesh.triangles[ iTriIndex+j ].vert[0] = iVertIndex+j;
					mdNewMesh.triangles[ iTriIndex+j ].vert[1] = iVertIndex;
					mdNewMesh.triangles[ iTriIndex+j ].vert[2] = iVertIndex+6;
				}

			}
			// create the center vertex
			mdNewMesh.vertices[iVertIndex+6].x = rShapeCurrent.pos.x;
			mdNewMesh.vertices[iVertIndex+6].y = rShapeCurrent.pos.y;
			mdNewMesh.vertices[iVertIndex+6].z = rShapeCurrent.pos.z;
			mdNewMesh.vertices[iVertIndex+6].nx = dir.x;
			mdNewMesh.vertices[iVertIndex+6].ny = dir.y;
			mdNewMesh.vertices[iVertIndex+6].nz = dir.z;
			mdNewMesh.vertices[iVertIndex+6].u = sqrtf( fabs( fSizeParent ) );
			mdNewMesh.vertices[iVertIndex+6].v = 0.5f;
		}
	}

	// Take the mesh data, and center it.
	{
		Vector3d centerPoint (0,0,0);
		for ( unsigned int vert = 0; vert < mdNewMesh.vertexNum; ++vert )
		{
			centerPoint.x += mdNewMesh.vertices[ vert ].x;
			centerPoint.y += mdNewMesh.vertices[ vert ].y;
			centerPoint.z += mdNewMesh.vertices[ vert ].z;
		}
		centerPoint /= ((ftype)(mdNewMesh.vertexNum));
		for ( unsigned int vert = 0; vert < mdNewMesh.vertexNum; ++vert )
		{
			mdNewMesh.vertices[ vert ].x -= centerPoint.x;
			mdNewMesh.vertices[ vert ].y -= centerPoint.y;
			mdNewMesh.vertices[ vert ].z -= centerPoint.z;
		}
	}

	// Set default effect values
	for ( unsigned int vert = 0; vert < mdNewMesh.vertexNum; ++vert )
	{
		mdNewMesh.vertices[vert].r = mdNewMesh.vertices[vert].x;
		mdNewMesh.vertices[vert].g = mdNewMesh.vertices[vert].y;
		mdNewMesh.vertices[vert].b = mdNewMesh.vertices[vert].z;
		mdNewMesh.vertices[vert].a = 0;
		mdNewMesh.vertices[vert].w = 0.25f;
		mdNewMesh.vertices[vert].u4 = 0;
		mdNewMesh.vertices[vert].v4 = 1;
	}

	// Create a new model based on the new mesh data
	pModel = new CModel( mdNewMesh, string("_sys_override_") );
	if ( pTreeMat ) {
		pModel->SetMaterial( pTreeMat );
	}
	else {
		pTreeMat = CTreeBase::pTreeBarkMaterial;
		if ( pTreeMat == NULL ) {
			CTreeBase::LoadTreeMaterials();
			pTreeMat = CTreeBase::pTreeBarkMaterial;
		}
		pModel->SetMaterial( pTreeMat );
	}
	
}

// Function for initializing the collision/physics
void ItemTreeTrunk::CreatePhysics ( void )
{
	CWeaponItem::CreatePhysics();

	if ( pBody )
	{
		//pBody->SetQualityType( HK_COLLIDABLE_QUALITY_DEBRIS );
		pBody->SetRestitution( 0.05f );
		pBody->SetLinearDamping( 0.2f );
	}
}

// Lookats create some outlines
void	ItemTreeTrunk::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.cOutlineColor = Color( 1.0f,0.7f,0.3f );
		//pModel->renderSettings.fOutlineWidth = 0.06f;
	}
}
void	ItemTreeTrunk::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.fOutlineWidth = -1.0f;
	}
}

void ItemTreeTrunk::Update ( void )
{
	// No behavior
}
bool ItemTreeTrunk::Use ( int x )
{
	// Return false, meaning fall back to ItemBlokPuncher beahavior.
	// You cannot place this kind of wood. You need to craft it into a wood block first.
	// Supposedly, anyways. This might change to more Minecraft-style in the future.
	return false;
}


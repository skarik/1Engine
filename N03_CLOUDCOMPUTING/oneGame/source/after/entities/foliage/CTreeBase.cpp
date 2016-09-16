
// == Includes ==
//#include "unused/CVoxelTerrain.h"
#include "core/math/Math.h"

#include "CTreeBase.h"
#include "after/renderer/objects/tree/CTreeRenderer.h"
//#include "CTreeLeafRenderer.h"
#include "after/terrain/Zones.h"
#include "after/types/terrain/BiomeTypes.h"

#include "renderer/texture/CTexture3D.h"
#include "renderer/material/glMaterial.h"
#include "renderer/camera/CCamera.h"

using std::vector;
using std::list;

// == Static variables ==
Perlin* CTreeBase::noise = new Perlin( 1,7.07f,1, 0x213 );
glMaterial* CTreeBase::pTreeBarkMaterial = NULL;
glMaterial* CTreeBase::pTreeLeafMaterial = NULL;

CModelVertex*		CTreeBase::pVertexBuffer	= NULL;
CModelTriangle*		CTreeBase::pTriangleBuffer	= NULL;
uint				CTreeBase::iVertexOffset	= 0;
uint				CTreeBase::iTriangleOffset	= 0;
bool				CTreeBase::bNeedRegen		= false;
vector<CTreeBase*>	CTreeBase::vTreeList;
CTreeRenderer*		CTreeBase::mRenderManager	= NULL;
//CTreeLeafRenderer*	CTreeBase::mLeafRenderManager=NULL;

// == Constructor and Destructor ==
CTreeBase::CTreeBase ( void )
	: CFoliage()
{
	noise->unnormalize = true;

	// Create models if not there
	//pTreeModel = NULL;
	//pLeafModel = NULL;
	if ( pVertexBuffer == NULL ) {
		pVertexBuffer = new CModelVertex [ 4096 ];
	}
	if ( pTriangleBuffer == NULL ) {
		pTriangleBuffer = new CModelTriangle [ 4096 ];
	}

	modelData.vertices = pVertexBuffer;
	modelData.triangles = pTriangleBuffer;

	// Set collision related object to initially null
	pTreeCollision	= NULL;
	pTreeBody		= NULL;

	// Load materials
	LoadTreeMaterials();
	
	// Create renderer manager
	if ( mRenderManager == NULL )
	{
		mRenderManager = new CTreeRenderer;
		mRenderManager->RemoveReference();
	}
	/*if ( mLeafRenderManager == NULL )
	{
		mLeafRenderManager = new CTreeLeafRenderer;
		mLeafRenderManager->RemoveReference();
	}*/

	// New stuff
	bNeedsRegen = false;
	bNeedCollisionRegen = true;
	bRegenPriority = false;

	if ( vTreeList.empty() ) {
		vTreeList.reserve( 2048 );
	}
	vTreeList.push_back( this );

	bNeedsLoDRegen = false;
	mLoDLevel = 3;
	bInView = false;
	vLastRegenPosition = Vector3d( 0,0,0 );
	UpdateLoD();

	// Set biome to null
	mBiome = Terrain::BIO_DEFAULT;

	// Set default rendering tech
	mTexCoordR = 0.25f;
}
void CTreeBase::LoadTreeMaterials ( void )
{
	// Create the materials if they haven't been initialized yet
	/*if ( pTreeBarkMaterial == NULL )
	{
		pTreeBarkMaterial =  new glMaterial();
		//pTreeBarkMaterial->loadTexture( "textures\\tree bark.jpg" );
		pTreeBarkMaterial->setTexture( 0, new CTexture3D( ".res/textures/treeTexture.jpg", Texture3D, RGBA8, 2, 1 ) );
		//pTreeBarkMaterial->setShader( new glShader( "shaders\\defaultDiffuse.glsl" ) );
		pTreeBarkMaterial->passinfo.push_back( glPass() );
		pTreeBarkMaterial->passinfo[0].shader = new glShader( ".res/shaders/world/treeDefault.glsl" );
	}
	if ( pTreeLeafMaterial == NULL )
	{
		pTreeLeafMaterial =  new glMaterial();
		pTreeLeafMaterial->setTexture( 0, new CTexture(".res/textures/leaves.tga") );
		pTreeLeafMaterial->passinfo.push_back( glPass() );
		pTreeLeafMaterial->passinfo[0].shader = new glShader( ".res/shaders/world/leavesDefault.glsl" );
		//pTreeLeafMaterial->isTransparent= true; // Transparent material
		pTreeLeafMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
	}*/
	if ( pTreeBarkMaterial == NULL )
	{
		pTreeBarkMaterial =  new glMaterial();

		pTreeBarkMaterial->setTexture( 0, new CTexture( "textures/leaves.tga" ) );
		pTreeBarkMaterial->setTexture( 1, new CTexture3D( "textures/treeTexture.jpg", Texture3D, RGBA8, 2, 1 ) );

		pTreeBarkMaterial->passinfo.push_back( glPass() );
		pTreeBarkMaterial->passinfo[0].shader = new glShader( "shaders/world/treeDefault.glsl" );
		pTreeBarkMaterial->passinfo.push_back( glPass() );
		pTreeBarkMaterial->passinfo[1].shader = new glShader( "shaders/world/leavesDefault.glsl" );
		pTreeBarkMaterial->passinfo[1].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;

		pTreeBarkMaterial->passinfo.push_back( glPass() );
		pTreeBarkMaterial->passinfo[2].shader = new glShader( "shaders/world/treeDefault.glsl" );
		pTreeBarkMaterial->passinfo[2].m_hint = RL_SHADOW_COLOR;
		pTreeBarkMaterial->passinfo.push_back( glPass() );
		pTreeBarkMaterial->passinfo[3].m_hint = RL_SHADOW_COLOR;
		pTreeBarkMaterial->passinfo[3].shader = new glShader( "shaders/world/leavesDefault.glsl" );
		pTreeBarkMaterial->passinfo[3].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;

		pTreeBarkMaterial->passinfo.push_back( glPass() );
		pTreeBarkMaterial->passinfo[4].shader = new glShader( "shaders/world/treeDefault.glsl" );
		pTreeBarkMaterial->passinfo[4].m_hint = RL_SKYGLOW;
		pTreeBarkMaterial->passinfo.push_back( glPass() );
		pTreeBarkMaterial->passinfo[5].m_hint = RL_SKYGLOW;
		pTreeBarkMaterial->passinfo[5].shader = new glShader( "shaders/world/leavesDefault.glsl" );
		pTreeBarkMaterial->passinfo[5].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;

		pTreeBarkMaterial->deferredinfo.push_back( glPass_Deferred() );
		pTreeBarkMaterial->deferredinfo[0].m_diffuse_method = Renderer::Deferred::DIFFUSE_TREESYS;
		pTreeBarkMaterial->deferredinfo[0].m_rimlight_strength = 0.2f;

		pTreeBarkMaterial->deferredinfo.push_back( glPass_Deferred() );
		pTreeBarkMaterial->deferredinfo[1].m_diffuse_method = Renderer::Deferred::DIFFUSE_TREESYS;
		pTreeBarkMaterial->deferredinfo[1].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
		pTreeBarkMaterial->deferredinfo[1].m_rimlight_strength = 0.6f;
	}
}
CTreeBase::~CTreeBase ( void )
{
	// Free Models
	FreeModels();
	// Free buffer
	mRenderManager->FreeTreeBuffer( &buffer );
	mRenderManager->FreeLeafBuffer( &buffer );

	// Free the part list
	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		delete part_list[i];
	}
	part_list.clear();

	// Find self in list and remove
	vector<CTreeBase*>::iterator foundtree = find( vTreeList.begin(), vTreeList.end(), this );
	vTreeList.erase( foundtree );
	//mRenderManager->Interrupt();
	//mLeafRenderManager->Interrupt();

	bNeedRegen = true;
}

void CTreeBase::FreeModels ( void )
{
	// Free the models if they've been loaded
	/*if ( pTreeModel )
		delete pTreeModel;
	pTreeModel = NULL;
	if ( pLeafModel )
		delete pLeafModel;
	pLeafModel = NULL;*/

	//mRenderManager->FreeBuffer( &buffer );

	// Free the collision if they're loaded
	if ( pTreeBody )			// First delete the collision
		delete pTreeBody;
	pTreeBody = NULL;
	if ( pTreeCollision )		// Then delete the collider
		delete pTreeCollision;
	pTreeCollision = NULL;
}

// == Update ==
void CTreeBase::Update ( void )
{
	// Loop through parts and check for connection to bottom?
	/*for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		DebugD::DrawLine( part_list[i]->shape.pos+transform.position, part_list[i]->shape.pos+part_list[i]->shape.dir+transform.position );
		for ( int j = 0; j < 6; j += 1 )
		{
			Vector3d gOffset;
			//gOffset = part_list[i]->shape.dir.cross( Vector3d( sin(degtorad(j*60)), cos(degtorad(j*60)), 1 ) );

			if ( part_list[i]->parent == NULL )
			{
				gOffset = part_list[i]->shape.dir.cross( Vector3d( sin(degtorad(j*60)), cos(degtorad(j*60)), 1 ) );
				gOffset = gOffset.normal() * part_list[i]->size;
			}
			else
			{
				gOffset = part_list[i]->parent->shape.dir.cross( Vector3d( sin(degtorad(j*60)), cos(degtorad(j*60)), 1 ) );
				gOffset = gOffset.normal() * part_list[i]->parent->size;
			}

			Vector3d tOffset;
			tOffset = part_list[i]->shape.dir.cross( Vector3d( sin(degtorad(j*60)), cos(degtorad(j*60)), 1 ) );
			tOffset = tOffset.normal() * part_list[i]->size;

			DebugD::DrawLine(
				part_list[i]->shape.pos+transform.position + gOffset,
				part_list[i]->shape.pos+part_list[i]->shape.dir+transform.position + tOffset
				);
		}
	}*/

	/*if ( bNeedsRegen )
	{
		FreeModels();
		//Generate();
		//GenerateTreeMesh();
		//GenerateLeafMesh( leaf_cloud );
		//bNeedsRegen = false;
	}*/

	/*if ( bNeedCollisionRegen && bRegenPriority ) 
	{
		GenerateTreeMesh();
	}*/

	if ( mBiome == Terrain::BIO_DEFAULT ) {
		mBiome = Zones.GetTerrainBiomeAt( transform.position );
	}

	// Update LoD stuff
	UpdateLoD();

	// Check if need to force update
	if ( mRenderManager->transform.position.sqrMagnitude() > 1.0f ) {
		bNeedsRegen = true;
	}

	// Update visual check
	bInView = CCamera::activeCamera->SphereIsVisible( transform.position+Vector3d(0,0,3), 8 );
	if ( !bInView ) {
		bInView = CCamera::activeCamera->SphereIsVisible( vLastRegenPosition+Vector3d(0,0,3), 8 );
	}

	if ( (bNeedsRegen || bNeedsLoDRegen) && bInView )
	{
		vLastRegenPosition = transform.position;

		// Pre-calc vert effects
		ftype snowAmt = 0.0f;
		if ( mBiome == Terrain::BIO_S_TUNDRA ) {
			snowAmt = 1.0f;
		}
		else if ( mBiome == Terrain::BIO_TAIGA || mBiome == Terrain::BIO_TUNDRA ) {
			snowAmt = 0.8f;
		}
		else {
			snowAmt = 0.0f;
		}

		//FreeModels();
		mRenderManager->FreeTreeBuffer( &buffer );

		GenerateTreeMesh();
		// Loop through the vertices and transform by pos and rotation
		// Also apply vertex effects
		for ( uint v = 0; v < modelData.vertexNum; ++v ) {
			pVertexBuffer[v].r = pVertexBuffer[v].x;
			pVertexBuffer[v].g = pVertexBuffer[v].y;
			pVertexBuffer[v].b = pVertexBuffer[v].z; // Copy local model space over to color
			pVertexBuffer[v].a = pVertexBuffer[v].z + (sqr(pVertexBuffer[v].x) + sqr(pVertexBuffer[v].y)) * 0.1f;
			pVertexBuffer[v].x += transform.position.x;
			pVertexBuffer[v].y += transform.position.y;
			pVertexBuffer[v].z += transform.position.z;
			// todo: stream rotation (How to stream rotation? Send a buffer of matrices instead? bite the bullet and transform it on the CPU?)

			// set w coord
			pVertexBuffer[v].w = mTexCoordR;

			// now for snow effects
			pVertexBuffer[v].u4 = snowAmt;
			//pVertexBuffer[v].v4 = 1-snowAmt;
		}
		if ( modelData.vertexNum > 1 ) {
			mRenderManager->UpdateTreeBuffer( &buffer, &modelData );
		}

		mRenderManager->FreeLeafBuffer( &buffer );

		GenerateLeafMesh( leaf_cloud );
		// Loop through the vertices and transform by pos and rotation
		for ( uint v = 0; v < modelData.vertexNum; ++v ) {	
			pVertexBuffer[v].r = pVertexBuffer[v].x;
			pVertexBuffer[v].g = pVertexBuffer[v].y;
			pVertexBuffer[v].b = pVertexBuffer[v].z; // Copy local model space over to color
			// alpha is....
			pVertexBuffer[v].x += transform.position.x;
			pVertexBuffer[v].y += transform.position.y;
			pVertexBuffer[v].z += transform.position.z;
			// todo: stream rotation

			// now for snow effects
			pVertexBuffer[v].u4 = snowAmt;
		}
		if ( modelData.vertexNum > 1 ) {
			mRenderManager->UpdateLeafBuffer( &buffer, &modelData );
		}

		bNeedsRegen = false;
		bNeedsLoDRegen = false;
	}
}

void CTreeBase::UpdateLoD ( void )
{
	uchar nextLoD = 3;
	if ( CCamera::activeCamera )
	{
		ftype distance = (CCamera::activeCamera->transform.position-transform.position).sqrMagnitude();

		if ( distance < sqr(10) ) {
			nextLoD = 0;
		}
		else if ( distance < sqr(40) ) {
			nextLoD = 1;
		}
		else if ( distance < sqr(100) ) {
			nextLoD = 2;
		}
	}

	if ( nextLoD != mLoDLevel ) {
		mLoDLevel = nextLoD;
		//bNeedsRegen = true;
		bNeedsLoDRegen = true;
		bNeedRegen = true;
	}
}

// == Getters and [giga drill] BREEAAAKKKAAHHHHHHHHSSSSSSSSS ==
TreePart* CTreeBase::GetPartClosestTo ( Vector3d vInPos )
{
	// Loop through all the parts, and find the one closest to the midpoint of the part
	float			currentDistance = -1;
	float			nextDistance;
	unsigned int	currentSegment = 0;

	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		// Distance to midpoint
		nextDistance = (vInPos - (transform.position+(part_list[i]->shape.pos)+(part_list[i]->shape.dir*0.5f))).sqrMagnitude();
		// Find smaller part
		if (( nextDistance < currentDistance )||( currentDistance < 0 ))
		{
			currentDistance = nextDistance;
			currentSegment = i;
		}
	}

	return part_list[currentSegment];
}

// == Include Items ==
#include "after/entities/item/material/ItemTreeTrunk.h"
#include "after/entities/item/material/ItemTreeBranch.h"
#include "after/entities/item/material/ItemTreeTwig.h"
#include "after/entities/item/material/ItemTreeResin.h"

// Breaks a part off the tree. Could be possibly optimized if worked with a double link list
// Current worst case is somewhere between O(nlogn) and O(n^2)
void CTreeBase::BreakPart ( TreePart* pInPart, bool bDropItems )
{
	bool found = false;
	
	//vector<vector<TreePart*>::iterator> removeList;
	//vector<unsigned int> removeList;
	vector<TreePart*> removeList;

	// First find the part in the list
	/*for ( vector<TreePart*>::iterator it = part_list.begin(); it != part_list.end(); it++ )
	{
		// Remove it from the list
		// nvm
		if ( (*it) == pInPart )
		{
			// Add it to the remove list
			removeList.push_back( it );
		}
	}*/
	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		// Remove it from the list
		// nvm
		if ( part_list[i] == pInPart )
		{
			// Add it to the remove list
			removeList.push_back( part_list[i] );
		}
	}
	// And remove from existance list
	for ( unsigned int i = 0; i < part_list_full.size(); i++ )
	{
		// Remove it from the list
		if ( part_list_full[i] == pInPart )
		{
			// Put it to the death list
			part_exists[i] = false;
		}
	}

	// Now, go through all the parts in the list, from front to back (as that's the order of the parts, so one pass will work)
	/*for ( vector<TreePart*>::iterator it = part_list.begin(); it != part_list.end(); it++ )
	{
		// Check the delete list
		for ( unsigned int i = 0; i < removeList.size(); i++ )
		{
			// If the parent is in the delete list, add this one to the delete list
			if ( (**removeList[i]) == (*it)->parent )
			{
				removeList.push_back( it );
				i = removeList.size()+1;	// Break out of loop
			}
		}
	}*/

	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		// Check the delete list
		for ( unsigned int j = 0; j < removeList.size(); j++ )
		{
			// If the parent is in the delete list, add this one to the delete list
			if ( part_list[i]->parent == removeList[j] )
			{
				removeList.push_back( part_list[i] );
				j = removeList.size()+1;	// Break out of loop
			}
		}
	}

	// Now delete all the dudes in the delete list
	vector<TreePart*>::iterator it = part_list.begin();
	vector<TreePart*>::iterator prev;
	while ( it != part_list.end() )
	{
		prev = it;
		bool deleted = false;
		// Check the delete list
		for ( unsigned int j = 0; j < removeList.size(); j++ )
		{
			if ( (*it) == removeList[j] )
			{
				// If need to drop items, then drop the item here
				if ( bDropItems )
				{
					ItemTreeTrunk* newItemPart;
					Ray		rnInShape, rnInPShape;
					ftype	fnInSize, fnInPSize;
					if ( (*it)->parent == NULL )
					{
						rnInShape	= (*it)->shape;
						rnInPShape	= rnInShape;
						fnInSize	= (*it)->size;
						fnInPSize	= fnInSize;
					}
					else
					{
						rnInShape	= (*it)->shape;
						rnInPShape	= (*it)->parent->shape;
						fnInSize	= (*it)->size;
						fnInPSize	= (*it)->parent->size;
					}
					// Create new part item
					/*if ( (*it)->size > 0.6f )
						newItemPart = new ItemTreeTrunk( rnInShape, rnInPShape, fnInSize, fnInPSize, pTreeBarkMaterial );
					else if ( (*it)->size > 0.15f )
						newItemPart = new ItemTreeBranch( rnInShape, rnInPShape, fnInSize, fnInPSize, pTreeBarkMaterial );
					else
						newItemPart = new ItemTreeTwig( rnInShape, rnInPShape, fnInSize, fnInPSize, pTreeBarkMaterial );*/
					newItemPart = (ItemTreeTrunk*)CreatePieceItem( (*it)->size, rnInShape, rnInPShape, fnInSize, fnInPSize );
					if ( newItemPart ) {
						// Set new part position and rotation
						newItemPart->transform.position = transform.WorldMatrix() * ((*it)->shape.pos + ((*it)->shape.dir*0.5f));
						newItemPart->transform.rotation = transform.rotation;
					}

					// Chance to drop resin at this point
					if ( random_range(0.0f,1.0f) < (*it)->size*0.4f ) {
						CWeaponItem* newResin = CreateResinItem();
						if ( newResin ) {
							newResin->transform.position = transform.WorldMatrix() * ((*it)->shape.pos + ((*it)->shape.dir*0.5f));
							newResin->transform.rotation = transform.rotation;
						}
					}

					// Turn on priority regeneration
					bRegenPriority = true;
				}

				//If it's in the part list, remove it
				delete (*it);				// Free it
				part_list.erase( it );		// Remove it from the list

				j = removeList.size()+1;	// End the search
				deleted = true;				// Set deleted flag
				it = prev;					// Take the iterator back one
			}
		}
		if ( !deleted )	// If we didn't delete
			it++;		// Increment the iterator
	}

	// Now needs a regeneration
	bNeedCollisionRegen = true;
	bNeedRegen = true;
	bNeedsRegen = true;

	// If there's no parts, should probably kill this tree.
	if ( part_list.size() <= 0 ) {
		RemoveFromTerrain();
		DeleteObject( this );
	}
}

CWeaponItem* CTreeBase::CreatePieceItem ( const ftype fnSize, const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize )
{
	ItemTreeTrunk* newItemPart;
	if ( fnSize > 0.6f )
		newItemPart = new ItemTreeTrunk( rInShape, rInPShape, fInSize, fInPSize, pTreeBarkMaterial );
	else if ( fnSize > 0.15f )
		newItemPart = new ItemTreeBranch( rInShape, rInPShape, fInSize, fInPSize, pTreeBarkMaterial );
	else
		newItemPart = new ItemTreeTwig( rInShape, rInPShape, fInSize, fInPSize, pTreeBarkMaterial );
	return newItemPart;
}
CWeaponItem* CTreeBase::CreateResinItem ( void )
{
	return new ItemTreeResin();
}



// Looks at a bitmask to toggle branch existance
void CTreeBase::SetToggle ( const char * toggleSet )
{
	for ( uint32_t i = 0; i < part_exists.size(); ++i )
	{
		uchar charindex = i/8;
		uchar bitindex = i%8;
		if ( (toggleSet[charindex]&(0x01<<bitindex)) == 0 ) {
			BreakPart( part_list_full[i], false );
		}
	}
}
// Sets a bitmask string correspondin to branch states
void CTreeBase::GetToggle ( char * toggleSet )
{
	memset( toggleSet, 0x00, 48 );
	for ( uint32_t i = 0; i < part_exists.size(); ++i )
	{
		uchar charindex = i/8;
		uchar bitindex = i%8;
		if ( part_exists[i] ) {
			toggleSet[charindex] |= (0x01<<bitindex); // Force true if part exists
		}
	}
}

// == Generate ==
void CTreeBase::Generate ( void )
{
	GenerateTreeData();
	/*GenerateTreeMesh();
	GenerateLeafMesh( leaf_cloud );*/
	bNeedCollisionRegen = true;
	bNeedRegen = true;
	bNeedsRegen = true;

	if ( part_exists.empty() )
	{
		for ( uint32_t i = 0; i < part_list.size(); ++i )
		{
			part_exists.push_back( true );
			part_list_full.push_back( part_list[i] );
		}
	}
	//cout << part_list.size() << endl;
}

#include "Math.h"

void CTreeBase::GenerateTreeData ( void )
{
	vector<bool>		pointCloudSplittable;
	vector<Vector3d>	pointCloud;
	Vector3d position = transform.position;
	float height	  =	16.0f;
	// First, generate the target point cloud at the top
	for ( unsigned int i = 0; i < 20; i++ )
	{
		Vector3d newVect (0,0,0);

		newVect.x = noise->Get3D( (position.x+position.y+position.z)/32.0f - i * 0.08f, position.x/32.0f, i * 0.13f );
		newVect.y = noise->Get3D( position.y/32.0f, i * 0.13f, (position.x+position.y+position.z)/32.0f - i * 0.08f );
		newVect.z = 0;
		if ( newVect.sqrMagnitude() > 1 ) {
			newVect.normalize();
		}
		newVect *= 17.0f;

		newVect.z = noise->Get3D( i * 0.13f, (position.x+position.y+position.z)/32.0f - i * 0.08f, position.z/32.0f );
		newVect.z *= 1.5f;
		newVect.z += height;

		pointCloud.push_back( newVect );
		pointCloudSplittable.push_back( true );
	}

	// Now, starting with a single ray in the queue
	Ray start;
	start.dir = Vector3d( 0,0,2 );
	start.pos = Vector3d( 0,0,0 );

	TreePart* startPart = new TreePart;
	startPart->parent = NULL;
	startPart->shape = start;
	startPart->userData = -1;
	list<TreePart*> rayList;
	rayList.push_front( startPart );

	// Set the travel direction branch split threshold
	float splitThreshold = 0.65f;

	// Set the initial trunk radius
	float branchRadius = 1.0f;

	// Count iterations
	int iterationCount = 0;

	// Continue iterating while the queue is not empty
	while ( !rayList.empty() )
	{
		iterationCount += 1;

		// Get the current ray
		TreePart* currentPart = rayList.back();
		rayList.pop_back();
		Ray current = currentPart->shape;

		// Set the current part's size
		currentPart->size = branchRadius + Math.Sqr<ftype>( std::max<int>( 4 - iterationCount, -1 )*0.2f ) * std::min<ftype>( 1.0f, std::max<ftype>( 0, (8-iterationCount)*0.5f ) );
		currentPart->strength = branchRadius;

		// Decrement the radius for this bit
		branchRadius *= 0.93f;

		// Add current branch to part list
		part_list.push_back( currentPart );
		
		// Move to the next position
		current.pos += current.dir;

		// Increase the split threshold
		splitThreshold = std::min<ftype>( 0.86f,splitThreshold+0.06f );

		// Go through all the points in the cloud and compute attraction
		Vector3d newDir = current.dir * 4.0f;
		bool branchFree = true;
		for ( unsigned int i = 0; i < pointCloud.size(); i++ )
		{
			Vector3d toDir = pointCloud[i]-current.pos;
			float distSqr = toDir.sqrMagnitude();
			Vector3d attraction = (toDir / distSqr)*20.0f;

			// Add attraction to new dir, with some weights based on partial known targets
			if ( currentPart->userData != (signed)i ) {
				//if ( pointCloudSplittable[i] ) {
					newDir += attraction;
				//}
				/*else {
					newDir += attraction*0.6f;
				}*/
			}
			else {
				newDir += attraction * (Real)pointCloud.size() * 0.7f;
			}

		}
		// Slow down vertical movement near the top
		if ( currentPart->userData != -1 ) {
			newDir.z *= std::max<ftype>(0.1f, std::min<ftype>( 1, ((height-current.pos.z)/height)*4.0f ) );
		}
		// Normalize the dir
		newDir = newDir.normal() * 2.0f;
		// Split off
		for ( unsigned int i = 0; i < pointCloud.size(); i++ )
		{
			Vector3d toDir = pointCloud[i]-current.pos;
			float distSqr = toDir.sqrMagnitude();
			Vector3d attraction = (toDir / distSqr)*20.0f;

			Vector3d nextDir = newDir + attraction;

			// Check here to see if the branch should end
			if ( distSqr < sqr( 2+1 ) ) // Stop if too close to the point
			{
				branchFree = false;
			}

			// If we can split towards the current point
			if ( pointCloudSplittable[i] )
			{
				// If the attraction and the current travel direction are too different, then split off
				//if ( attraction.normal().dot( nextDir.normal() ) < splitThreshold )
				ftype splitValue = attraction.normal().dot( nextDir.normal() );
				if ( splitValue > 0.7f && splitValue < splitThreshold )
				{
					// Then we can't split towards this point anymore
					pointCloudSplittable[i] = false;
					// And we add a new travel dir
					Ray newRay = current;
					newRay.dir = nextDir.normal()*1.7f; // do not normalize to get the 'magnet' trees

					TreePart* newPart = new TreePart;
					newPart->shape = newRay;
					newPart->parent = currentPart;
					newPart->userData = i;
					rayList.push_front( newPart );
				}
			}
		}
		
		// Stops branch if too high or too far away from center
		//if ( current.pos.sqrMagnitude() > Math.Sqr( height*1.1f + 2.3f ) ) 
		if ( (currentPart->size < 0.1f) || (current.pos.z > height*1.1f + 1.0f) || (Vector2d(current.pos.x,current.pos.y).sqrMagnitude() > Math.Sqr( height*0.1f + 3.9f )) ) 
		{
			branchFree = false;
		}

		// If the branch should not end, continue
		if ( branchFree )
		{
			// Go through all the points in the rayList and compute repulsion
			for ( list<TreePart*>::iterator it = rayList.begin(); it != rayList.end(); it++ )
			{
				Vector3d toDir = ((*it)->shape.pos+(*it)->shape.dir)-(current.pos+current.dir);
				float distSqr = toDir.sqrMagnitude(); // If there's no attraction from the leaves, this becomes zero, causing a divZERO exception
				Vector3d repulsion = (toDir / distSqr)*0.4f;
				newDir -= repulsion;
			}
			// Normalize the dir
			newDir = newDir.normal() * 1.7f;

			// Take the new dir
			current.dir = newDir;

			// Add the ray to the list
			TreePart* newPart = new TreePart;
			newPart->shape = current;
			newPart->parent = currentPart;
			newPart->userData = currentPart->userData;
			rayList.push_front( newPart );

			// Random chance to add a leaf on a branch
			if ( branchRadius < 0.38f )
			{
				if ( noise->Get( position.x*14.6f + newDir.x, newDir.y*newDir.z*32.0f + part_list.size()*0.7f ) > 0.2f )
				{
					// Add a leaf at the mid of the branch
					leaf_cloud.push_back( current.pos + current.dir*0.5f );
					leaf_branch.push_back( currentPart );
				}
			}
		}
		else
		{
			// Add a leaf at the end of the branch
			leaf_cloud.push_back( current.pos + current.dir );
			leaf_branch.push_back( currentPart );
		}
	}
}

void CTreeBase::GenerateTreeMesh ( void )
{
	int mSides = 3;
	if ( mLoDLevel == 0 ) {
		mSides = 8;
	}
	else if ( mLoDLevel == 1 ) {
		mSides = 6;
	}
	else if ( mLoDLevel == 2 ) {
		mSides = 4;
	}
	//cout << "TREE LOD: " << (int)mLoDLevel << endl;

	// == Update ==
	// Need to count number of endpoints
	bool* ignoreList = new bool [part_list.size()];
	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		ignoreList[i] = false;
	}
	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		if ( part_list[i]->parent != NULL )
		{
			// look for parent
			for ( unsigned int j = 0; j < part_list.size(); j++ )
			{
				// put parent into ignore list
				if ( part_list[j] == part_list[i]->parent )
				{
					ignoreList[j] = true;
				}
			}
		}
	}
	int iEndPointCount = 0;
	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		if ( !ignoreList[i] )
			iEndPointCount++;
	}

	// Now, we want to translate to a mesh.
	CModelData &newModel = modelData;
	newModel.vertexNum = 2 * mSides * part_list.size() + iEndPointCount*(mSides+1);
	//newModel.vertices = new CModelVertex [ newModel.vertexNum ];
	newModel.triangleNum = 2 * mSides * part_list.size() + iEndPointCount*mSides;
	//newModel.triangles = new CModelTriangle [ newModel.triangleNum ];

	ftype angleDiv = (360.0f/mSides);

	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		for ( int j = 0; j < mSides; j++ )
		{
			Vector3d gOffset;

			//Vector3d vAngleVector = Vector3d( (ftype)sin(degtorad(j*angleDiv)), (ftype)cos(degtorad(j*angleDiv)), 1 );

			if ( part_list[i]->parent == NULL )
			{
				//gOffset = part_list[i]->shape.dir.cross( vAngleVector );
				//gOffset = gOffset.normal() * part_list[i]->size;
				Quaternion quat;
				quat.AxisAngle( part_list[i]->shape.dir, -j*angleDiv );
				//gOffset = quat * Vector3d::forward * part_list[i]->size;
				if ( part_list.size() > 1  ) {
					gOffset = quat * Vector3d::forward * (2*part_list[i]->size-part_list[i+1]->size);
				}
				else {
					gOffset = quat * Vector3d::forward * part_list[i]->size;
				}
			}
			else
			{
				//gOffset = part_list[i]->parent->shape.dir.cross( vAngleVector );
				//gOffset = gOffset.normal() * part_list[i]->parent->size;
				Quaternion quat;
				quat.AxisAngle( part_list[i]->parent->shape.dir, -j*angleDiv );
				gOffset = quat * Vector3d::forward * part_list[i]->parent->size;
			}

			Vector3d tOffset;
			//tOffset = part_list[i]->shape.dir.cross( vAngleVector );
			//tOffset = tOffset.normal() * part_list[i]->size;
			{
				Quaternion quat;
				quat.AxisAngle( part_list[i]->shape.dir, -j*angleDiv );
				tOffset = quat * Vector3d::forward * part_list[i]->size;
			}

			//Vector3d pBottom = part_list[i]->shape.pos+transform.position + gOffset;
			//Vector3d pTop = part_list[i]->shape.pos+part_list[i]->shape.dir+transform.position + tOffset;
			Vector3d pBottom = part_list[i]->shape.pos + gOffset;
			Vector3d pTop = part_list[i]->shape.pos+part_list[i]->shape.dir + tOffset;

			unsigned int vertIndex = i*(mSides*2) + j*2;
			newModel.vertices[ vertIndex ].x = pBottom.x;
			newModel.vertices[ vertIndex ].y = pBottom.y;
			newModel.vertices[ vertIndex ].z = pBottom.z;
			newModel.vertices[ vertIndex ].v = pBottom.z*0.25f;
			newModel.vertices[ vertIndex ].u = fabs(1.0f - (j/(ftype)mSides)*2.0f);
			newModel.vertices[ vertIndex ].nx = gOffset.normal().x;
			newModel.vertices[ vertIndex ].ny = gOffset.normal().y;
			newModel.vertices[ vertIndex ].nz = gOffset.normal().z;
			vertIndex++;
			newModel.vertices[ vertIndex ].x = pTop.x;
			newModel.vertices[ vertIndex ].y = pTop.y;
			newModel.vertices[ vertIndex ].z = pTop.z;
			newModel.vertices[ vertIndex ].v = pTop.z*0.25f;
			newModel.vertices[ vertIndex ].u = fabs(1.0f - (j/(ftype)mSides)*2.0f);
			newModel.vertices[ vertIndex ].nx = tOffset.normal().x;
			newModel.vertices[ vertIndex ].ny = tOffset.normal().y;
			newModel.vertices[ vertIndex ].nz = tOffset.normal().z;

			unsigned int triIndex = i*(mSides*2) + j*2;
			newModel.triangles[ triIndex ].vert[0] = triIndex;
			newModel.triangles[ triIndex ].vert[1] = triIndex+1;
			newModel.triangles[ triIndex ].vert[2] = triIndex+2;
			if ( j == mSides-1 )
			{
				newModel.triangles[ triIndex ].vert[2] -= (mSides*2);
			}
			newModel.triangles[ triIndex+1 ].vert[0] = triIndex+1;
			newModel.triangles[ triIndex+1 ].vert[1] = triIndex+3;
			newModel.triangles[ triIndex+1 ].vert[2] = triIndex+2;
			if ( j == mSides-1 )
			{
				newModel.triangles[ triIndex+1 ].vert[1] -= (mSides*2);
				newModel.triangles[ triIndex+1 ].vert[2] -= (mSides*2);
			}
		}
	}

	
	unsigned int iVertIndex = mSides*2 * part_list.size();
	unsigned int iTriIndex	= mSides*2 * part_list.size();
	for ( unsigned int i = 0; i < part_list.size(); i++ )
	{
		if ( !ignoreList[i] )
		{
			// get the part normal
			Vector3d dir = part_list[i]->shape.dir.normal();

			for ( int j = 0; j < mSides; j++ )
			{
				// create the edge point
				Vector3d gOffset;

				//Vector3d vAngleVector = Vector3d( (ftype)sin(degtorad(j*angleDiv)), (ftype)cos(degtorad(j*angleDiv)), 1 );

				if ( part_list[i]->parent == NULL )
				{
					//gOffset = part_list[i]->shape.dir.cross( vAngleVector );
					//gOffset = gOffset.normal() * part_list[i]->size;
					Quaternion quat;
					quat.AxisAngle( part_list[i]->shape.dir, -j*angleDiv );
					gOffset = quat * Vector3d::forward * part_list[i]->size;
				}
				else
				{
					//gOffset = part_list[i]->parent->shape.dir.cross( vAngleVector );
					//gOffset = gOffset.normal() * part_list[i]->parent->size;
					Quaternion quat;
					quat.AxisAngle( part_list[i]->parent->shape.dir, -j*angleDiv );
					gOffset = quat * Vector3d::forward * part_list[i]->parent->size;
				}

				Vector3d tOffset;
				//tOffset = part_list[i]->shape.dir.cross( vAngleVector );
				//tOffset = tOffset.normal() * part_list[i]->size;
				{
					Quaternion quat;
					quat.AxisAngle( part_list[i]->shape.dir, -j*angleDiv );
					tOffset = quat * Vector3d::forward * part_list[i]->size;
				}

				Vector3d pTop = part_list[i]->shape.pos+part_list[i]->shape.dir + tOffset;

				// create the vertices
				newModel.vertices[iVertIndex+j].x = pTop.x;
				newModel.vertices[iVertIndex+j].y = pTop.y;
				newModel.vertices[iVertIndex+j].z = pTop.z;
				newModel.vertices[iVertIndex+j].nx = dir.x;
				newModel.vertices[iVertIndex+j].ny = dir.y;
				newModel.vertices[iVertIndex+j].nz = dir.z;
				newModel.vertices[iVertIndex+j].u = pTop.z*0.011f;
				//newModel.vertices[iVertIndex+j].v = fabs(1.0 - j/3.0);
				newModel.vertices[iVertIndex+j].v = fabs( 1.0f - (j/(ftype)mSides)*2.0f )*0.1f + 0.45f;

				// create the triangles
				if ( j != mSides-1 )
				{
					newModel.triangles[ iTriIndex+j ].vert[1] = iVertIndex+j;
					newModel.triangles[ iTriIndex+j ].vert[0] = iVertIndex+j+1;
					newModel.triangles[ iTriIndex+j ].vert[2] = iVertIndex+mSides;
				}
				else
				{
					newModel.triangles[ iTriIndex+j ].vert[1] = iVertIndex+j;
					newModel.triangles[ iTriIndex+j ].vert[0] = iVertIndex;
					newModel.triangles[ iTriIndex+j ].vert[2] = iVertIndex+mSides;
				}

			}
			// create the center vertex
			newModel.vertices[iVertIndex+mSides].x = part_list[i]->shape.pos.x+part_list[i]->shape.dir.x;
			newModel.vertices[iVertIndex+mSides].y = part_list[i]->shape.pos.y+part_list[i]->shape.dir.y;
			newModel.vertices[iVertIndex+mSides].z = part_list[i]->shape.pos.z+part_list[i]->shape.dir.z;
			newModel.vertices[iVertIndex+mSides].nx = dir.x;
			newModel.vertices[iVertIndex+mSides].ny = dir.y;
			newModel.vertices[iVertIndex+mSides].nz = dir.z;
			//newModel.vertices[iVertIndex+6].u = part_list[i]->shape.pos.z*0.014f;
			//newModel.vertices[iVertIndex+6].u = part_list[i]->shape.pos.z*0.034f;
			newModel.vertices[iVertIndex+mSides].u = sqrtf( fabs( part_list[i]->size ) );
			newModel.vertices[iVertIndex+mSides].v = 0.5f;

			// increment to next vert
			iVertIndex	+= mSides+1;
			iTriIndex	+= mSides;
		}
	}
	delete [] ignoreList;

	// Create the mesh
	/*pTreeModel = new CModel( newModel, string("_sys_override_") );
	pTreeModel->transform.Get( transform );
	pTreeModel->SetMaterial( pTreeBarkMaterial );*/

	if ( bNeedCollisionRegen )
	{
		if ( mSides >= 5 )
		{
			FreeModels(); // Frees collision
			
			// Create collision based on the tree mesh
			//pTreeCollision = new CStaticMeshCollider( pTreeModel );
			pTreeCollision = new CStaticMeshCollider( &modelData );
			pTreeBody = new CRigidBody( pTreeCollision, this, -1.0f );

			bNeedCollisionRegen = false;
		}
	}

	// Set transform to dirty to update the rigidbody
	transform.SetDirty();
	
}

void CTreeBase::GenerateLeafMesh ( vector<Vector3d>& pointCloud )
{
	int mSkipDiv = 3;
	if ( mLoDLevel == 0 ) {
		mSkipDiv = 1;
	}
	else if ( mLoDLevel == 1 ) {
		mSkipDiv = 1;
	}
	else if ( mLoDLevel == 2 ) {
		mSkipDiv = 2;
	}

	int lSkipCount = (pointCloud.size()/mSkipDiv);

	CModelData &newModel = modelData;
	//newModel.vertexNum = 8 * 3 * lSkipCount;
	//newModel.triangleNum = 4 * 3 * lSkipCount;
	newModel.vertexNum = 0;
	newModel.triangleNum = 0;

	// Iterate through each mesh cloud
	//for ( unsigned int i = 0; i < pointCloud.size(); i++ )
	for ( int i = 0; i < lSkipCount; i++ )
	{
		bool exists = false;
		// If the associated branch is existance
		for ( unsigned int j = 0; j < part_list.size(); j++ )
		{
			if ( part_list[j] == leaf_branch[i*mSkipDiv] )
				exists = true;
		}
		if ( !exists )
			continue;
		// With every mesh cloud, create a burst of three leaf planes
		for ( int j = 0; j < 3; j++ )
		{
			// Create the four points
			Vector3d planePoints [4];
			planePoints[0] = Vector3d( -1,-1,0 );
			planePoints[1] = Vector3d( 1,-1,0 );
			planePoints[2] = Vector3d( 1,1,0 );
			planePoints[3] = Vector3d( -1,1,0 );
			//Vector3d normal = Vector3d( 0,0,1 );
			Vector3d normals[4];
			/*normals[0] = Vector3d( -1,-1,3 ).normal();
			normals[1] = Vector3d( 1,-1,3 ).normal();
			normals[2] = Vector3d( 1,1,3 ).normal();
			normals[3] = Vector3d( -1,1,3 ).normal();*/
			normals[0] = Vector3d( -1,-1,0 ).normal();
			normals[1] = Vector3d( 1,-1,0 ).normal();
			normals[2] = Vector3d( 1,1,0 ).normal();
			normals[3] = Vector3d( -1,1,0 ).normal();

			// Set the vertices uv
			for ( int k = 0; k < 4; k++ )
			{
				int vertIndex = i*24 + j*8 + k;

				newModel.vertices[ vertIndex ].u = (planePoints[k].x + 1)*0.5f;
				newModel.vertices[ vertIndex ].v = (planePoints[k].y + 1)*0.5f;

				newModel.vertices[ vertIndex+4 ].u = (planePoints[k].x + 1)*0.5f;
				newModel.vertices[ vertIndex+4 ].v = (planePoints[k].y + 1)*0.5f;
			}

			// Rotate them
			Matrix4x4 rotMatx;
			rotMatx.setRotation( (ftype)30+i*213+j*145, (ftype)30-i*423+j*155, (ftype)30+i*167-j*235 );
			for ( int k = 0; k < 4; k++ )
			{
				planePoints[k] = rotMatx*planePoints[k]*3.3f + pointCloud[i*mSkipDiv];
				normals[k] = rotMatx*normals[k];
			}

			// Set the vertices
			for ( int k = 0; k < 4; k++ )
			{
				//int vertIndex = i*24 + j*8 + k;
				int vertIndex = newModel.vertexNum + k;

				newModel.vertices[ vertIndex ].x = planePoints[k].x;
				newModel.vertices[ vertIndex ].y = planePoints[k].y;
				newModel.vertices[ vertIndex ].z = planePoints[k].z;
				newModel.vertices[ vertIndex ].nx = normals[k].x;
				newModel.vertices[ vertIndex ].ny = normals[k].y;
				newModel.vertices[ vertIndex ].nz = normals[k].z;

				newModel.vertices[ vertIndex+4 ].x = planePoints[k].x;
				newModel.vertices[ vertIndex+4 ].y = planePoints[k].y;
				newModel.vertices[ vertIndex+4 ].z = planePoints[k].z;
				newModel.vertices[ vertIndex+4 ].nx = normals[k].x;
				newModel.vertices[ vertIndex+4 ].ny = normals[k].y;
				newModel.vertices[ vertIndex+4 ].nz = normals[k].z;
			}

			//int triIndex = i*12 + j*4;
			int triIndex = newModel.triangleNum;
			/*newModel.triangles[ triIndex ].vert[0] = i*24 + j*8;
			newModel.triangles[ triIndex ].vert[1] = i*24 + j*8 + 1;
			newModel.triangles[ triIndex ].vert[2] = i*24 + j*8 + 3;*/
			newModel.triangles[ triIndex ].vert[0] = newModel.vertexNum;
			newModel.triangles[ triIndex ].vert[1] = newModel.vertexNum + 1;
			newModel.triangles[ triIndex ].vert[2] = newModel.vertexNum + 3;
			triIndex++;
			/*newModel.triangles[ triIndex ].vert[0] = i*24 + j*8 + 1;
			newModel.triangles[ triIndex ].vert[1] = i*24 + j*8 + 2;
			newModel.triangles[ triIndex ].vert[2] = i*24 + j*8 + 3;*/
			newModel.triangles[ triIndex ].vert[0] = newModel.vertexNum + 1;
			newModel.triangles[ triIndex ].vert[1] = newModel.vertexNum + 2;
			newModel.triangles[ triIndex ].vert[2] = newModel.vertexNum + 3;

			triIndex++;
			/*newModel.triangles[ triIndex ].vert[2] = 4 + i*24 + j*8;
			newModel.triangles[ triIndex ].vert[1] = 4 + i*24 + j*8 + 1;
			newModel.triangles[ triIndex ].vert[0] = 4 + i*24 + j*8 + 3;*/
			newModel.triangles[ triIndex ].vert[2] = 4 + newModel.vertexNum;
			newModel.triangles[ triIndex ].vert[1] = 4 + newModel.vertexNum + 1;
			newModel.triangles[ triIndex ].vert[0] = 4 + newModel.vertexNum + 3;
			triIndex++;
			/*newModel.triangles[ triIndex ].vert[2] = 4 + i*24 + j*8 + 1;
			newModel.triangles[ triIndex ].vert[1] = 4 + i*24 + j*8 + 2;
			newModel.triangles[ triIndex ].vert[0] = 4 + i*24 + j*8 + 3;*/
			newModel.triangles[ triIndex ].vert[2] = 4 + newModel.vertexNum + 1;
			newModel.triangles[ triIndex ].vert[1] = 4 + newModel.vertexNum + 2;
			newModel.triangles[ triIndex ].vert[0] = 4 + newModel.vertexNum + 3;

			newModel.vertexNum += 8;
			newModel.triangleNum += 4;
		}
	}
	// Created mesh
}
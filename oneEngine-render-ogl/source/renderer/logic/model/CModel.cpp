
#include "CModel.h"

#include "core/time/time.h"
#include "core-ext/animation/CAnimation.h"

#include "core-ext/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"

#include "renderer/material/glMaterial.h"
#include "renderer/object/mesh/CMesh.h"
#include "renderer/resource/CModelMaster.h"

using namespace std;

// Constructor, taking model object
CModel::CModel ( const char* sFilename )
	: CLogicObject()
{
	// Create filename
	myModelFilename = sFilename;
	// Standardize the filename
	myModelFilename = IO::FilenameStandardize( myModelFilename );
	// Look for the valid resource to load
	myModelFilename = Core::Resources::PathTo( myModelFilename );
#ifndef _ENGINE_DEBUG
	throw Core::NotYetImplementedException();
#endif

	// Clear out uniform lists
	uniformMapFloat = NULL;
	uniformMapVect2d = NULL;
	uniformMapVect3d = NULL;
	uniformMapColor = NULL;

	// Clear out the material list
	//vMaterials.clear();
	// Initialize animation to null
	pMyAnimation = NULL;

	// Set basic properties
	bUseFrustumCulling = true;
	//bCelShadingEnabled = false;
	//bUseSeparateMaterialBatches = false;
	//visible = false;
	
	// Set animation properties
	pReferencedAnimation = NULL;
	bReferenceAnimation = false;
	eRefMode = ANIM_REF_NONE;

	// First look for the model in the model master
	//const vector<glMesh*> * pMeshSetReference = ModelMaster.GetReference( myModelFilename );
	auto t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
	// If there's no reference, then load it
	if ( t_meshSet == NULL )
	{
		LoadModel( myModelFilename );
		t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
	}
	if ( t_meshSet )
	{
		m_glMeshlist = *t_meshSet;	// Copy the meshes to local list
		Debug::Console->PrintMessage( " +Has mesh set\n" );
	}
	/*if ( pMeshSetReference == NULL )
	{
		LoadModel( myModelFilename );
		// We don't want to use the animation reference we just created, so we make another
		// Look for the reference and copy it
		CAnimation* pFoundReference = ModelMaster.GetAnimationReference( myModelFilename );
		if ( pFoundReference != NULL && pFoundReference->IsValid() ) {
			pMyAnimation = new CAnimation( myModelFilename, pFoundReference );
			pMyAnimation->SetOwner( this );	// Set this as the owner
			ModelMaster.AddReference( myModelFilename, pMyAnimation );
		}
	}
	else // If there is a reference, copy the data
	{
		m_glMeshlist = *pMeshSetReference;
		vHitboxes = *ModelMaster.GetHitboxReference( myModelFilename );
		// Also create a new animation, with a reference that we found ourself
		CAnimation* pFoundReference = ModelMaster.GetAnimationReference( myModelFilename );
		if ( pFoundReference != NULL && pFoundReference->IsValid() ) {
			pMyAnimation = new CAnimation( myModelFilename, pFoundReference );
			pMyAnimation->SetOwner( this ); // Set this model as the owner
			ModelMaster.AddReference( myModelFilename, pMyAnimation );
		}
	}
	// Add to the reference of the model
	ModelMaster.AddReference( myModelFilename, m_glMeshlist, vHitboxes );
	ModelMaster.AddReference( myModelFilename, m_physMeshlist );*/
	
	// Create mesh list
	for ( uint i = 0; i < m_glMeshlist.size(); ++i ) 
	{
		m_meshes.push_back( new CMesh( m_glMeshlist[i] ) );
		m_meshes[i]->m_parent = this;
	}
	

	// Check for a valid animation
	if ( pMyAnimation != NULL && !pMyAnimation->IsValid() )
	{
		delete pMyAnimation;
		pMyAnimation = NULL;
	}

	// Create default list of materials
	/*while ( vMaterials.size() < vMeshes.size() )
	{
		vMaterials.push_back( NULL );
	}
	vMaterials[0] = vMeshes[0]->pmMat;*/

	// Get the bounding box
	//CalculateBoundingBox();
	/*for ( uint i = 0; i < m_meshes.size(); ++i ) {
		//m_meshes[i]->CalculateBoundingBox(); // calculated
		CalculateBoundingBox();
	}*/
	CalculateBoundingBox();
	// Force render
	//bCanRender = true;

	// Check errors
	//GL.CheckError();
}

// Constructor, taking model info struct (nice for procedural generation)
CModel::CModel ( CModelData& mdInModelData, const char* sModelName )
	: CLogicObject()
{
	// Start out setting the model name
	myModelFilename = sModelName;

	// Clear out uniform lists
	uniformMapFloat = NULL;
	uniformMapVect2d = NULL;
	uniformMapVect3d = NULL;
	uniformMapColor = NULL;

	// Clear out the material list
	//vMaterials.clear();
	// Initialize animation to null
	pMyAnimation = NULL;

	// Set basic properties
	bUseFrustumCulling = true;
	//bCelShadingEnabled = false;
	//bUseSeparateMaterialBatches = false;
	//visible = false;

	// First look for the model in the model master
	//const vector<glMesh*> * pMeshSetReference = ModelMaster.GetReference( sModelName );
	auto t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
	// If there's no reference, then load it
	if ( t_meshSet == NULL || myModelFilename == "_sys_override_" )
	{
		CModelData* mdModelData = new CModelData();
		(*mdModelData) = mdInModelData;

		// Create new mesh with the model data
		glMesh* newMesh = new glMesh ();
		newMesh->Initialize( "procedural mesh", mdModelData );

		// Put the mesh into the render list
		m_glMeshlist.push_back( newMesh );
		
		// Create a new name
		if ( sModelName == "_sys_override_" )
		{
			char t_newName [64];
			sprintf( t_newName, "_sys_override_%x", (uint32_t)(this) );
			myModelFilename = t_newName;
		}

		Debug::Console->PrintMessage( " +Adding data for procedural mesh...\n" );
		Debug::Console->PrintMessage( " +Has mesh set\n" );
	}
	else // If there is a reference, copy the data
	{
		m_glMeshlist = *t_meshSet;	// Copy the meshes to local list
		Debug::Console->PrintMessage( " +Has mesh set\n" );
	}
	
	// Add to the reference of the model
	/*ModelMaster.AddReference( sModelName, m_glMeshlist, vHitboxes );
	ModelMaster.AddReference( sModelName, m_physMeshlist );

	// Add animation reference
	CAnimation* pAnimSetReference = ModelMaster.GetAnimationReference( sModelName );
	ModelMaster.AddReference( sModelName, pAnimSetReference );*/

	// Create mesh list
	for ( uint i = 0; i < m_glMeshlist.size(); ++i ) 
	{
		m_meshes.push_back( new CMesh( m_glMeshlist[i] ) );
		m_meshes[i]->m_parent = this;
	}

	// Set the default white material
	/*glMaterial* defaultMat = new glMaterial;
	defaultMat->releaseOwnership();
	vMaterials.push_back( defaultMat );

	// Set the texture ref list to NULL
	pvMaterials = NULL;*/

	// Get the bounding box
	//CalculateBoundingBox();
	CalculateBoundingBox();
	// Force render
	//bCanRender = true;

	// Check errors
	//GL.CheckError();
}


// Destructor
CModel::~CModel ( void )
{
	/*for ( unsigned int n = 0; n < vMaterials.size(); ++n )
	{
		if ( vMaterials[n] )
			vMaterials[n]->releaseOwnership();
	}*/
	for ( uint i = 0; i < m_meshes.size(); ++i ) 
	{
		delete m_meshes[i];
		m_meshes[i] = NULL;
	}
	m_meshes.clear();

	if ( !m_glMeshlist.empty() )
		RenderResources::Active()->ReleaseMeshSet( myModelFilename.c_str() );
	//ModelMaster.RemoveReference( myModelFilename );
	//ModelMaster.RemovePhysReference( myModelFilename );
	// TODO delete materials with no reference

	// Free animation reference
	/*ModelMaster.RemoveAnimSetReference( myModelFilename );*/
	if ( pMyAnimation != NULL )
		delete pMyAnimation;
	pMyAnimation = NULL;

	delete_safe( uniformMapFloat );
	delete_safe( uniformMapVect2d );
	delete_safe( uniformMapVect3d );
	delete_safe( uniformMapColor );
}


// Begin render
void CModel::PreStep ( void ) 
{
	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		m_meshes[i]->transform.Get(transform);
		m_meshes[i]->bUseFrustumCulling = bUseFrustumCulling;
	}
	/*if ( visible )
	{
		for ( uint i = 0; i < m_meshes.size(); ++i )
		{
			m_meshes[i]->visible = true;
		}
		visible = false;
	}*/
}
// End render
void CModel::PostStepSynchronus ( void ) 
{
	if ( pMyAnimation )
	{
		pMyAnimation->UpdateTransform( XTransform( transform.position, transform.rotation, transform.scale ) );
		if ( !bReferenceAnimation )
		{
			pMyAnimation->Update(Time::deltaTime);
		}
		else
		{
			if ( eRefMode = ANIM_REF_DIRECT )
			{
				// Directly copy the transforms
				//pReferencedAnimation->GetAnimationSet
				//vector<void*>& refList = pReferencedAnimation->animRefs;
				// THIS WON'T WORK FOR NORMAL FUCKING ANIMATIONS
			}
		}
	}
}


//#include "renderer/debug/CDebugDrawer.h"

void CModel::CalculateBoundingBox ( void )
{
	Vector3d minPos, maxPos;

	for ( unsigned int i = 0; i < m_glMeshlist.size(); i++ )
	{
		CModelData* pmData = m_glMeshlist[i]->pmData;
		for ( unsigned int v = 0; v < pmData->vertexNum; v++ )
		{
			CModelVertex* vert = &(pmData->vertices[v]);
			minPos.x = std::min<ftype>( minPos.x, vert->x );
			minPos.y = std::min<ftype>( minPos.y, vert->y );
			minPos.z = std::min<ftype>( minPos.z, vert->z );
			maxPos.x = std::max<ftype>( maxPos.x, vert->x );
			maxPos.y = std::max<ftype>( maxPos.y, vert->y );
			maxPos.z = std::max<ftype>( maxPos.z, vert->z );
		}
	}

	vMinExtents = minPos;
	vMaxExtents = maxPos;
	vCheckRenderPos = (minPos+maxPos)/2;
	fCheckRenderDist = (( maxPos-minPos )/2).magnitude();
	//bbCheckRenderBox.Set( transform.GetTransformMatrix(), vMinExtents, vMaxExtents );
	bbCheckRenderBox.Set( Matrix4x4(), vMinExtents, vMaxExtents );
	//bbCheckRenderBox.Set( transform.GetTransformMatrix(), Vector3d( -0.1f,-0.1f,-0.1f ), Vector3d( 0.1f,0.1f,0.1f ) );
}

// ==Shader interface==
void CModel::SetShaderUniform ( const char* sUniformName, float const fInput )
{
	/*if ( !CGameSettings::Active()->b_ro_EnableShaders ) {
		return; // TODO
	}*/
	if ( !uniformMapFloat ) uniformMapFloat = new unordered_map<arstring128,float>;
	(*uniformMapFloat)[arstring128(sUniformName)] = fInput;
}
void CModel::SetShaderUniform ( const char* sUniformName, const Vector2d& fInput )
{
	if ( !uniformMapVect2d ) uniformMapVect2d = new unordered_map<arstring128,Vector2d>;
	(*uniformMapVect2d)[arstring128(sUniformName)] = fInput;
}
void CModel::SetShaderUniform ( const char* sUniformName, const Vector3d& fInput )
{
	if ( !uniformMapVect3d ) uniformMapVect3d = new unordered_map<arstring128,Vector3d>;
	(*uniformMapVect3d)[arstring128(sUniformName)] = fInput;
}
void CModel::SetShaderUniform ( const char* sUniformName, const Color& fInput )
{
	if ( !uniformMapColor ) uniformMapColor = new unordered_map<arstring128,Color>;
	(*uniformMapColor)[arstring128(sUniformName)] = fInput;
}

void CModel::SendShaderUniforms ( void )
{
	if ( glMaterial::current->getUsingShader() ) {
		if ( uniformMapFloat ) {
			for ( auto entry = uniformMapFloat->begin(); entry != uniformMapFloat->end(); ++entry ) {
				glMaterial::current->setUniform( entry->first.c_str(), entry->second );
			}
		}
		if ( uniformMapVect2d ) {
			for ( auto entry = uniformMapVect2d->begin(); entry != uniformMapVect2d->end(); ++entry ) {
				glMaterial::current->setUniform( entry->first.c_str(), entry->second );
			}
		}
		if ( uniformMapVect3d ) {
			for ( auto entry = uniformMapVect3d->begin(); entry != uniformMapVect3d->end(); ++entry ) {
				glMaterial::current->setUniform( entry->first.c_str(), entry->second );
			}
		}
		if ( uniformMapColor ) {
			for ( auto entry = uniformMapColor->begin(); entry != uniformMapColor->end(); ++entry ) {
				glMaterial::current->setUniform( entry->first.c_str(), entry->second );
			}
		}
	}
}

// Set material for meshes
void CModel::SetMaterial ( glMaterial* n_pNewMaterial )
{
	if ( m_meshes.size() == 1 ) {
		m_meshes[0]->SetMaterial( n_pNewMaterial );
	}
	else {
		throw std::exception();
	}
}
// Get material for meshes
glMaterial* CModel::GetMaterial ( void )
{
	if ( m_meshes.size() == 1 ) {
		return m_meshes[0]->GetMaterial();
	}
	else {
		//throw std::exception();
		return NULL;
	}
}

// Recalculate normals
void CModel::RecalculateNormals ( void )
{
	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		m_glMeshlist[i]->RecalculateNormals();
	}
}

// Duplicate materials to a local copy
void CModel::DuplicateMaterials ( void )
{
	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		m_meshes[i]->m_material->removeReference();
		m_meshes[i]->m_material = m_meshes[i]->m_material->copy();
	}
}

// Set if to use frustum culling or not
void CModel::SetFrustumCulling ( bool useCulling ) {
	bUseFrustumCulling = useCulling;
}
// Forces the model to be drawn the next frame
void CModel::SetForcedDraw ( void ) {
	//bCanRender = true;
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		m_meshes[i]->bCanRender = true;
	}
}

// Change the animation mode to reference another model's animation.
// This mode will save memory, but will completely copy the referenced animation
//  with no ability to change this model's animation.
// If the referenced model is deleted, unexpected behavior will occur.
void CModel::SetReferencedAnimationMode ( CModel* pReference, const AnimRefType ref_type )
{
	throw Core::DeprecatedCallException();
	//pReferencedAnimation = pReference->GetAnimation();
	//bReferenceAnimation = true;
	//eRefMode = ref_type;
}

// Hides or shows all of the child meshes
void CModel::SetVisibility ( const bool n_visibility ) {
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		m_meshes[i]->visible = n_visibility;
	}
}
// Hides or shows all of the child meshes
void CModel::SetRenderType ( const Renderer::RenderingType n_type ) {
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		m_meshes[i]->renderType = n_type;
	}
}

//======================================================//
// GETTERS / FINDERS
//======================================================//

// Get the number of meshes this model manages
uint CModel::GetMeshCount ( void ) const
{
	return m_meshes.size();
}
// Gets the mesh with the index
CMesh* CModel::GetMesh ( const uint n_index ) const
{
	return m_meshes[n_index];
}
// Gets the mesh with the name
CMesh* CModel::GetMesh ( const char* n_name ) const
{
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		if ( m_meshes[i]->GetName().find( n_name ) != string::npos ) {
			return m_meshes[i];
		}
	}
	return NULL;
}

// Gets the indicated mesh data in the array
CModelData* CModel::GetModelData ( int iMeshIndex ) const
{
	//return (vMeshes[iMeshIndex]->pmData);
	CMesh* mesh = GetMesh( iMeshIndex );
	if ( mesh ) {
		return mesh->m_glMesh->pmData;
	}
	return NULL;
}

// Returns the first matching mesh with the given name in the array
CModelData* CModel::GetModelDataByName ( const char* nNameMatch ) const 
{
	CMesh* mesh = GetMesh( nNameMatch );
	if ( mesh ) {
		return mesh->m_glMesh->pmData;
	}
	return NULL;
}

// Return the first matching material
glMaterial*	CModel::FindMaterial ( const char* n_name, const int n_offset ) const
{
	int foundCount = 0;
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		if ( string(m_meshes[i]->GetMaterial()->getName()).find( n_name ) != string::npos ) {
			foundCount += 1;
			if ( foundCount > n_offset ) {
				return m_meshes[i]->GetMaterial();
			}
		}
	}
	return NULL;
}

// Gets if any of the meshes are being rendered
bool CModel::GetVisibility ( void )
{
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		if ( m_meshes[i]->GetVisible() ) {
			return true;
		}
	}
	return false;
}
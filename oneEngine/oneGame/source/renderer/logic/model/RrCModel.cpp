#include "RrCModel.h"

#include "core/time/time.h"
#include "core/utils/string.h"
#include "core-ext/animation/AnimationControl.h"

#include "core-ext/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/resources/ResourceManager.h"

#include "renderer/object/mesh/Mesh.h"
#include "renderer/logic/model/RrAnimatedMeshGroup.h"
//#include "renderer/resource/RrCModelMaster.h"
#include "renderer/logic/model/RrModelMasterSubsystem.h"

#include "core-ext/system/io/assets/ModelLoader.h"

RrCModel*
RrCModel::Load ( const char* resource_name )
{
	rrModelLoadParams mlparams;
	mlparams.resource_name = resource_name;
	mlparams.morphs = true;
	mlparams.animation = true;
	mlparams.hitboxes = true;
	mlparams.collision = true;

	return Load(mlparams);
}

RrCModel*
RrCModel::Load ( const rrModelLoadParams& load_params )
{
	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filename:
	arstring256 resource_str_id (load_params.resource_name);
	core::utils::string::ToResourceName(resource_str_id);

	// First, find the model in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeRrMeshGroup, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		RrAnimatedMeshGroup* existingMeshGroup = (RrAnimatedMeshGroup*)existingResource;
		existingMeshGroup->AddReference();

		// Check what is loaded in the mesh group, and load up the rest:
		core::ModelLoader loader;

		if (load_params.morphs && existingMeshGroup->m_morphs == NULL)
		{
			loader.m_loadMorphs = true;
		}
		if ((load_params.hitboxes) && existingMeshGroup->m_skeleton == NULL)
		{
			loader.m_loadSkeleton = true;
		}

		// Load model
		bool model_loaded = loader.LoadModel(load_params.resource_name);
		if (model_loaded == false)
		{
			debug::Console->PrintError("Could not load model resource \"%s\".\n", load_params.resource_name);
			return NULL;
		}

		// Create a new RrCModel with the given RrAnimatedMeshGroup.
		RrCModel* model = new RrCModel(existingMeshGroup, load_params);

		return model;
	}
	else
	{
		// No model. We need to load up a new mesh group.

		// Create a new RrAnimatedMeshGroup.
		core::ModelLoader loader;
		loader.m_loadMesh = true;
		loader.m_loadMorphs = load_params.morphs;
		loader.m_loadAnimation = load_params.animation;
		loader.m_loadActions = load_params.animation;
		loader.m_loadSkeleton = load_params.animation | load_params.hitboxes;

		// Load model
		bool model_loaded = loader.LoadModel(load_params.resource_name);
		if (model_loaded == false)
		{
			//ARCORE_ERROR(0, "Unimplemented RrCModel instantiation.");
			debug::Console->PrintError("Could not load model resource \"%s\".\n", load_params.resource_name);
			return NULL;
		}

		// Create a mesh group that holds the information:
		RrAnimatedMeshGroup* meshGroup = new RrAnimatedMeshGroup();
		{
			ARCORE_ERROR("Unimplemented RrCModel instantiation.");
			return NULL;
		}

		// Add self to the resource system:
		if (resm->Contains(meshGroup) == false)
			resm->Add(meshGroup);

		// Create a new RrCModel with the given RrAnimatedMeshGroup.
		RrCModel* model = new RrCModel(meshGroup, load_params);

		return model;
	}
}

RrCModel*
RrCModel::Upload ( arModelData& model_data )
{
	return NULL;
}

RrCModel::RrCModel ( RrAnimatedMeshGroup* mesh_group, const rrModelLoadParams& params )
	: RrLogicObject(),
	m_animRefMode(kAnimReferenceNone)
{
	;
}
//
//// Constructor, taking model object
//RrCModel::RrCModel ( const char* sFilename )
//	: RrLogicObject()
//{
//	// Create filename
//	myModelFilename = sFilename;
//	// Standardize the filename
//	myModelFilename = IO::FilenameStandardize( myModelFilename );
//	// Look for the valid resource to load
//	myModelFilename = core::Resources::PathTo( myModelFilename );
//#ifndef _ENGINE_DEBUG
//	throw core::NotYetImplementedException();
//#endif
//
//	// Clear out uniform lists
//	//uniformMapFloat = NULL;
//	//uniformMapVect2d = NULL;
//	//uniformMapVect3d = NULL;
//	//uniformMapColor = NULL;
//
//	// Clear out the material list
//	//vMaterials.clear();
//	// Initialize animation to null
//	pMyAnimation = NULL;
//
//	// Set basic properties
//	bUseFrustumCulling = true;
//	//bCelShadingEnabled = false;
//	//bUseSeparateMaterialBatches = false;
//	//visible = false;
//	
//	// Set animation properties
//	pReferencedAnimation = NULL;
//	bReferenceAnimation = false;
//	eRefMode = ANIM_REF_NONE;
//
//	// First look for the model in the model master
//	//const vector<rrMesh*> * pMeshSetReference = ModelMaster.GetReference( myModelFilename );
//	auto t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
//	// If there's no reference, then load it
//	if ( t_meshSet == NULL )
//	{
//		LoadModel( myModelFilename );
//		t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
//	}
//	if ( t_meshSet )
//	{
//		m_glMeshlist = *t_meshSet;	// Copy the meshes to local list
//		debug::Console->PrintMessage( " +Has mesh set\n" );
//	}
//	/*if ( pMeshSetReference == NULL )
//	{
//		LoadModel( myModelFilename );
//		// We don't want to use the animation reference we just created, so we make another
//		// Look for the reference and copy it
//		AnimationControl* pFoundReference = ModelMaster.GetAnimationReference( myModelFilename );
//		if ( pFoundReference != NULL && pFoundReference->IsValid() ) {
//			pMyAnimation = new AnimationControl( myModelFilename, pFoundReference );
//			pMyAnimation->SetOwner( this );	// Set this as the owner
//			ModelMaster.AddReference( myModelFilename, pMyAnimation );
//		}
//	}
//	else // If there is a reference, copy the data
//	{
//		m_glMeshlist = *pMeshSetReference;
//		vHitboxes = *ModelMaster.GetHitboxReference( myModelFilename );
//		// Also create a new animation, with a reference that we found ourself
//		AnimationControl* pFoundReference = ModelMaster.GetAnimationReference( myModelFilename );
//		if ( pFoundReference != NULL && pFoundReference->IsValid() ) {
//			pMyAnimation = new AnimationControl( myModelFilename, pFoundReference );
//			pMyAnimation->SetOwner( this ); // Set this model as the owner
//			ModelMaster.AddReference( myModelFilename, pMyAnimation );
//		}
//	}
//	// Add to the reference of the model
//	ModelMaster.AddReference( myModelFilename, m_glMeshlist, vHitboxes );
//	ModelMaster.AddReference( myModelFilename, m_physMeshlist );*/
//	
//	// Create mesh list
//	for ( uint i = 0; i < m_glMeshlist.size(); ++i ) 
//	{
//		m_meshes.push_back( new CMesh( m_glMeshlist[i] ) );
//		m_meshes[i]->m_parent = this;
//	}
//	
//
//	// Check for a valid animation
//	if ( pMyAnimation != NULL && !pMyAnimation->IsValid() )
//	{
//		delete pMyAnimation;
//		pMyAnimation = NULL;
//	}
//
//	// Create default list of materials
//	/*while ( vMaterials.size() < vMeshes.size() )
//	{
//		vMaterials.push_back( NULL );
//	}
//	vMaterials[0] = vMeshes[0]->pmMat;*/
//
//	// Get the bounding box
//	//CalculateBoundingBox();
//	/*for ( uint i = 0; i < m_meshes.size(); ++i ) {
//		//m_meshes[i]->CalculateBoundingBox(); // calculated
//		CalculateBoundingBox();
//	}*/
//	CalculateBoundingBox();
//	// Force render
//	//bCanRender = true;
//
//	// Check errors
//	//GL.CheckError();
//}
//
//// Constructor, taking model info struct (nice for procedural generation)
//RrCModel::RrCModel ( arModelData& mdInModelData, const char* sModelName )
//	: RrLogicObject()
//{
//	// Start out setting the model name
//	myModelFilename = sModelName;
//
//	// Clear out uniform lists
//	//uniformMapFloat = NULL;
//	//uniformMapVect2d = NULL;
//	//uniformMapVect3d = NULL;
//	//uniformMapColor = NULL;
//
//	// Clear out the material list
//	//vMaterials.clear();
//	// Initialize animation to null
//	pMyAnimation = NULL;
//
//	// Set basic properties
//	bUseFrustumCulling = true;
//	//bCelShadingEnabled = false;
//	//bUseSeparateMaterialBatches = false;
//	//visible = false;
//
//	// First look for the model in the model master
//	//const vector<rrMesh*> * pMeshSetReference = ModelMaster.GetReference( sModelName );
//	auto t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
//	// If there's no reference, then load it
//	if ( t_meshSet == NULL || myModelFilename == "_sys_override_" )
//	{
//		arModelData* mdModelData = new arModelData();
//		(*mdModelData) = mdInModelData;
//
//		// Create new mesh with the model data
//		rrMesh* newMesh = new rrMesh ();
//		newMesh->Initialize( "procedural mesh", mdModelData );
//
//		// Put the mesh into the render list
//		m_glMeshlist.push_back( newMesh );
//		
//		// Create a new name
//		if ( sModelName == "_sys_override_" )
//		{
//			char t_newName [64];
//			sprintf( t_newName, "_sys_override_%x", (uint32_t)(this) );
//			myModelFilename = t_newName;
//		}
//
//		debug::Console->PrintMessage( " +Adding data for procedural mesh...\n" );
//		debug::Console->PrintMessage( " +Has mesh set\n" );
//	}
//	else // If there is a reference, copy the data
//	{
//		m_glMeshlist = *t_meshSet;	// Copy the meshes to local list
//		debug::Console->PrintMessage( " +Has mesh set\n" );
//	}
//	
//	// Add to the reference of the model
//	/*ModelMaster.AddReference( sModelName, m_glMeshlist, vHitboxes );
//	ModelMaster.AddReference( sModelName, m_physMeshlist );
//
//	// Add animation reference
//	AnimationControl* pAnimSetReference = ModelMaster.GetAnimationReference( sModelName );
//	ModelMaster.AddReference( sModelName, pAnimSetReference );*/
//
//	// Create mesh list
//	for ( uint i = 0; i < m_glMeshlist.size(); ++i ) 
//	{
//		m_meshes.push_back( new CMesh( m_glMeshlist[i] ) );
//		m_meshes[i]->m_parent = this;
//	}
//
//	// Set the default white material
//	/*RrMaterial* defaultMat = new RrMaterial;
//	defaultMat->releaseOwnership();
//	vMaterials.push_back( defaultMat );
//
//	// Set the texture ref list to NULL
//	pvMaterials = NULL;*/
//
//	// Get the bounding box
//	//CalculateBoundingBox();
//	CalculateBoundingBox();
//	// Force render
//	//bCanRender = true;
//
//	// Check errors
//	//GL.CheckError();
//}


// Destructor
RrCModel::~RrCModel ( void )
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

	//if ( !m_glMeshlist.empty() )
	//	RenderResources::Active()->ReleaseMeshSet( myModelFilename.c_str() );
	//ModelMaster.RemoveReference( myModelFilename );
	//ModelMaster.RemovePhysReference( myModelFilename );
	// TODO delete materials with no reference

	// Free animation reference
	/*ModelMaster.RemoveAnimSetReference( myModelFilename );*/
	if ( pMyAnimation != NULL )
		delete pMyAnimation;
	pMyAnimation = NULL;

	//delete_safe( uniformMapFloat );
	//delete_safe( uniformMapVect2d );
	//delete_safe( uniformMapVect3d );
	//delete_safe( uniformMapColor );
}


// Begin render
void RrCModel::PreStep ( void ) 
{
	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		//m_meshes[i]->transform.Get(transform);
		m_meshes[i]->transform.world = transform;
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
void RrCModel::PostStepSynchronus ( void ) 
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
			if ( m_animRefMode == kAnimReferenceDirect )
			{
				// Directly copy the transforms
				//pReferencedAnimation->GetAnimationSet
				//vector<void*>& refList = pReferencedAnimation->animRefs;
				// THIS WON'T WORK FOR NORMAL FUCKING ANIMATIONS
			}
		}
	}
}


//#include "renderer/debug/RrDebugDrawer.h"

void RrCModel::CalculateBoundingBox ( void )
{
	Vector3f minPos, maxPos;

	for ( size_t i = 0; i < m_meshGroup->m_meshCount; i++ )
	{
		arModelData* modeldata = m_meshGroup->m_meshes[i]->m_modeldata;
		for ( size_t v = 0; v < modeldata->vertexNum; v++ )
		{
			//arModelVertex* vert = &(modeldata->vertices[v]);
			Vector3f* vert = &(modeldata->position[v]);
			minPos.x = std::min<Real>( minPos.x, vert->x );
			minPos.y = std::min<Real>( minPos.y, vert->y );
			minPos.z = std::min<Real>( minPos.z, vert->z );
			maxPos.x = std::max<Real>( maxPos.x, vert->x );
			maxPos.y = std::max<Real>( maxPos.y, vert->y );
			maxPos.z = std::max<Real>( maxPos.z, vert->z );
		}
	}

	vMinExtents = minPos;
	vMaxExtents = maxPos;
	vCheckRenderPos = (minPos+maxPos)/2;
	fCheckRenderDist = (( maxPos-minPos )/2).magnitude();
	//bbCheckRenderBox.Set( transform.GetTransformMatrix(), vMinExtents, vMaxExtents );
	m_renderBoundingBox.Set( Matrix4x4(), vMinExtents, vMaxExtents );
	//bbCheckRenderBox.Set( transform.GetTransformMatrix(), Vector3f( -0.1f,-0.1f,-0.1f ), Vector3f( 0.1f,0.1f,0.1f ) );
}

// ==Shader interface==
//void RrCModel::SetShaderUniform ( const char* sUniformName, float const fInput )
//{
//	/*if ( !CGameSettings::Active()->b_ro_EnableShaders ) {
//		return; // TODO
//	}*/
//	if ( !uniformMapFloat ) uniformMapFloat = new unordered_map<arstring128,float>;
//	(*uniformMapFloat)[arstring128(sUniformName)] = fInput;
//}
//void RrCModel::SetShaderUniform ( const char* sUniformName, const Vector2f& fInput )
//{
//	if ( !uniformMapVect2d ) uniformMapVect2d = new unordered_map<arstring128,Vector2f>;
//	(*uniformMapVect2d)[arstring128(sUniformName)] = fInput;
//}
//void RrCModel::SetShaderUniform ( const char* sUniformName, const Vector3f& fInput )
//{
//	if ( !uniformMapVect3d ) uniformMapVect3d = new unordered_map<arstring128,Vector3f>;
//	(*uniformMapVect3d)[arstring128(sUniformName)] = fInput;
//}
//void RrCModel::SetShaderUniform ( const char* sUniformName, const Color& fInput )
//{
//	if ( !uniformMapColor ) uniformMapColor = new unordered_map<arstring128,Color>;
//	(*uniformMapColor)[arstring128(sUniformName)] = fInput;
//}

//void RrCModel::SendShaderUniforms ( void )
//{
//	if ( RrMaterial::current->getUsingShader() ) {
//		if ( uniformMapFloat ) {
//			for ( auto entry = uniformMapFloat->begin(); entry != uniformMapFloat->end(); ++entry ) {
//				RrMaterial::current->setUniform( entry->first.c_str(), entry->second );
//			}
//		}
//		if ( uniformMapVect2d ) {
//			for ( auto entry = uniformMapVect2d->begin(); entry != uniformMapVect2d->end(); ++entry ) {
//				RrMaterial::current->setUniform( entry->first.c_str(), entry->second );
//			}
//		}
//		if ( uniformMapVect3d ) {
//			for ( auto entry = uniformMapVect3d->begin(); entry != uniformMapVect3d->end(); ++entry ) {
//				RrMaterial::current->setUniform( entry->first.c_str(), entry->second );
//			}
//		}
//		if ( uniformMapColor ) {
//			for ( auto entry = uniformMapColor->begin(); entry != uniformMapColor->end(); ++entry ) {
//				RrMaterial::current->setUniform( entry->first.c_str(), entry->second );
//			}
//		}
//	}
//}

// Set material for meshes
//void RrCModel::SetMaterial ( RrMaterial* n_pNewMaterial )
//{
//	if ( m_meshes.size() == 1 ) {
//		m_meshes[0]->SetMaterial( n_pNewMaterial );
//	}
//	else {
//		throw std::exception();
//	}
//}
//// Get material for meshes
//RrMaterial* RrCModel::GetMaterial ( void )
//{
//	if ( m_meshes.size() == 1 ) {
//		return m_meshes[0]->GetMaterial();
//	}
//	else {
//		//throw std::exception();
//		return NULL;
//	}
//}

// Recalculate normals
//void RrCModel::RecalculateNormals ( void )
//{
//	for ( uint i = 0; i < m_meshes.size(); ++i )
//	{
//		m_meshes[i]->RecalculateNormals();
//	}
//}

// Duplicate materials to a local copy
/*void RrCModel::DuplicateMaterials ( void )
{
	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		m_meshes[i]->m_material->removeReference();
		m_meshes[i]->m_material = m_meshes[i]->m_material->copy();
	}
}*/

// Set if to use frustum culling or not
void RrCModel::SetFrustumCulling ( bool useCulling ) {
	bUseFrustumCulling = useCulling;
}
// Forces the model to be drawn the next frame
void RrCModel::SetForcedDraw ( void ) {
	//bCanRender = true;
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		m_meshes[i]->bCanRender = true;
	}
}

// Change the animation mode to reference another model's animation.
// This mode will save memory, but will completely copy the referenced animation
//  with no ability to change this model's animation.
// If the referenced model is deleted, unexpected behavior will occur.
void RrCModel::SetReferencedAnimationMode ( RrCModel* pReference, const rrAnimReferenceType ref_type )
{
	throw core::DeprecatedCallException();
	//pReferencedAnimation = pReference->GetAnimation();
	//bReferenceAnimation = true;
	//eRefMode = ref_type;
}

// Hides or shows all of the child meshes
void RrCModel::SetVisibility ( const bool n_visibility ) {
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		m_meshes[i]->visible = n_visibility;
	}
}
// Hides or shows all of the child meshes
//void RrCModel::SetRenderType ( const renderer::eRenderLayer n_type ) {
//	for ( uint i = 0; i < m_meshes.size(); ++i ) {
//		m_meshes[i]->renderLayer = n_type;
//	}
//}

//======================================================//
// GETTERS / FINDERS
//======================================================//

// Get the number of meshes this model manages
size_t RrCModel::GetMeshCount ( void ) const
{
	return m_meshes.size();
}
// Gets the mesh with the index
renderer::Mesh* RrCModel::GetMesh ( const uint n_index ) const
{
	return m_meshes[n_index];
}
// Gets the mesh with the name
renderer::Mesh* RrCModel::GetMesh ( const char* n_name ) const
{
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		if ( strstr( m_meshes[i]->GetName(), n_name ) != NULL ) {
			return m_meshes[i];
		}
	}
	return NULL;
}

// Gets the indicated mesh data in the array
arModelData* RrCModel::GetModelData ( int iMeshIndex ) const
{
	renderer::Mesh* mesh = GetMesh( iMeshIndex );
	if ( mesh ) {
		return mesh->m_mesh->m_modeldata;
	}
	return NULL;
}

// Returns the first matching mesh with the given name in the array
arModelData* RrCModel::GetModelDataByName ( const char* nNameMatch ) const 
{
	renderer::Mesh* mesh = GetMesh( nNameMatch );
	if ( mesh ) {
		return mesh->m_mesh->m_modeldata;
	}
	return NULL;
}

// Return the first matching material
//RrMaterial*	RrCModel::FindMaterial ( const char* n_name, const int n_offset ) const
//{
//	int foundCount = 0;
//	for ( uint i = 0; i < m_meshes.size(); ++i ) {
//		if ( string(m_meshes[i]->GetMaterial()->getName()).find( n_name ) != string::npos ) {
//			foundCount += 1;
//			if ( foundCount > n_offset ) {
//				return m_meshes[i]->GetMaterial();
//			}
//		}
//	}
//	return NULL;
//}

// Gets if any of the meshes are being rendered
bool RrCModel::GetVisibility ( void )
{
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		if ( m_meshes[i]->GetVisible() ) {
			return true;
		}
	}
	return false;
}
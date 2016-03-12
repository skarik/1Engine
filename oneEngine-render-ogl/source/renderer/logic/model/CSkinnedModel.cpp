
#include "CSkinnedModel.h"

#include "core/time/time.h"
#include "core/settings/CGameSettings.h"
#include "core/math/Math.h"

#include "core-ext/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"

#include "core-ext/profiler/CTimeProfiler.h"

#include "core-ext/animation/CAnimation.h"
#include "physical/animation/CHKAnimation.h"

#include "renderer/system/glMainSystem.h"

#include "renderer/resource/CModelMaster.h"
#include "renderer/object/mesh/CMesh.h"
#include "renderer/object/mesh/system/glSkinnedMesh.h"

#include "renderer/logic/model/morpher/CMorpher.h"

#include "renderer/debug/CDebugDrawer.h"

//#include <boost/lexical_cast.hpp>

// Constructor
CSkinnedModel::CSkinnedModel( const string &sFilename )
	: CModel()
{
	std::cout << "Loading mesh " << sFilename << std::endl;
	// Set model name to input filename
	myModelFilename = sFilename;
	// Standardize the filename
	myModelFilename = IO::FilenameStandardize( myModelFilename );
	// Look for the valid resource to load
	myModelFilename = Core::Resources::PathTo( myModelFilename );
#ifndef _ENGINE_DEBUG
	throw Core::NotYetImplementedException();
#endif

	// Clear out the material list
	//vMaterials.clear();
	// Set basic properties
	bUseFrustumCulling = true;
	//bCelShadingEnabled = false;
	//bUseSeparateMaterialBatches = false;
	bDrawSkeleton = true;

	// Set animation properties
	pReferencedAnimation = NULL;
	bReferenceAnimation = false;
	eRefMode = ANIM_REF_NONE;

	// Initialize morpher objects
	bDoMorphing = false;
	iMorphTarget = 0;
	pMorpher = NULL;

	// Set skeleton properties
	pReferencedSkeleton = NULL;
	bReferenceSkeleton = false;
	bPerfectReference = false;

	// First look for the model in the model master
	const std::vector<glMesh*> * pMeshSetReference = ModelMaster.GetReference( myModelFilename );
	// If there's no reference, then load it
	if ( pMeshSetReference == NULL )
	{
		LoadSkinnedModel( myModelFilename );
		// Copy the skeleton list and leave it alone. We don't want to use the mesh's skeleton.
		std::vector<skeletonBone_t*>* skellyList = new std::vector<skeletonBone_t*>;
		(*skellyList) = vSkeleton;
		for ( unsigned int n = 0; n < m_glMeshlist.size(); ++n ) {
			((glSkinnedMesh*)m_glMeshlist[n])->SetSkeleton( skellyList );
		}
		vSkeleton.clear();
		// Create a new mesh set
		pvOriginalMeshSet = new std::vector<glMesh*>;
		(*pvOriginalMeshSet) = m_glMeshlist;	// Copy the data from the meshes to the new set
	}
	else // If there is a reference, copy the data
	{
		m_glMeshlist = *pMeshSetReference;	// Copy the mesh to local list
		pvOriginalMeshSet = new std::vector<glMesh*>;
		*pvOriginalMeshSet = *pMeshSetReference;	// Point original set over

		vHitboxes = *ModelMaster.GetHitboxReference( myModelFilename );
	}

	// Duplicate the mesh streams
	CopyMeshStreams( &m_glMeshlist );

	// Add to the reference of the model
	ModelMaster.AddReference( myModelFilename, *pvOriginalMeshSet, vHitboxes );	// Add reference to original mesh set
	ModelMaster.AddReference( myModelFilename, m_physMeshlist );

	// Grab a copy of the skeleton
	GrabSkeletonCopy( ((glSkinnedMesh*)m_glMeshlist[0])->GetSkeleton() );
	// Create a new animation, which will look for the reference and copy it
	CAnimation* pFoundReference = ModelMaster.GetAnimationReference( myModelFilename );
	if ( pFoundReference == NULL ) {
		throw Core::InvalidCallException();
	}
	else {
		ModelMaster.AddReference( myModelFilename, pFoundReference );
	}
	if ( !CAnimation::useHavok ) {
		pMyAnimation = new CAnimation( myModelFilename, pFoundReference );
	}
	else {
		pMyAnimation = new CHKAnimation( myModelFilename, pFoundReference );
	}
	pMyAnimation->SetOwner( this );
	// Send the animation our references as well
	SendSkeletonReferences();

	// Check for a valid animation
	if ( !pMyAnimation->IsValid() ) {
		delete pMyAnimation;
		pMyAnimation = NULL;
	}
	else {
		// Play zero animation to start
		(*pMyAnimation)[0].Play();
	}

	// Search for a morph to use
	CMorpher* possibleMorpher = ModelMaster.GetMorpherReference( myModelFilename );
	if ( possibleMorpher ) {
		// If there is a morpher, then load it up
		bDoMorphing = true;
		iMorphTarget = possibleMorpher->GetMorpherSet()->iMorphTarget;
		// Create a new morpher, which will look for the reference and copy it
		pMorpher = new CMorpher( myModelFilename, possibleMorpher );
		ModelMaster.AddReference( myModelFilename, pMorpher );
		std::cout << " +Has morphs" << std::endl;
	}

	// Create mesh list
	for ( uint i = 0; i < m_glMeshlist.size(); ++i ) 
	{
		m_meshes.push_back( new CMesh( m_glMeshlist[i], true ) );
		m_meshes[i]->m_parent = this;
	}

	// Create default list of materials
	/*while ( vMaterials.size() < vMeshes.size() )
	{
		vMaterials.push_back( NULL );
	}
	vMaterials[0] = vMeshes[0]->pmMat;*/

	// Get the bounding box
	CalculateBoundingBox();
	// Increase culling distance
	fCheckRenderDist *= 2;

	// Check errors
	GL_ACCESS GL.CheckError();

	std::cout << " +Loaded mesh" << std::endl;
}

// Destructor
CSkinnedModel::~CSkinnedModel ( void )
{
	//cout << "Entering ~CSkinnedModel.";

	// Free the created mesh streams
	FreeMeshStreams( &m_glMeshlist );
	//cout << " Freed mesh streams.";

	// The model references should decrement automatically from CModel's destructor

	// Delete the created copied skeleton
	while ( !vSkeleton.empty() ) {
		delete vSkeleton[vSkeleton.size()-1];
		vSkeleton.pop_back();
	}

	//cout << " Freed skeleton instance." << endl;

	//cout << "     Leaving ~CSkinnedModel" << endl;
}

// Copy skeleton
void CSkinnedModel::GrabSkeletonCopy ( std::vector<skeletonBone_t*>* pvOldSkeleton )
{
	skeletonBone_t* newBone;
	// For each bone, create a copy
	for ( unsigned int i = 0; i < pvOldSkeleton->size(); i++ )
	{
		newBone = new skeletonBone_t( NULL );
		// copy bind pose and all
		(*newBone) = (*((*pvOldSkeleton)[i]));

		// thats it
		vSkeleton.push_back( newBone );
	}

	// Loop through the current bones
	/*for ( unsigned int k = 0; k < vSkeleton.size(); k++ )
	{
		vSkeleton[k]->transform.LateUpdate();
	}*/

	// Now, link up bones
	for ( unsigned int i = 0; i < pvOldSkeleton->size(); i++ )
	{
		// Look at old skelly
		glBone* oldBone = ((*pvOldSkeleton)[i]);

		// Look at the children
		for ( unsigned int j = 0; j < oldBone->transform.children.size(); j++ )
		{
			// now for each child, find the corresponding bone
			// get the bone assiociated with the child
			skeletonBone_t* childBone = ((skeletonBone_t*)(oldBone->transform.children[j]->owner));

			// Loop through the current bones
			for ( unsigned int k = 0; k < vSkeleton.size(); k++ )
			{
				// if the indices match
				if ( vSkeleton[k]->index == childBone->index )
				{
					// add the child
					vSkeleton[k]->transform.SetParent( &(vSkeleton[i]->transform) );
					vSkeleton[k]->transform.LateUpdate();
					// break out
					k = vSkeleton.size()+1;
				}
			}
		}

	}

	// Loop through the current bones
	/*for ( unsigned int k = 0; k < vSkeleton.size(); k++ )
	{
		vSkeleton[k]->transform.LateUpdate();
	}*/

	// For each bone, create a copy
	for ( unsigned int i = 0; i < pvOldSkeleton->size(); i++ )
	{
		// copy bind pose and all
		(*(vSkeleton[i])) = (*((*pvOldSkeleton)[i]));
	}

	// Set root bone
	rootBone = vSkeleton[0];

	char tempstring [64];
	sprintf( tempstring, "%p", rootBone );
	//Debug::Console->PrintError( boost::lexical_cast<string>(rootBone) );
	Debug::Console->PrintError( tempstring );
}
// Send skeleton referneces
void CSkinnedModel::SendSkeletonReferences ( void )
{
	// And set the pointer map for the animation
	std::vector<void*> voidMap;
	for ( unsigned int i = 0; i < vSkeleton.size(); i++ )
	{
		//voidMap.push_back( (void*)(&(vSkeleton[i]->tempMatx)) );
		voidMap.push_back( (void*)(&(vSkeleton[i]->animTransform)) );
	}
	pMyAnimation->AssignReferenceList( voidMap );

	// Build the skeleton if we're in Havok mode
	if ( CAnimation::useHavok )
	{
		((CHKAnimation*)pMyAnimation)->SetSkeleton( rootBone, vSkeleton );
		((CHKAnimation*)pMyAnimation)->GetRagdollPose( rootBone, vSkeleton );
	}
}

void CSkinnedModel::CopyMeshStreams ( std::vector<glMesh*> *psrcStream )
{
	// Need to duplicate the mesh, keeping the same mesh data, but using unique stream values.
	std::vector<glMesh*> newStream;

	// Loop through the source stream, and copy each mesh
	for ( std::vector<glMesh*>::iterator meshIter = psrcStream->begin(); meshIter != psrcStream->end(); ++meshIter )
	{
		glSkinnedMesh* srcSkinnedMesh = (glSkinnedMesh*)(*meshIter);

		glSkinnedMesh* destMesh = new glSkinnedMesh();
		destMesh->CopyStream( srcSkinnedMesh ); // Copy the stream
		newStream.push_back( destMesh );
	}

	// Clear the mesh list
	m_glMeshlist.clear();
	// Copy the new stream over
	m_glMeshlist = newStream;
}
void CSkinnedModel::FreeMeshStreams ( std::vector<glMesh*> *psrcStream )
{
	// Loop through the source stream, and copy each mesh
	for ( std::vector<glMesh*>::iterator meshIter = psrcStream->begin(); meshIter != psrcStream->end(); ++meshIter )
	{
		glSkinnedMesh* srcSkinnedMesh = (glSkinnedMesh*)(*meshIter);
		srcSkinnedMesh->FreeStream();
	}
}

// Nonsynchronous step
void CSkinnedModel::PreStep ( void )
{
	// Update the mesh objects
	CModel::PreStep();

	// Work on the bone references
	if ( bReferenceSkeleton )
	{
		if ( !bPerfectReference )
		{
			/*for ( uint i = 0; i < vSkeleton.size(); ++i )
			{
				//vSkeleton[i]->transform.SetLocalTransform( vSkeleton[i]->xBindPose );
				vSkeleton[i]->transform.SetLocalTransform( vSkeleton[i]->animTransform );
			}*/

			for ( auto boneRef = mBoneMismatchList.begin(); boneRef != mBoneMismatchList.end(); ++boneRef )
			{
				(*boneRef)->transform.SetLocalTransform( (*boneRef)->animTransform );
			}
		}
		std::map<skeletonBone_t*,skeletonBone_t*>::iterator refSet;
		for ( refSet = mBoneReferenceMap.begin(); refSet != mBoneReferenceMap.end(); ++refSet )
		{
			//fnl_assert( refSet->first != NULL && refSet->second != NULL );
			// TODO: Measure difference between refpose and curpose to apply changes.
			//refSet->first->transform.Get( refSet->second->transform ); // Copy the transform over
			if ( refSet->first && refSet->second ) {
				refSet->first->transform.Get( refSet->second->transform ); // Copy the transform over
				//refSet->first->transform.rotation = refSet->second->transform.rotation; // Copy the transform over
				//refSet->first->transform.localRotation = refSet->second->transform.localRotation; // Copy the transform over
			}
		}
		UpdateSkeleton( false );
		for ( refSet = mBoneReferenceMap.begin(); refSet != mBoneReferenceMap.end(); ++refSet )
		{
			//refSet->first->tempMatx = refSet->second->tempMatx; // Copy the pose over (we don't copy this one over as to not upset smooth effects)
			//refSet->first->currentPose = refSet->second->currentPose; // Copy the pose over to grab jiggle effects
			//refSet->first->transform.SetLocalTransform( refSet->second->transform.localPosition, refSet->second->transform.localRotation, refSet->second->transform.localScale );
			if ( refSet->first && refSet->second ) {
				refSet->first->currentPose = refSet->second->currentPose; // Copy the pose over to grab jiggle effects
			}
		}
		//UpdateSkeleton();
		if ( !bPerfectReference )
		{
			for ( auto boneRef = mBoneMismatchList.begin(); boneRef != mBoneMismatchList.end(); ++boneRef )
			{
				(*boneRef)->transform.LateUpdate();
				//((glBone*)(cur_transform->pOwnerRenderer))->UpdatePose();
				(*boneRef)->UpdatePose();
			}
			/*for ( uint i = 0; i < vSkeleton.size(); ++i )
			{
				vSkeleton[i]->transform.SetLocalTransform( vSkeleton[i]->animTransform );
			}*/
		}
		//UpdateSkeleton();
	}
	else
	{
		UpdateSkeleton();	// approximately 2 ms on a 67-bone skeleton
	}

	// Update the jigglebones
	UpdateJigglebones();
}

void CSkinnedModel::PreStepSynchronus ( void )
{
	TimeProfiler.BeginTimeProfile( "rs_skinned_model_prestep1" );

	// Rendering debug skeletons is on
	if ( bDrawSkeleton )
	{
		DebugRenderSkeleton( rootBone );
	}

	// Grab current skeleton mode
	auto renderMode = ((glSkinnedMesh*)m_glMeshlist[0])->iRenderMode;

	// Check visiblity on child meshes to check if will render
	bool willRender = false;
	for ( auto it = m_meshes.begin(); it != m_meshes.end(); ++it )
	{
		if ( (*it)->GetCanRender() ) {
			willRender = true;
			break;
		}
	}

	if ( willRender )
	{
		// Before skinning and morphs, prepare the stream
		if ( renderMode == glSkinnedMesh::RD_CPU )
		{
			for ( std::vector<glMesh*>::iterator it = m_glMeshlist.begin(); it != m_glMeshlist.end(); it++ )
			{
				glSkinnedMesh* mesh = ((glSkinnedMesh*)(*it));
				mesh->PrepareStream();
			}
		}
		else if ( renderMode == glSkinnedMesh::RD_GPU )
		{	// Only morpher needs to stream vertices
			if ( bDoMorphing && m_meshes[iMorphTarget]->GetCanRender() )
			{
				((glSkinnedMesh*)m_glMeshlist[iMorphTarget])->PrepareStream();
			}
		}

		// Before skinning, do morphs on the target
		if ( bDoMorphing && m_meshes[iMorphTarget]->GetCanRender() )
		{
			//pMorpher->FindAction("blink")->weight = fabs(sinf(Time::currentTime*2.0f))*0.10f;
			//(*pMorpher)[0].weight = fabs(sinf(Time::currentTime*2.0f))*0.10f;
			pMorpher->PerformMorph( (glSkinnedMesh*)(m_glMeshlist[iMorphTarget]) );
		}

		//TimeProfiler.BeginTimeProfile( "rs_skinned_model_begin_x" );
		GL_ACCESS;
		// Update the meshes
		for ( std::vector<glMesh*>::iterator it = m_glMeshlist.begin(); it != m_glMeshlist.end(); it++ )
		{
			glSkinnedMesh* mesh = ((glSkinnedMesh*)(*it));
			mesh->SetSkeleton( &vSkeleton );
			if ( CGameSettings::Active()->b_dbg_ro_EnableSkinning ) 
			{
				GL.ThreadGrabLock();
				if ( renderMode == glSkinnedMesh::RD_CPU )
				{
					mesh->StartSkinning();
					mesh->UpdateVBO();
				}
				else if ( renderMode == glSkinnedMesh::RD_GPU )
				{
					// Morphing needs full CPU streaming done
					if ( bDoMorphing && m_glMeshlist[iMorphTarget] == mesh && m_meshes[iMorphTarget]->GetCanRender() )
					{
						mesh->StartSkinning();
						mesh->UpdateVBO();
					}
					// Otherwise, only the first mesh needs to be streaming
					else if ( it == m_glMeshlist.begin() )
					{
						mesh->StartSkinning();
					}
					else
					{
						// Send softbody
						mesh->StartSkinning();

						//mesh->m_textureBufferData = ((glSkinnedMesh*)m_glMeshlist[0])->getTextureBuffer();
						//mesh->m_textureSoftBuffer = ((glSkinnedMesh*)m_glMeshlist[0])->getSoftTextureBuffer();
					}
					// And turn off the streaming
					mesh->newStreamReady = false;
				}
				GL.ThreadReleaseLock();
			}
		}
		//TimeProfiler.EndAddTimeProfile( "rs_skinned_model_begin_x" );
	}

	//m_textureBuffer
	/*for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		m_meshes[i]->m_material->m_bufferMatricesSkinning = ((glSkinnedMesh*)m_glMeshlist[i])->m_textureBuffer;
		m_meshes[i]->m_material->m_bufferMatricesSoftbody = NULL;
	}*/

	TimeProfiler.EndAddTimeProfile( "rs_skinned_model_prestep1" );
}

void CSkinnedModel::PostStepSynchronus ( void )
{
	// update animation
	if ( pMyAnimation )
	{
		pMyAnimation->UpdateTransform( XTransform( transform.position, transform.rotation, transform.scale ) );
		if ( !bReferenceAnimation )
		{
			pMyAnimation->Update(Time::deltaTime); // Get next frame's animation
		}
		else
		{
			if ( eRefMode = ANIM_REF_DIRECT )
			{
				// If not a perfect reference, we just copy over the reference mode transforms
				/*if ( bPerfectReference == false )
				{
					//vector<void*>& refList = pReferencedAnimation->animRefs;
					//vector<void*>& tarList = pMyAnimation->animRefs;

				}*/

				// Directly copy the transforms
				//pReferencedAnimation->GetAnimationSet
				std::vector<void*>& refList = pReferencedAnimation->animRefs;
				std::vector<void*>& tarList = pMyAnimation->animRefs;
				for ( unsigned int i = 0; i < std::min<unsigned int>(refList.size(),tarList.size()); ++i )
				{
					(*(XTransform*)tarList[i]) = (*(XTransform*)refList[i]);
				}
			} // End Direct Referenced animation mode
		}
	}
	//if ( CTransform::updateRenderSide )
	{
		SendOutSkeleton();
	}
}

/*
void CSkinnedModel::PassBoneMatrices ( glMaterial* mat )
{
	Matrix4x4* sys_BoneMatrix = new Matrix4x4 [vSkeleton.size()] ();
	for ( vector<glBone*>::iterator bone = vSkeleton.begin(); bone != vSkeleton.end(); bone++ )
	{
		sys_BoneMatrix[(*bone)->index]	= ((*bone)->GetPoseMatrix());
	}
	mat->setUniformV( "sys_BoneMatrix", vSkeleton.size(), sys_BoneMatrix );
	delete [] sys_BoneMatrix;
}
*/
//#include "CDebugDrawer.h"

void CSkinnedModel::DebugRenderSkeleton ( glBone* bone )
{
	if ( bone->transform.GetParent() != NULL )
	{
		//CDebugDrawer::DrawLine( bone->transform.position + transform.position, bone->transform.GetParent()->position + transform.position );
		//if (( bone->transform.scale.x < 0 )||( bone->transform.scale.y < 0 )||( bone->transform.scale.z < 0 ))
		//	CDebugDrawer::DrawLine( bone->transform.position + transform.position, transform.position );
		//CDebugDrawer::DrawLine( bone->transform.position + transform.position, bone->transform.position + transform.position + bone->transform.Forward()*0.3f );
	}
	Color t_drawColor ( 1,1,0,1 );
	if ( bone->name.find( "Bip001 R" ) != string::npos ) {
		t_drawColor = Color( 0,1,0,1 );
	}
	else if ( bone->name.find( "Bip001 L" ) != string::npos ) {
		t_drawColor = Color( 0,0,1,1 );
	}
	Vector3d t_bonePosition = bone->transform.position + transform.position;
	Debug::Drawer->DrawLine( t_bonePosition, t_bonePosition + bone->transform.Forward()*0.2f, t_drawColor );
	Debug::Drawer->DrawLine( t_bonePosition, t_bonePosition + bone->transform.Up()*0.2f, t_drawColor );
	Debug::Drawer->DrawLine( t_bonePosition, t_bonePosition + bone->transform.Side()*0.2f, t_drawColor );
	// loop through children
	for ( std::vector<Transform*>::iterator it = bone->transform.children.begin(); it != bone->transform.children.end(); it++ )
	{
		Debug::Drawer->DrawLine( t_bonePosition, (*it)->position + transform.position, t_drawColor );
		DebugRenderSkeleton( (glBone*)((*it)->owner) );
	}
}

void CSkinnedModel::UpdateSkeleton ( bool lateUpdate )
{
	UpdateSkeleton( &(rootBone->transform), lateUpdate );
}

void CSkinnedModel::UpdateSkeleton( Transform* cur_transform, bool lateUpdate )
{
	// Update the transform
	if ( lateUpdate ) 
	{
		cur_transform->LateUpdate();
	}
	// Send the pose from the transform to the bone.
	((skeletonBone_t*)(cur_transform->owner))->UpdatePose();
	// loop through children
	for ( std::vector<Transform*>::iterator it = cur_transform->children.begin(); it != cur_transform->children.end(); it++ )
	{
		UpdateSkeleton( (*it), lateUpdate );
	}
}

void CSkinnedModel::SendOutSkeleton ( void )
{
	SendOutSkeleton( rootBone );
}
void CSkinnedModel::SendOutSkeleton ( glBone* bone )
{
	// send out bone
	//bone->transform.LateUpdate();
	bone->SendTransformation();
	//bone->transform.LateUpdate();
	// loop through children
	for ( std::vector<Transform*>::iterator it = bone->transform.children.begin(); it != bone->transform.children.end(); it++ )
	{
		SendOutSkeleton( (glBone*)((*it)->owner) );
	}
}



Transform* CSkinnedModel::GetSkeletonRoot( void )
{
	return &(rootBone->transform);
}
std::vector<skeletonBone_t*>* CSkinnedModel::GetSkeletonList( void )
{
	return &(vSkeleton);
}
CMorpher*	CSkinnedModel::GetMorpher ( void )
{
	return pMorpher;
}


// This sets the bones to copy the target skinned model's bones completely.
//  It does this referencing by creating a reference list that is generated by name matching.
// If the referenced model is deleted, unexpected behavior will occur.
void CSkinnedModel::SetReferencedBoneMode ( CSkinnedModel* pReference )
{
	if ( !pReference ) {
		// Turn off skeleton referencing
		pReferencedSkeleton = NULL;
		bReferenceSkeleton = false;
		bPerfectReference = false;
		mBoneReferenceMap.clear();
		mBoneMismatchList.clear();
	}
	else
	{
		// Draw the skeleton of our reference to show the target's animation state
		//bDrawSkeleton = false;
		//pReference->bDrawSkeleton = true;

		// Enable skelly reference
		pReferencedSkeleton = pReference;
		bReferenceSkeleton = true;
		bPerfectReference = true;
		mBoneReferenceMap.clear();
		mBoneMismatchList.clear();
		// Grab the refed skelly
		std::vector<skeletonBone_t*>* refedSkelly = pReference->GetSkeletonList();

		// Loop through all of our bones.
		for ( uint i = 0; i < vSkeleton.size(); ++i )
		{
			// Loop through all of the target's bones for a match.
			for ( std::vector<skeletonBone_t*>::iterator refBone = refedSkelly->begin(); refBone != refedSkelly->end(); ++refBone )
			{
				// If it matches, add it to the map.
				if ( vSkeleton[i]->name == (*refBone)->name ) {
					mBoneReferenceMap[vSkeleton[i]] = (*refBone);
					break; // Break out and go to the next iteration
				}
			}
		}
		// If our matching does not match up with the size of our skeleton, then some bones were not mapped
		if ( vSkeleton.size() != mBoneReferenceMap.size() )
		{
			bPerfectReference = false; // Turn off perfect reference mode
			// Loop through all the bones. Anything not in the map, we add to the mismatch map.
			for ( uint i = 0; i < vSkeleton.size(); ++i )
			{
				if ( mBoneReferenceMap.find( vSkeleton[i] ) == mBoneReferenceMap.end() ) {
					mBoneMismatchList.push_back( vSkeleton[i] );
				}
			}
			// And that's it!
		}
	}
}

// Updates jigglebones
void CSkinnedModel::UpdateJigglebones ( void )
{
	Matrix4x4 newTransform = transform.GetTransformMatrix();
	Matrix4x4 difference = matrixPreviousStep.inverse() * newTransform;
	Matrix4x4 differenceDifference = matrixPreviousStepDiff.inverse() * difference;

	int t_breastCount = 0;
	for ( auto bone = vSkeleton.begin(); bone != vSkeleton.end(); ++bone )
	{
		skeletonBone_t* t_bone = *bone;
		if ( t_bone->effect == 0 )
		{	// No effect, just copy over the next matrix to use.
			t_bone->tempMatx = t_bone->currentPose;
		}
		else if ( t_bone->effect == 1 )
		{	// Do some sexy ass wobble.
			Matrix4x4 t_tempVelocities;
			Matrix4x4 t_finalEditVelocity;
			// Add world position to the wobble
			// Decrease translation input
			const ftype t_moveDistChange = 0.5f;
			//difference.pData[3] *= t_moveDistChange;
			//difference.pData[7] *= t_moveDistChange;
			//difference.pData[11]*= t_moveDistChange;
			Matrix4x4 nextDifference = difference;
			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
			// Add translation to the matrix
			//t_bone->tempMatx = t_bone->tempMatx * (nextDifference.inverse());
			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;

			// Create the "to resting position" velocities
			const ftype t_moveLimit_TranslationOffset = 10.0f;
			const ftype t_moveLimit = Time::deltaTime * 0.1f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( i == 3 || i == 7 || i == 11 ) {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
					}	
				}
				else {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
					}	
				}
			}
			// Add this to the velocity mix
			t_finalEditVelocity = t_tempVelocities;

			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
			const ftype t_dampenConstant = 4.0f;
			const ftype t_dampenLimit = Time::deltaTime * 4.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
					std::cout << "Hit damping limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}
			// Now apply distance acceleration
			const ftype t_distanceConstant = 1.2f;
			const ftype t_distanceLimit = Time::deltaTime * 10.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
					std::cout << "Hit distance limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}

			// Add the spring velocities to the final velocity
			t_finalEditVelocity += t_bone->veloMatx;

			// Apply the matrix difference
			t_bone->tempMatx += t_finalEditVelocity;

			// Limit the translation
			const ftype t_positionLimit_Translation = 0.4f;
			for ( uint i = 3; i < 12; i += 4 ) {
				ftype currentDifference = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( fabs(currentDifference) > t_positionLimit_Translation ) {
					t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] - Math.sgn(currentDifference)*t_positionLimit_Translation;
				}
			}

			// Edit the pose
			t_bone->currentPose = t_bone->tempMatx;
		}
		// General wobble test
		else if ( t_bone->effect == 11 )
		{
			// Do some sexy ass wobble.
			Matrix4x4 t_tempVelocities;
			Matrix4x4 t_finalEditVelocity;
			// Add world position to the wobble
			// Decrease translation input
			const ftype t_moveDistChange = 0.5f;
			//difference.pData[3] *= t_moveDistChange;
			//difference.pData[7] *= t_moveDistChange;
			//difference.pData[11]*= t_moveDistChange;
			Matrix4x4 nextDifference = difference;
			//nextDifference *= t_bone->tempMatx;
			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
			// Add translation to the matrix
			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;

			// Create the "to resting position" velocities
			const ftype t_moveLimit_TranslationOffset = 10.0f;
			const ftype t_moveLimit = Time::deltaTime * 0.1f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( i == 3 || i == 7 || i == 11 ) {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
					}	
				}
				else {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
					}	
				}
			}
			// Add this to the velocity mix
			t_finalEditVelocity = t_tempVelocities;

			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
			const ftype t_dampenConstant = 4.0f;
			const ftype t_dampenLimit = Time::deltaTime * 4.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
					std::cout << "Hit damping limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}
			// Now apply distance acceleration
			const ftype t_distanceConstant = 1.2f;
			const ftype t_distanceLimit = Time::deltaTime * 10.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
					std::cout << "Hit distance limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}

			// Add the spring velocities to the final velocity
			t_finalEditVelocity += t_bone->veloMatx;

			// Apply the matrix difference
			t_bone->tempMatx += t_finalEditVelocity;

			// Limit the translation
			const ftype t_positionLimit_Translation = 0.4f;
			for ( uint i = 3; i < 12; i += 4 ) {
				ftype currentDifference = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( fabs(currentDifference) > t_positionLimit_Translation ) {
					t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] - Math.sgn(currentDifference)*t_positionLimit_Translation;
				}
			}

			// Edit the pose
			t_bone->currentPose = t_bone->tempMatx;
		}
		// Breast jiggle
		else if ( t_bone->effect == 12 )
		{
			Matrix4x4 boobNewTransform = transform.GetTransformMatrix();//t_bone->transform.GetTransformMatrix();//transform.GetTransformMatrix() * t_bone->transform.GetTransformMatrix();
			boobNewTransform = Matrix4x4();
			boobNewTransform.setTranslation( t_bone->transform.position );
			Matrix4x4 boobDifference = matrixSavePoints[t_breastCount*3+0].inverse() * boobNewTransform;
			Matrix4x4 boobDifferenceDifference = matrixSavePoints[t_breastCount*3+1].inverse() * boobDifference;

			boobDifferenceDifference = boobDifferenceDifference.LerpTo( matrixSavePoints[t_breastCount*3+2], 0.5f );

			matrixSavePoints[t_breastCount*3+0] = boobNewTransform;
			matrixSavePoints[t_breastCount*3+1] = boobDifference;
			matrixSavePoints[t_breastCount*3+2] = boobDifferenceDifference;

			boobDifferenceDifference = boobDifferenceDifference.LerpTo( boobDifference, 0.5f );

			// Create data points for the breast acceleration
			/*Matrix4x4 newTransform = transform.GetTransformMatrix();
			Matrix4x4 difference = matrixPreviousStep.inverse() * newTransform;
			Matrix4x4 differenceDifference = matrixPreviousStepDiff.inverse() * difference;
			*/

			t_breastCount += 1;

			// As close to critical damped as possible
			Matrix4x4 t_tempVelocities;
			Matrix4x4 t_finalEditVelocity;

			Matrix4x4 nextDifference = boobDifferenceDifference;
			// Add world position to the wobble
			// Decrease translation input
			const ftype t_moveDistChange = 0.0f;
			//nextDifference *= t_bone->tempMatx;
			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
			// Add translation to the matrix
			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;

			// Create the "to resting position" velocities
			const ftype t_moveLimit_TranslationOffset = 5.0f;
			const ftype t_moveLimit = Time::deltaTime * 0.05f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( i == 3 || i == 7 || i == 11 ) {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
					}	
				}
				else {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
					}	
				}
			}
			// Add this to the velocity mix
			t_finalEditVelocity = t_tempVelocities;

			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
			const ftype t_dampenConstant = 8.0f;
			const ftype t_dampenLimit = Time::deltaTime * 8.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
					std::cout << "Hit damping limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}
			// Now apply distance acceleration
			const ftype t_distanceConstant = 2.4f; // Fast acceleration
			const ftype t_distanceLimit = Time::deltaTime * 20.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
					std::cout << "Hit distance limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}

			// Add the spring velocities to the final velocity
			t_finalEditVelocity += t_bone->veloMatx;

			// Apply the matrix difference
			t_bone->tempMatx += t_finalEditVelocity;

			// Limit the translation
			const ftype t_positionLimit_Translation = 0.005f; // Low translation limit
			const ftype t_positionLimit_Rotation = 0.04f; // Higher rotation limit
			for ( uint i = 0; i < 16; ++i ) {
				ftype currentDifference = t_bone->tempMatx.pData[i] - t_bone->currentPose.pData[i];
				if ( i == 3 || i == 7 || i == 11 ) {
					if ( fabs(currentDifference) > t_positionLimit_Translation ) {
						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Translation;
					}
				}
				else {
					if ( fabs(currentDifference) > t_positionLimit_Rotation ) {
						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Rotation;
					}
				}
			}

			// Edit the pose
			t_bone->currentPose = t_bone->tempMatx;
		}
		// Tail effect
		else if ( t_bone->effect == 25 )
		{
			// Go to child and set it to tail jiggle
			t_bone->effect = 3;
			int set = 1;
			while ( !t_bone->transform.children.empty() )
			{
				t_bone = (skeletonBone_t*)(t_bone->transform.children[0]->owner);
				t_bone->effect = std::min<int>( 3+set, 10 );
				set += 1;
			}
		}
		else if ( t_bone->effect >= 3 && t_bone->effect <= 10 )
		{
			// Do some sexy ass wobble.
			Matrix4x4 t_tempVelocities;
			Matrix4x4 t_finalEditVelocity;
			// Add world position to the wobble
			// Decrease translation input
			ftype t_moveDistChange = 0.4f;
			if ( t_bone->effect == 3 ) {
				t_moveDistChange = 0.8f;
			}
			//difference.pData[3] *= t_moveDistChange;
			//difference.pData[7] *= t_moveDistChange;
			//difference.pData[11]*= t_moveDistChange;
			Matrix4x4 nextDifference = difference;
			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
			nextDifference = nextDifference;
			// Add translation to the matrix
			//t_bone->tempMatx = t_bone->tempMatx * (difference.inverse()) * ((t_bone->effect==3) ? 1.0f  );
			//if ( t_bone->effect == 3 ) {
				//t_bone->tempMatx = t_bone->tempMatx * (nextDifference.inverse());
			//}
			//else {
			//	t_bone->tempMatx = t_bone->tempMatx * difference;
			//}
			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;

			// Create the "to resting position" velocities
			const ftype t_moveLimit_TranslationOffset = 10.0f;
			const ftype t_moveLimit = Time::deltaTime * 0.1f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( i == 3 || i == 7 || i == 11 ) {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
					}	
				}
				else {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
					}	
				}
			}
			// Add this to the velocity mix
			t_finalEditVelocity = t_tempVelocities;

			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
			const ftype t_dampenConstant = 5.0f;
			const ftype t_dampenLimit = Time::deltaTime * 3.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
					std::cout << "Hit damping limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}
			// Now apply distance acceleration
			ftype t_distanceConstant = 0.9f;
			if ( t_bone->effect == 3 ) {
				t_distanceConstant = 1.9f;
			}
			const ftype t_distanceLimit = Time::deltaTime * 10.0f;
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
					std::cout << "Hit distance limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}

			// Add the spring velocities to the final velocity
			t_finalEditVelocity += t_bone->veloMatx;

			// Apply the matrix difference
			t_bone->tempMatx += t_finalEditVelocity;

			// Limit the translation
			ftype t_positionLimit_Translation = 0.4f;
			t_positionLimit_Translation = (t_bone->effect-3)*0.12f;
			if ( t_bone->effect == 3 ) {
				t_positionLimit_Translation = 0.0f;
			}
			for ( uint i = 3; i < 12; i += 4 ) {
				ftype currentDifference = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( fabs(currentDifference) > t_positionLimit_Translation ) {
					t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] - Math.sgn(currentDifference)*t_positionLimit_Translation;
				}
			}

			// Edit the pose
			t_bone->currentPose = t_bone->tempMatx;
		}
		// General jiggle
		else if ( t_bone->effect == 35 )
		{
			// Do some sexy ass wobble.
			Matrix4x4 t_tempVelocities;
			Matrix4x4 t_finalEditVelocity;
			// Decrease translation input
			const ftype t_moveDistChange = t_bone->effect_v[0];
			Matrix4x4 nextDifference = difference;
			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
			// Add translation to the matrix
			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;

			// Create the "to resting position" velocities
			const ftype t_moveLimit_TranslationOffset = t_bone->effect_v[1];
			const ftype t_moveLimit = Time::deltaTime * t_bone->effect_v[2];
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
				if ( i == 3 || i == 7 || i == 11 ) {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
					}	
				}
				else {
					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
					}	
				}
			}
			// Add this to the velocity mix
			t_finalEditVelocity = t_tempVelocities;

			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
			const ftype t_dampenConstant = t_bone->effect_v[3];
			const ftype t_dampenLimit = Time::deltaTime * t_bone->effect_v[4];
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
					std::cout << "Hit damping limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}
			// Now apply distance acceleration
			const ftype t_distanceConstant = t_bone->effect_v[5]; // Fast acceleration
			const ftype t_distanceLimit = Time::deltaTime * t_bone->effect_v[6];
			for ( uint i = 0; i < 16; ++i ) {
				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
					std::cout << "Hit distance limit" << std::endl;
				}
				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
			}

			// Add the spring velocities to the final velocity
			t_finalEditVelocity += t_bone->veloMatx;

			// Apply the matrix difference
			t_bone->tempMatx += t_finalEditVelocity;

			// Limit the translation
			const ftype t_positionLimit_Translation = t_bone->effect_v[7]; // Low translation limit
			const ftype t_positionLimit_Rotation = t_bone->effect_v[8]; // Higher rotation limit
			for ( uint i = 0; i < 16; ++i ) {
				ftype currentDifference = t_bone->tempMatx.pData[i] - t_bone->currentPose.pData[i];
				if ( i == 3 || i == 7 || i == 11 ) {
					if ( fabs(currentDifference) > t_positionLimit_Translation ) {
						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Translation;
					}
				}
				else {
					if ( fabs(currentDifference) > t_positionLimit_Rotation ) {
						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Rotation;
					}
				}
			}

			// Edit the pose
			t_bone->currentPose = t_bone->tempMatx;
		}
		// End effect type check
	}

	matrixPreviousStep = newTransform;
	matrixPreviousStepDiff = difference;
	//TimeProfiler.EndAddTimeProfile( "rs_skinned_model_begin_w" );
}
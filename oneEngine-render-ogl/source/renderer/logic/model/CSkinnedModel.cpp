
#include "CSkinnedModel.h"

#include "core/time/time.h"
#include "core/settings/CGameSettings.h"
#include "core/math/Math.h"

#include "core-ext/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"

#include "core-ext/profiler/CTimeProfiler.h"

#include "core-ext/animation/CAnimation.h"
#include "physical/animation/CHKAnimation.h"
#include "physical/resource/ResourceManager.h"

#include "renderer/system/glMainSystem.h"

#include "renderer/resource/CModelMaster.h"
#include "renderer/object/mesh/CMesh.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"

#include "renderer/logic/model/morpher/CMorpher.h"

#include "renderer/debug/CDebugDrawer.h"

// Constructor
CSkinnedModel::CSkinnedModel( const string &sFilename )
	: CModel()
{
	debug::Console->PrintMessage( "Loading model \"" + sFilename + "\"\n" );
	// Set model name to input filename
	myModelFilename = sFilename;
	// Standardize the filename
	myModelFilename = IO::FilenameStandardize( myModelFilename );
	// Look for the valid resource to load
	myModelFilename = core::Resources::PathTo( myModelFilename );
#ifndef _ENGINE_DEBUG
	throw core::NotYetImplementedException();
#endif

	// Set basic properties
	bUseFrustumCulling = true;
	bDrawSkeleton = true;

	// Set animation properties
	pReferencedAnimation = NULL;
	bReferenceAnimation = false;
	eRefMode = ANIM_REF_NONE;

	// Initialize morpher objects
	bDoMorphing = false;
	iMorphTarget = -1;
	pMorpher = NULL;

	// Set referenced properties
	referenceToCopySkeletonFrom = NULL;

	// First look for the model in the model master
	auto t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
	// If there's no reference, then load it
	if ( t_meshSet == NULL )
	{
		LoadSkinnedModel( myModelFilename );
		t_meshSet = RenderResources::Active()->GetMesh( myModelFilename.c_str() );
	}
	if ( t_meshSet )
	{
		m_glMeshlist = *t_meshSet;	// Copy the meshes to local list
		debug::Console->PrintMessage( " +Has mesh set\n" );
	}
	else
	{
		throw core::NullReferenceException();
	}

	// Grab a copy of the skeleton
	auto t_skeleton = PhysicalResources::Active()->GetSkeleton( myModelFilename.c_str() );
	if ( t_skeleton != NULL )
	{
		skeleton = *t_skeleton;
		debug::Console->PrintMessage( " +Has skeleton\n" );
	}
	else
	{
		throw core::NullReferenceException();
	}

	// Search for a morph to use
	//CMorpher* possibleMorpher = ModelMaster.GetMorpherReference( myModelFilename );
	auto t_morphSet = RenderResources::Active()->GetMorpher( myModelFilename.c_str() );
	if ( t_morphSet != NULL )
	{
		// If there is a morpher, then load it up
		bDoMorphing = true;
		// Create a new morpher, which will look for the reference and copy it
		pMorpher = new CMorpher( myModelFilename, t_morphSet );
		// Set morph target
		iMorphTarget = pMorpher->GetMorpherSet()->iMorphTarget;
		// Say we got morphs
		debug::Console->PrintMessage( " +Has morph set\n" );
	}
	else
	{
		pMorpher = NULL;
	}

	// Create a stream if needed
	if ( iMorphTarget >= 0 )
	{
		rrSkinnedMesh* source_mesh = (rrSkinnedMesh*)(m_meshes[iMorphTarget]);
		m_glStreamedMesh = source_mesh->Copy();
	}
	else
	{
		m_glStreamedMesh = NULL;
	}

	// Create mesh list
	for ( uint i = 0; i < m_glMeshlist.size(); ++i ) 
	{
		if ( m_glStreamedMesh != NULL && i == iMorphTarget ) // Push back streamed mesh if have morph
			m_meshes.push_back( new CMesh( m_glStreamedMesh, true ) );
		else // Otherwise use the statically created mesh
			m_meshes.push_back( new CMesh( m_glMeshlist[i], true ) );
		m_meshes[i]->m_parent = this;
	}

	// Create skinning buffers
	SkinningBuffersInit();
	// Set skinned count
	skinning_data.bonecount = skeleton.names.size();

	// Get the bounding box
	CalculateBoundingBox();
	// Increase culling distance
	fCheckRenderDist *= 2;

	// Check errors
	GL_ACCESS GL.CheckError();

	debug::Console->PrintMessage( " +Loaded\n" );
}

// Destructor
CSkinnedModel::~CSkinnedModel ( void )
{
	// RenderResources::Active()->ReleaseMeshSet( myModelFilename.c_str() );
	// The model references should decrement automatically from CModel's destructor, thus no need for ReleaseMeshSet
	if ( pMorpher != NULL ) {
		RenderResources::Active()->ReleaseMorphSet( myModelFilename.c_str() );
		pMorpher = NULL;
	}
	PhysicalResources::Active()->ReleaseSkeleton( myModelFilename.c_str() );

	//throw core::DeprecatedCallException();

	//cout << "Entering ~CSkinnedModel.";

	// Free the created mesh streams
	if ( m_glStreamedMesh != NULL )
	{
		delete m_glStreamedMesh;
		m_glStreamedMesh = NULL;
	}
	// Free skinning buffers
	SkinningBuffersFree();
}

// Nonsynchronous step
void CSkinnedModel::PreStep ( void )
{
	// Update the mesh objects
	CModel::PreStep();

	// If not referenced perfectly, convert the pose into a GL compatible form
	if ( referenceToCopySkeletonFrom != NULL )
	{
		Animation::Skeleton::GenerateShaderMatrices( skeleton );
	}

	// Mark skinning as out of date
	skinning_pushed = false;
}

void CSkinnedModel::PreStepSynchronus ( void )
{
	TimeProfiler.BeginTimeProfile( "rs_skinned_model_prestep1" );

	// Rendering debug skeletons is on
	if ( bDrawSkeleton )
	{
		//DebugRenderSkeleton( rootBone );
		DebugRenderSkeleton();
	}

	// Grab current skeleton mode
	//auto renderMode = ((rrSkinnedMesh*)m_glMeshlist[0])->iRenderMode;

	TimeProfiler.EndAddTimeProfile( "rs_skinned_model_prestep1" );
}

void CSkinnedModel::PostStepSynchronus ( void )
{
	//// update animation
	//if ( pMyAnimation )
	//{
	//	pMyAnimation->UpdateTransform( XTransform( transform.position, transform.rotation, transform.scale ) );
	//	if ( !bReferenceAnimation )
	//	{
	//		pMyAnimation->Update(Time::deltaTime); // Get next frame's animation
	//	}
	//	else
	//	{
	//		if ( eRefMode = ANIM_REF_DIRECT )
	//		{
	//			// If not a perfect reference, we just copy over the reference mode transforms
	//			/*if ( bPerfectReference == false )
	//			{
	//				//vector<void*>& refList = pReferencedAnimation->animRefs;
	//				//vector<void*>& tarList = pMyAnimation->animRefs;

	//			}*/

	//			// Directly copy the transforms
	//			//pReferencedAnimation->GetAnimationSet
	//			std::vector<void*>& refList = pReferencedAnimation->animRefs;
	//			std::vector<void*>& tarList = pMyAnimation->animRefs;
	//			for ( unsigned int i = 0; i < std::min<unsigned int>(refList.size(),tarList.size()); ++i )
	//			{
	//				(*(XTransform*)tarList[i]) = (*(XTransform*)refList[i]);
	//			}
	//		} // End Direct Referenced animation mode
	//	}
	//}
	////if ( CTransform::updateRenderSide )
	//{
	//	SendOutSkeleton();
	//}
}

void CSkinnedModel::PostStep ( void )
{
	TimeProfiler.BeginTimeProfile( "rs_skinned_model_poststep1" );

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
		// Find skinning to push to the graphics device
		if ( !skinning_pushed ) 
		{
			if ( referenceToCopySkeletonFrom == NULL )
			{	// No reference, push normally
				SkinningBuffersPush();
			}
			else if ( !referenceToCopySkeletonFrom->skinning_pushed )
			{	// Here before reference? Push reference's skelly.
				referenceToCopySkeletonFrom->SkinningBuffersPush();
				referenceToCopySkeletonFrom->skinning_pushed = true;
			}
			// Otherwise, the skeleton has already been pushed to GPU so no action is needed.
			skinning_pushed = true; // Mark as pushed
		}

		// Before skinning and morphs, prepare the stream
		//if ( renderMode == rrSkinnedMesh::RD_CPU )
		//{
		//	for ( std::vector<rrMesh*>::iterator it = m_glMeshlist.begin(); it != m_glMeshlist.end(); it++ )
		//	{
		//		rrSkinnedMesh* mesh = ((rrSkinnedMesh*)(*it));
		//		mesh->PrepareStream();
		//	}
		//}
		//else if ( renderMode == rrSkinnedMesh::RD_GPU )
		//{	// Only morpher needs to stream vertices
		//	if ( bDoMorphing && m_meshes[iMorphTarget]->GetCanRender() )
		//	{
		//		((rrSkinnedMesh*)m_glMeshlist[iMorphTarget])->PrepareStream();
		//	}
		//}

		// Before skinning, do morphs on the target
		if ( bDoMorphing && m_meshes[iMorphTarget]->GetCanRender() )
		{
			//pMorpher->FindAction("blink")->weight = fabs(sinf(Time::currentTime*2.0f))*0.10f;
			//(*pMorpher)[0].weight = fabs(sinf(Time::currentTime*2.0f))*0.10f;
			pMorpher->PerformMorph( (rrSkinnedMesh*)(m_glMeshlist[iMorphTarget]), m_glStreamedMesh );
			m_glStreamedMesh->UpdateVBO();
		}

		GL_ACCESS;
		// Update the meshes
		for ( std::vector<rrMesh*>::iterator it = m_glMeshlist.begin(); it != m_glMeshlist.end(); it++ )
		{
			rrSkinnedMesh* mesh = ((rrSkinnedMesh*)(*it));
			// Give the mesh the skeleton to pull the matrices from
			if ( referenceToCopySkeletonFrom == NULL )
			{
				mesh->SetSkinningData( skinning_data );
			}
			else
			{
				mesh->SetSkinningData( referenceToCopySkeletonFrom->skinning_data );
			}
			// Apply the skinning
			//if ( CGameSettings::Active()->b_dbg_ro_EnableSkinning ) 
			//{
			//	GL.ThreadGrabLock();
			//	//if ( renderMode == rrSkinnedMesh::RD_CPU )
			//	//{	// CPU Skinning does all the streaming each frame
			//	//	mesh->StartSkinning();
			//	//	mesh->UpdateVBO();
			//	//}
			//	//else if ( renderMode == rrSkinnedMesh::RD_GPU )
			//	//{
			//	if ( bDoMorphing && m_glMeshlist[iMorphTarget] == mesh && m_meshes[iMorphTarget]->GetCanRender() )
			//	{	// Morphing needs full CPU streaming done
			//		mesh->StartSkinning();
			//		mesh->UpdateVBO();
			//	}
			//	else if ( it == m_glMeshlist.begin() )
			//	{	// Otherwise, only the first mesh needs to be streaming, as the rest pull the same buffer information
			//		mesh->StartSkinning();
			//	}
			//	else
			//	{	// TODO: Are these buffer forwarding necessary?
			//		mesh->StartSkinning();
			//	}
			//	// And turn off the streaming
			//	mesh->newStreamReady = false;
			//	//}
			//	GL.ThreadReleaseLock();
			//}
		}
		//TimeProfiler.EndAddTimeProfile( "rs_skinned_model_begin_x" );
	}

	TimeProfiler.EndAddTimeProfile( "rs_skinned_model_poststep1" );
}

/*
void CSkinnedModel::PassBoneMatrices ( RrMaterial* mat )
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

void CSkinnedModel::DebugRenderSkeleton ( void )
{
	for ( size_t i = 0; i < skeleton.parent.size(); ++i )
	{
		//if ( bone->transform.GetParent() != NULL )
		if ( skeleton.parent[i] >= 0 )
		{
			//CDebugDrawer::DrawLine( bone->transform.position + transform.position, bone->transform.GetParent()->position + transform.position );
			//if (( bone->transform.scale.x < 0 )||( bone->transform.scale.y < 0 )||( bone->transform.scale.z < 0 ))
			//	CDebugDrawer::DrawLine( bone->transform.position + transform.position, transform.position );
			//CDebugDrawer::DrawLine( bone->transform.position + transform.position, bone->transform.position + transform.position + bone->transform.Forward()*0.3f );
		}
		Color t_drawColor ( 1,1,0,1 );
		string bone_name = skeleton.names[i];
		if ( bone_name.find( "Bip001 R" ) != string::npos ) {
			t_drawColor = Color( 0,1,0,1 );
		}
		else if ( bone_name.find( "Bip001 L" ) != string::npos ) {
			t_drawColor = Color( 0,0,1,1 );
		}
		//Vector3d t_bonePosition = bone->transform.position + transform.position;
		Vector3d t_bonePosition = skeleton.current_transform[i].world.position + transform.position;
		Rotator& t_rotation = skeleton.current_transform[i].world.rotation;
		debug::Drawer->DrawLine( t_bonePosition, t_bonePosition + t_rotation * Vector3d::forward * 0.2f, t_drawColor );
		debug::Drawer->DrawLine( t_bonePosition, t_bonePosition + t_rotation * Vector3d(0,0,1) * 0.2f, t_drawColor );
		debug::Drawer->DrawLine( t_bonePosition, t_bonePosition + t_rotation * Vector3d(0,1,0) * 0.2f, t_drawColor );
		// loop through children
		/*for ( std::vector<Transform*>::iterator it = bone->transform.children.begin(); it != bone->transform.children.end(); it++ )
		{
			debug::Drawer->DrawLine( t_bonePosition, (*it)->position + transform.position, t_drawColor );
			DebugRenderSkeleton( (glBone*)((*it)->owner) );
		}*/
	}
}

void CSkinnedModel::EnsureUpdateSkeleton ( void )
{
	// Nothing here, yet.
}

CMorpher*	CSkinnedModel::GetMorpher ( void )
{
	return pMorpher;
}
// Get the skeleton 
Animation::Skeleton* CSkinnedModel::GetSkeleton ( void )
{
	return &skeleton;
}

// This sets the bones to copy the target skinned model's bones completely.
//  It does this referencing by creating a reference list that is generated by name matching.
// If the referenced model is deleted, unexpected behavior will occur.
void CSkinnedModel::SetReferencedSkeletonBuffer ( CSkinnedModel* n_reference )
{
	if ( n_reference == NULL )
	{
		referenceToCopySkeletonFrom = NULL;
	}
	else
	{
		referenceToCopySkeletonFrom = n_reference;
		if ( n_reference->skeleton.names.size() != this->skeleton.names.size() )
		{
			throw core::InvalidArgumentException();
		}
	}
}

void CSkinnedModel::SkinningBuffersInit ( void )
{
	SkinningBuffersFree();
	glGenBuffers( 1, &skinning_data.textureBufferData );
	glBindBuffer( GL_UNIFORM_BUFFER, skinning_data.textureBufferData );
	glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*skeleton.names.size(), NULL, GL_STREAM_DRAW );
	glGenBuffers( 1, &skinning_data.textureBufferData_Swap );
	glBindBuffer( GL_UNIFORM_BUFFER, skinning_data.textureBufferData_Swap );
	glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*skeleton.names.size(), NULL, GL_STREAM_DRAW );
	//glGenTextures( 1, &m_textureBuffer );
	//glBindTexture( GL_TEXTURE_BUFFER, m_textureBuffer );

	//glBindTexture( GL_TEXTURE_BUFFER, m_textureBuffer );
	//glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_textureBufferData ); 

	//glGenBuffers( 1, &m_textureSoftBufferData );
	//glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData );
	//glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
	//glGenBuffers( 1, &m_textureSoftBufferData_Swap );
	//glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData_Swap );
	//glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
	//glGenTextures( 1, &m_textureSoftBuffer );
	//glBindTexture( GL_TEXTURE_BUFFER, m_textureSoftBuffer );

	//glBindTexture( GL_TEXTURE_BUFFER, m_textureSoftBuffer );
	//glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_textureSoftBufferData );
}
void CSkinnedModel::SkinningBuffersFree ( void )
{
	//if ( skinning_data.textureBuffer != 0 )
	//{
	//	glDeleteTextures( 1, &skinning_data.textureBuffer );
	//	skinning_data.textureBuffer = 0;
	//}
	if ( skinning_data.textureBufferData != 0 )
	{
		glDeleteBuffers( 1, &skinning_data.textureBufferData );
		skinning_data.textureBufferData = 0;
	}
	if ( skinning_data.textureBufferData_Swap != 0 )
	{
		glDeleteBuffers( 1, &skinning_data.textureBufferData_Swap );
		skinning_data.textureBufferData_Swap = 0;
	}
	//if ( m_textureSoftBuffer != 0 )
	//{
	//	glDeleteTextures( 1, &m_textureSoftBuffer );
	//	m_textureSoftBuffer = 0;
	//}
	//if ( m_textureSoftBufferData != 0 )
	//{
	//	glDeleteBuffers( 1, &m_textureSoftBufferData );
	//	m_textureSoftBufferData = 0;
	//}
	//if ( m_textureSoftBufferData_Swap != 0 )
	//{
	//	glDeleteBuffers( 1, &m_textureSoftBufferData_Swap );
	//	m_textureSoftBufferData_Swap = 0;
	//}
}
void CSkinnedModel::SkinningBuffersPush ( void )
{
	// Push values to buffer
	if ( skinning_data.textureBufferData )
	{
		std::swap( skinning_data.textureBufferData, skinning_data.textureBufferData_Swap );
		glBindBuffer( GL_UNIFORM_BUFFER, skinning_data.textureBufferData );
		/*glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW ); // orphaning seems to work better on certain drivers
		//glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvPoseMatrices );
		//GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_WRITE_ONLY );
		void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4)*pvSkeleton->size(), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
		if ( p ) {
		memcpy( p, pvPoseMatrices, sizeof(Matrix4x4)*pvSkeleton->size() );
		glUnmapBuffer( GL_UNIFORM_BUFFER );
		}*/
		glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*skeleton.current_pose.size(), NULL, GL_STREAM_DRAW ); // orphaning seems to work better on certain drivers
		//glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvPoseMatrices );
		//GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_WRITE_ONLY );
		void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4)*skeleton.current_pose.size(), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
		//void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4)*pvSkeleton->size(), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT );
		if ( p )
		{
			memcpy( p, &skeleton.current_pose[0], sizeof(Matrix4x4)*skeleton.current_pose.size() );
			glUnmapBuffer( GL_UNIFORM_BUFFER );
		}
	}
}

// Updates jigglebones
//void CSkinnedModel::UpdateJigglebones ( void )
//{
//	Matrix4x4 newTransform = transform.GetTransformMatrix();
//	Matrix4x4 difference = matrixPreviousStep.inverse() * newTransform;
//	Matrix4x4 differenceDifference = matrixPreviousStepDiff.inverse() * difference;
//
//	int t_breastCount = 0;
//	for ( auto bone = vSkeleton.begin(); bone != vSkeleton.end(); ++bone )
//	{
//		skeletonBone_t* t_bone = *bone;
//		if ( t_bone->effect == 0 )
//		{	// No effect, just copy over the next matrix to use.
//			t_bone->tempMatx = t_bone->currentPose;
//		}
//		else if ( t_bone->effect == 1 )
//		{	// Do some sexy ass wobble.
//			Matrix4x4 t_tempVelocities;
//			Matrix4x4 t_finalEditVelocity;
//			// Add world position to the wobble
//			// Decrease translation input
//			const ftype t_moveDistChange = 0.5f;
//			//difference.pData[3] *= t_moveDistChange;
//			//difference.pData[7] *= t_moveDistChange;
//			//difference.pData[11]*= t_moveDistChange;
//			Matrix4x4 nextDifference = difference;
//			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
//			// Add translation to the matrix
//			//t_bone->tempMatx = t_bone->tempMatx * (nextDifference.inverse());
//			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;
//
//			// Create the "to resting position" velocities
//			const ftype t_moveLimit_TranslationOffset = 10.0f;
//			const ftype t_moveLimit = Time::deltaTime * 0.1f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( i == 3 || i == 7 || i == 11 ) {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
//					}	
//				}
//				else {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
//					}	
//				}
//			}
//			// Add this to the velocity mix
//			t_finalEditVelocity = t_tempVelocities;
//
//			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
//			const ftype t_dampenConstant = 4.0f;
//			const ftype t_dampenLimit = Time::deltaTime * 4.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
//					std::cout << "Hit damping limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//			// Now apply distance acceleration
//			const ftype t_distanceConstant = 1.2f;
//			const ftype t_distanceLimit = Time::deltaTime * 10.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
//					std::cout << "Hit distance limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//
//			// Add the spring velocities to the final velocity
//			t_finalEditVelocity += t_bone->veloMatx;
//
//			// Apply the matrix difference
//			t_bone->tempMatx += t_finalEditVelocity;
//
//			// Limit the translation
//			const ftype t_positionLimit_Translation = 0.4f;
//			for ( uint i = 3; i < 12; i += 4 ) {
//				ftype currentDifference = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( fabs(currentDifference) > t_positionLimit_Translation ) {
//					t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] - Math.sgn(currentDifference)*t_positionLimit_Translation;
//				}
//			}
//
//			// Edit the pose
//			t_bone->currentPose = t_bone->tempMatx;
//		}
//		// General wobble test
//		else if ( t_bone->effect == 11 )
//		{
//			// Do some sexy ass wobble.
//			Matrix4x4 t_tempVelocities;
//			Matrix4x4 t_finalEditVelocity;
//			// Add world position to the wobble
//			// Decrease translation input
//			const ftype t_moveDistChange = 0.5f;
//			//difference.pData[3] *= t_moveDistChange;
//			//difference.pData[7] *= t_moveDistChange;
//			//difference.pData[11]*= t_moveDistChange;
//			Matrix4x4 nextDifference = difference;
//			//nextDifference *= t_bone->tempMatx;
//			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
//			// Add translation to the matrix
//			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;
//
//			// Create the "to resting position" velocities
//			const ftype t_moveLimit_TranslationOffset = 10.0f;
//			const ftype t_moveLimit = Time::deltaTime * 0.1f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( i == 3 || i == 7 || i == 11 ) {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
//					}	
//				}
//				else {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
//					}	
//				}
//			}
//			// Add this to the velocity mix
//			t_finalEditVelocity = t_tempVelocities;
//
//			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
//			const ftype t_dampenConstant = 4.0f;
//			const ftype t_dampenLimit = Time::deltaTime * 4.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
//					std::cout << "Hit damping limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//			// Now apply distance acceleration
//			const ftype t_distanceConstant = 1.2f;
//			const ftype t_distanceLimit = Time::deltaTime * 10.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
//					std::cout << "Hit distance limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//
//			// Add the spring velocities to the final velocity
//			t_finalEditVelocity += t_bone->veloMatx;
//
//			// Apply the matrix difference
//			t_bone->tempMatx += t_finalEditVelocity;
//
//			// Limit the translation
//			const ftype t_positionLimit_Translation = 0.4f;
//			for ( uint i = 3; i < 12; i += 4 ) {
//				ftype currentDifference = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( fabs(currentDifference) > t_positionLimit_Translation ) {
//					t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] - Math.sgn(currentDifference)*t_positionLimit_Translation;
//				}
//			}
//
//			// Edit the pose
//			t_bone->currentPose = t_bone->tempMatx;
//		}
//		// Breast jiggle
//		else if ( t_bone->effect == 12 )
//		{
//			Matrix4x4 boobNewTransform = transform.GetTransformMatrix();//t_bone->transform.GetTransformMatrix();//transform.GetTransformMatrix() * t_bone->transform.GetTransformMatrix();
//			boobNewTransform = Matrix4x4();
//			boobNewTransform.setTranslation( t_bone->transform.position );
//			Matrix4x4 boobDifference = matrixSavePoints[t_breastCount*3+0].inverse() * boobNewTransform;
//			Matrix4x4 boobDifferenceDifference = matrixSavePoints[t_breastCount*3+1].inverse() * boobDifference;
//
//			boobDifferenceDifference = boobDifferenceDifference.LerpTo( matrixSavePoints[t_breastCount*3+2], 0.5f );
//
//			matrixSavePoints[t_breastCount*3+0] = boobNewTransform;
//			matrixSavePoints[t_breastCount*3+1] = boobDifference;
//			matrixSavePoints[t_breastCount*3+2] = boobDifferenceDifference;
//
//			boobDifferenceDifference = boobDifferenceDifference.LerpTo( boobDifference, 0.5f );
//
//			// Create data points for the breast acceleration
//			/*Matrix4x4 newTransform = transform.GetTransformMatrix();
//			Matrix4x4 difference = matrixPreviousStep.inverse() * newTransform;
//			Matrix4x4 differenceDifference = matrixPreviousStepDiff.inverse() * difference;
//			*/
//
//			t_breastCount += 1;
//
//			// As close to critical damped as possible
//			Matrix4x4 t_tempVelocities;
//			Matrix4x4 t_finalEditVelocity;
//
//			Matrix4x4 nextDifference = boobDifferenceDifference;
//			// Add world position to the wobble
//			// Decrease translation input
//			const ftype t_moveDistChange = 0.0f;
//			//nextDifference *= t_bone->tempMatx;
//			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
//			// Add translation to the matrix
//			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;
//
//			// Create the "to resting position" velocities
//			const ftype t_moveLimit_TranslationOffset = 5.0f;
//			const ftype t_moveLimit = Time::deltaTime * 0.05f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( i == 3 || i == 7 || i == 11 ) {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
//					}	
//				}
//				else {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
//					}	
//				}
//			}
//			// Add this to the velocity mix
//			t_finalEditVelocity = t_tempVelocities;
//
//			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
//			const ftype t_dampenConstant = 8.0f;
//			const ftype t_dampenLimit = Time::deltaTime * 8.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
//					std::cout << "Hit damping limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//			// Now apply distance acceleration
//			const ftype t_distanceConstant = 2.4f; // Fast acceleration
//			const ftype t_distanceLimit = Time::deltaTime * 20.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
//					std::cout << "Hit distance limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//
//			// Add the spring velocities to the final velocity
//			t_finalEditVelocity += t_bone->veloMatx;
//
//			// Apply the matrix difference
//			t_bone->tempMatx += t_finalEditVelocity;
//
//			// Limit the translation
//			const ftype t_positionLimit_Translation = 0.005f; // Low translation limit
//			const ftype t_positionLimit_Rotation = 0.04f; // Higher rotation limit
//			for ( uint i = 0; i < 16; ++i ) {
//				ftype currentDifference = t_bone->tempMatx.pData[i] - t_bone->currentPose.pData[i];
//				if ( i == 3 || i == 7 || i == 11 ) {
//					if ( fabs(currentDifference) > t_positionLimit_Translation ) {
//						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Translation;
//					}
//				}
//				else {
//					if ( fabs(currentDifference) > t_positionLimit_Rotation ) {
//						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Rotation;
//					}
//				}
//			}
//
//			// Edit the pose
//			t_bone->currentPose = t_bone->tempMatx;
//		}
//		// Tail effect
//		else if ( t_bone->effect == 25 )
//		{
//			// Go to child and set it to tail jiggle
//			t_bone->effect = 3;
//			int set = 1;
//			while ( !t_bone->transform.children.empty() )
//			{
//				t_bone = (skeletonBone_t*)(t_bone->transform.children[0]->owner);
//				t_bone->effect = std::min<int>( 3+set, 10 );
//				set += 1;
//			}
//		}
//		else if ( t_bone->effect >= 3 && t_bone->effect <= 10 )
//		{
//			// Do some sexy ass wobble.
//			Matrix4x4 t_tempVelocities;
//			Matrix4x4 t_finalEditVelocity;
//			// Add world position to the wobble
//			// Decrease translation input
//			ftype t_moveDistChange = 0.4f;
//			if ( t_bone->effect == 3 ) {
//				t_moveDistChange = 0.8f;
//			}
//			//difference.pData[3] *= t_moveDistChange;
//			//difference.pData[7] *= t_moveDistChange;
//			//difference.pData[11]*= t_moveDistChange;
//			Matrix4x4 nextDifference = difference;
//			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
//			nextDifference = nextDifference;
//			// Add translation to the matrix
//			//t_bone->tempMatx = t_bone->tempMatx * (difference.inverse()) * ((t_bone->effect==3) ? 1.0f  );
//			//if ( t_bone->effect == 3 ) {
//				//t_bone->tempMatx = t_bone->tempMatx * (nextDifference.inverse());
//			//}
//			//else {
//			//	t_bone->tempMatx = t_bone->tempMatx * difference;
//			//}
//			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;
//
//			// Create the "to resting position" velocities
//			const ftype t_moveLimit_TranslationOffset = 10.0f;
//			const ftype t_moveLimit = Time::deltaTime * 0.1f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( i == 3 || i == 7 || i == 11 ) {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
//					}	
//				}
//				else {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
//					}	
//				}
//			}
//			// Add this to the velocity mix
//			t_finalEditVelocity = t_tempVelocities;
//
//			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
//			const ftype t_dampenConstant = 5.0f;
//			const ftype t_dampenLimit = Time::deltaTime * 3.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
//					std::cout << "Hit damping limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//			// Now apply distance acceleration
//			ftype t_distanceConstant = 0.9f;
//			if ( t_bone->effect == 3 ) {
//				t_distanceConstant = 1.9f;
//			}
//			const ftype t_distanceLimit = Time::deltaTime * 10.0f;
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
//					std::cout << "Hit distance limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//
//			// Add the spring velocities to the final velocity
//			t_finalEditVelocity += t_bone->veloMatx;
//
//			// Apply the matrix difference
//			t_bone->tempMatx += t_finalEditVelocity;
//
//			// Limit the translation
//			ftype t_positionLimit_Translation = 0.4f;
//			t_positionLimit_Translation = (t_bone->effect-3)*0.12f;
//			if ( t_bone->effect == 3 ) {
//				t_positionLimit_Translation = 0.0f;
//			}
//			for ( uint i = 3; i < 12; i += 4 ) {
//				ftype currentDifference = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( fabs(currentDifference) > t_positionLimit_Translation ) {
//					t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] - Math.sgn(currentDifference)*t_positionLimit_Translation;
//				}
//			}
//
//			// Edit the pose
//			t_bone->currentPose = t_bone->tempMatx;
//		}
//		// General jiggle
//		else if ( t_bone->effect == 35 )
//		{
//			// Do some sexy ass wobble.
//			Matrix4x4 t_tempVelocities;
//			Matrix4x4 t_finalEditVelocity;
//			// Decrease translation input
//			const ftype t_moveDistChange = t_bone->effect_v[0];
//			Matrix4x4 nextDifference = difference;
//			nextDifference = nextDifference.LerpTo( Matrix4x4(), t_moveDistChange );
//			// Add translation to the matrix
//			t_bone->tempMatx = (nextDifference.inverse()) * t_bone->tempMatx;
//
//			// Create the "to resting position" velocities
//			const ftype t_moveLimit_TranslationOffset = t_bone->effect_v[1];
//			const ftype t_moveLimit = Time::deltaTime * t_bone->effect_v[2];
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i];
//				if ( i == 3 || i == 7 || i == 11 ) {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit*t_moveLimit_TranslationOffset ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit*t_moveLimit_TranslationOffset;
//					}	
//				}
//				else {
//					if ( fabs(t_tempVelocities.pData[i]) > t_moveLimit ) {
//						t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_moveLimit;
//					}	
//				}
//			}
//			// Add this to the velocity mix
//			t_finalEditVelocity = t_tempVelocities;
//
//			// Modify the real velocity matrix by working on acceleration. First, damp the velocity to zero.
//			const ftype t_dampenConstant = t_bone->effect_v[3];
//			const ftype t_dampenLimit = Time::deltaTime * t_bone->effect_v[4];
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = -t_bone->veloMatx.pData[i] * Time::deltaTime * t_dampenConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_dampenLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_dampenLimit;
//					std::cout << "Hit damping limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//			// Now apply distance acceleration
//			const ftype t_distanceConstant = t_bone->effect_v[5]; // Fast acceleration
//			const ftype t_distanceLimit = Time::deltaTime * t_bone->effect_v[6];
//			for ( uint i = 0; i < 16; ++i ) {
//				t_tempVelocities.pData[i] = (t_bone->currentPose.pData[i] - t_bone->tempMatx.pData[i]) * Time::deltaTime * t_distanceConstant;
//				if ( fabs(t_tempVelocities.pData[i]) > t_distanceLimit ) {
//					t_tempVelocities.pData[i] = Math.sgn( t_tempVelocities.pData[i] ) * t_distanceLimit;
//					std::cout << "Hit distance limit" << std::endl;
//				}
//				t_bone->veloMatx.pData[i] += t_tempVelocities.pData[i];
//			}
//
//			// Add the spring velocities to the final velocity
//			t_finalEditVelocity += t_bone->veloMatx;
//
//			// Apply the matrix difference
//			t_bone->tempMatx += t_finalEditVelocity;
//
//			// Limit the translation
//			const ftype t_positionLimit_Translation = t_bone->effect_v[7]; // Low translation limit
//			const ftype t_positionLimit_Rotation = t_bone->effect_v[8]; // Higher rotation limit
//			for ( uint i = 0; i < 16; ++i ) {
//				ftype currentDifference = t_bone->tempMatx.pData[i] - t_bone->currentPose.pData[i];
//				if ( i == 3 || i == 7 || i == 11 ) {
//					if ( fabs(currentDifference) > t_positionLimit_Translation ) {
//						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Translation;
//					}
//				}
//				else {
//					if ( fabs(currentDifference) > t_positionLimit_Rotation ) {
//						t_bone->tempMatx.pData[i] = t_bone->currentPose.pData[i] + Math.sgn(currentDifference)*t_positionLimit_Rotation;
//					}
//				}
//			}
//
//			// Edit the pose
//			t_bone->currentPose = t_bone->tempMatx;
//		}
//		// End effect type check
//	}
//
//	matrixPreviousStep = newTransform;
//	matrixPreviousStepDiff = difference;
//	//TimeProfiler.EndAddTimeProfile( "rs_skinned_model_begin_w" );
//}
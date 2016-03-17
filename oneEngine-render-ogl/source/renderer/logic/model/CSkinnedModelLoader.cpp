
#include "CSkinnedModel.h"
#include "CModelLoader.h"

#include "physical/skeleton/skeletonBone.h"
#include "physical/physics/shapes/physMesh.h"

#include "core-ext/animation/set/CAnimationSet.h"
#include "core-ext/animation/curve/CArrayAnimationCurve.h"
#include "physical/animation/set/CHKAnimationSet.h"

#include "core-ext/animation/CAnimation.h"
#include "core-ext/animation/CAnimAction.h"
#include "physical/animation/CHKAnimation.h"

#include "core/utils/StringUtils.h"
#include "core/system/io/CSegmentedFile.h"
#include "core/debug/CDebugConsole.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/FileUtils.h"

#include "renderer/resource/CModelMaster.h"
#include "renderer/object/mesh/system/glSkinnedMesh.h"
#include "renderer/material/glMaterial.h"
#include "renderer/logic/model/morpher/CMorpher.h"

#include <sstream>

using namespace std;

void CSkinnedModel::LoadSkinnedModel ( const string& sFilename )
{
	bool haveConverter = IO::FileExists("_devtools/FBXtoPAD.exe");

	// First check for needed file conversion!
	string sTargetFilename = sFilename;//Core::Resources::PathTo( sFilename );
	string sFileExtention = StringUtils::ToLower( StringUtils::GetFileExtension( sTargetFilename ) );
	//cout << sFileExtention << endl;

	if ( sFileExtention == "fbx" )
	{
		// Get the vanilla filename
		sTargetFilename = sTargetFilename.substr( 0, sTargetFilename.length()-(sFileExtention.length()+1) );
		
		// Run the converter program
		if ( haveConverter )
		{
#		ifdef _WIN32
			string sArgument;
			sArgument = sTargetFilename + ".FBX " + sTargetFilename + ".PAD";

			LPTSTR lpCommandLine = new CHAR [1024];
			strcpy( lpCommandLine, (string("_devtools/FBXtoPAD.exe ") + sArgument).c_str() );
			STARTUPINFO startupInfo;
			ZeroMemory( &startupInfo, sizeof( STARTUPINFO ) );
			startupInfo.cb = sizeof( STARTUPINFO );
			PROCESS_INFORMATION procInfo;
			ZeroMemory( &procInfo, sizeof(PROCESS_INFORMATION) );

			int result = CreateProcess( NULL, lpCommandLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &procInfo );

			if ( !result )
			{
				cout << "Cannot convert model without FBX conversion devtool! (loading PAD anyways)" << endl;
			}
			else
			{
				// Wait for it to finish conversion
				WaitForSingleObject( procInfo.hProcess, INFINITE );

				// Close process and thread handles. 
				CloseHandle( procInfo.hProcess );
				CloseHandle( procInfo.hThread );
			}
#		elif
			cout << "Model conversion on other platforms not yet supported." << endl;
#		endif
		}

		// Set the fileobject
		sTargetFilename = sTargetFilename + ".PAD";

		// Look for the valid resource to load
		sTargetFilename = Core::Resources::PathTo( sTargetFilename );
	}
	else if ( sFileExtention == "pad" )
	{
		// Nothing.
	}
	// End Conversion
	//--------------------

	// Read in the file
	CSegmentedFile* inputFile = new CSegmentedFile ( sTargetFilename );
	if ( inputFile->ReadData() )
	{
		// Temporary buffer stuff
		stringstream sin;
		string sOutput;

		/*
		Segment Name: $_INFO
		Keys:
			(see documentation)
		*/
		//===============================
		//==Read in the header block==
		inputFile->GetSectionData( string("$_INFO"), sOutput );
		sin.str( sOutput );
		sin.seekg( 0, ios::beg );

		ModelHeader header;
		sin.read( (char*)&header, sizeof( ModelHeader ) );

		uint32_t iMeshNum		= header.meshnum;
		uint32_t iUniqueMeshNum	= header.uniquemeshnum;
		uint32_t bAnimated		= header.animated;
		uint32_t iFrameNum		= header.framenumber;
		uint32_t iMatNum		= header.materialnum;
		uint32_t iBoneNum		= header.bonenum;
		uint32_t iSplineNum		= header.splinenum;
		uint32_t iVersion		= header.m_version;
		uint32_t iCameraNum		= header.cameranum;
		uint32_t iMiscNum		= header.miscnum;
		uint32_t iMorphNum		= header.morphnum;
		uint32_t iMorphMesh		= header.morphmesh;

		//===End Reading Header Block===
		//===============================

		//===============================
		//==Read in the material block==
		vector<string> vMaterialFiles;
		if ( iMatNum > 0 )
		{
			// Read in the material info
			for ( uint32_t i = 0; i < iMatNum; i += 1 )
			{
				// Read in the section data
				sin.seekg( 0, ios::beg );
				sin.str( "" );
				sin << "$_MATERIAL" << i;
				if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
				{
					cout << "Bad section find ($_MATERIAL):: " << sin.str() << endl;
					continue;
				}

				// Move the data to a stream
				sin.seekg( 0, ios::beg );
				sin.str( sOutput );
				sin.seekg( 0, ios::beg );

				{
					// Read in the material info
					string		materialName;
					char		isReferenced;
					string		materialFileName;

					/*[string] string giving material name, null terminated
					[char]{bool} is referenced?
					If it's referenced
					[string] string giving the material file, null terminated
					*/
					
					//[string] string giving mesh name
					char tempChar;
					do
					{
						sin.read( &tempChar, 1 );
						materialName += tempChar;
					}
					while ( tempChar != 0 );

					//[char]{bool} animated per-vertex
					sin.read( &isReferenced, sizeof( char ) );

					if ( isReferenced )
					{
						//[string] string giving the material file, null terminated
						do
						{
							sin.read( &tempChar, 1 );
							materialFileName += tempChar;
						}
						while ( tempChar != 0 );

						// Add to list
						vMaterialFiles.push_back( materialFileName );
					}
				}
			}
		}
		//===============================


		//===============================
		//==Prepare animations==
		CAnimationSet*	newAnimSet;
#ifdef PHYSICS_USING_BOX2D
		newAnimSet = new CAnimationSet();
#elif defined(PHYSICS_USING_HAVOK)
		newAnimSet = new CHKAnimationSet( iBoneNum, iFrameNum );
#else
#	error Invalid physics engine selected
#endif
		//===============================


		//===============================
		//==Prepare animations==
		CAnimation* newAnimation = new CAnimation( sFilename, newAnimSet );
		{
			// First, search for an action block
			uint32_t iEventNum = 0;
			if ( inputFile->GetSectionData( string("$_ACTIONS"), sOutput ) )
			{
				// Move the data to a stream
				sin.seekg( 0, ios::beg );
				sin.str( sOutput );
				sin.seekg( 0, ios::beg );

				//============================================================================
				//[uint32] action num
				//============================================================================
				uint32_t iActionNum;
				sin.read( (char*)(&iActionNum), sizeof( uint32_t ) );

				//============================================================================
				//[uint32,uint32,string,char][n] start frame; end frame; null-terminated string giving action name, where n is an integer from 0 to actionnum; 8-bit flag of options
				//============================================================================
				for ( uint32_t iCurAction = 0; iCurAction < iActionNum; ++iCurAction )
				{
					//[uint32] start frame
					uint32_t iStartFrame;
					sin.read( (char*)(&iStartFrame), sizeof( uint32_t ) );

					//[uint32] end frame
					uint32_t iEndFrame;
					sin.read( (char*)(&iEndFrame), sizeof( uint32_t ) );

					//[string] string giving action name, null terminated
					string sAnimName;
					char tempChar;
					do
					{
						sin.read( &tempChar, 1 );
						sAnimName += tempChar;
					}
					while ( tempChar != 0 );
					sAnimName = sAnimName.substr( 0, sAnimName.length()-1 );

					//[char] flag
					char iOptionsFlag;
					sin.read( (char*)(&iOptionsFlag), sizeof( char ) );

					// Create new action
					CAnimAction newAction ( StringUtils::ToLower( sAnimName ) );
					newAction.SetRange( (ftype)(iStartFrame), (ftype)(iEndFrame) );
					newAction.framesPerSecond = 30.0f;
					newAction.index = iCurAction;
					newAction.loop = (iOptionsFlag & 0x01) > 0;
					newAction.extrapolateMotion[0] = (iOptionsFlag & 0x02) > 0;
					newAction.extrapolateMotion[1] = (iOptionsFlag & 0x04) > 0;
					newAction.extrapolateMotion[2] = (iOptionsFlag & 0x08) > 0;
					newAnimation->AddAction( newAction );

					//cout << "Anim: " << sAnimName << " S: " << iStartFrame << " E: " << iEndFrame << " L: " << iLooped << endl;
				}

				//============================================================================
				//[uint32] event num
				//============================================================================
				sin.read( (char*)(&iEventNum), sizeof( uint32_t ) );

				// Skip if no events found
				if ( (iEventNum == 0) || sin.eof() ) {
					// Don't need to do anything yet
				}
				else {
					//============================================================================
					//[uint32,uint32,string,uint32][n] target action index; frame; null-terminated giving event name; data set
					//============================================================================
					for ( uint32_t iCurEvent = 0; iCurEvent < iEventNum; ++iCurEvent )
					{
						//[uint32] target action
						uint32_t iTargetAction;
						sin.read( (char*)(&iTargetAction), sizeof( uint32_t ) );

						//[uint32] target frame
						uint32_t iFrame;
						sin.read( (char*)(&iFrame), sizeof( uint32_t ) );

						//[string] string giving event name, null terminated
						string sEventName;
						char tempChar;
						do
						{
							sin.read( &tempChar, 1 );
							sEventName += tempChar;
						}
						while ( tempChar != 0 );
						sEventName = sEventName.substr( 0, sEventName.length()-1 );

						//[uint32] data
						uint32_t iData;
						sin.read( (char*)(&iData), sizeof( uint32_t ) );

						// Add event
						Animation::ActionEvent newEvent;
						newEvent.frame = (ftype)iFrame;
						newEvent.data = iData;
						if ( sEventName == "attack" ) {
							newEvent.type = Animation::Event_Attack;
						}
						else if ( sEventName == "footstep" ) {
							newEvent.type = Animation::Event_Footstep;
						}
						else if ( sEventName == "lfootstep" ) {
							newEvent.type = Animation::Event_Footstep_Left;
						}
						else if ( sEventName == "rfootstep" ) {
							newEvent.type = Animation::Event_Footstep_Right;
						}
						else if ( sEventName == "clang_check" ) {
							newEvent.type = Animation::Event_ClangCheck;
						}
						else {
							newEvent.type = Animation::Event_INVALID;
						}
						//newEvent.name = sEventName;
						if ( newEvent.frame >= (*newAnimation)[iTargetAction].GetLength() ) {
							newEvent.frame -= (*newAnimation)[iTargetAction].GetLength();
						}
						(*newAnimation)[iTargetAction].AddEvent( newEvent );

						// Print info
						//cout << " Event: " << sEventName << " F: " << iFrame << " ta: " << iTargetAction << endl;
					}
				}
			}
			
			if ( iEventNum == 0 )
			{
				/*Debug::Console->PrintWarning( "No animation action chunk found. Adding default ref.\n" );
				// Since there's no action block, we just create one for the entire animation
				CAnimAction newAction ( "ref" );
				newAction.SetRange( 0, (ftype)(iFrameNum) );
				newAction.framesPerSecond = 30.0f;
				newAction.index = 0;
				newAction.loop = false;
				newAction.extrapolateMotion[0] = false;
				newAction.extrapolateMotion[1] = false;
				newAction.extrapolateMotion[2] = false;
				newAnimation->AddAction( newAction );*/
			}

			// Play the first animation, which is the default
			//(*newAnimation)[0].Play();
			//(*newAnimation)["ragdoll"].Play();
			//newAnimation->Play( "ragdoll" );
		}
		// And set the pointer map for the animation
		/*vector<void*> voidMap;
		for ( unsigned int i = 0; i < vSkeleton.size(); i++ )
		{
			voidMap.push_back( (void*)(&(vSkeleton[i]->currentPose)) );
		}
		newAnimation->AssignReferenceList( voidMap );*/
		// Update animation set now
		if ( CAnimation::useHavok )
		{
			// Loop through the actions of the system. Get their index, create animation from that.
			auto actionMap = newAnimation->GetActionMap();
			((CHKAnimationSet*)newAnimSet)->SetActionCount( actionMap.size() );
			for ( auto action = actionMap.begin(); action != actionMap.end(); ++action )
			{
				//((CHKAnimationSet*)newAnimSet)->SewtActions();
				((CHKAnimationSet*)newAnimSet)->SetAction( action->second.index, (int)action->second.GetStart(), (int)action->second.GetEnd() );
			}
		}
		//===============================


		//===============================
		//==Read in the Bone Block==
		Vector3d bone_rootScaling;
		for ( uint32_t i = 0; i < iBoneNum; i += 1 )
		{
			// Create Bone Data
			//CModelData* newModelData = new CModelData();
			//glBone* newBone = new glBone( NULL );

			// Read in the section data
			sin.seekg( 0, ios::beg );
			sin.str( "" );
			sin << "$_BONE" << i;
			if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
			{
				cout << "Bad section find ($_BONE):: " << sin.str() << endl;
				continue;
			}

			// Move the data to a stream
			sin.seekg( 0, ios::beg );
			sin.str( sOutput );
			sin.seekg( 0, ios::beg );
			
			{	// -Begin Bone Chunk-
				string		boneName;
				string		boneParent;
				//float		transformMatx [16];
				/*
		[string] string giving bone name, null terminated
		[string] string giving name of bone parent, null terminated. This is “_is_root_” if there is no parent.
		[float][16] transformation matrix of the bone in world space. Supposed to be column-major.
		[float][16][f] transformation matrix of the bone for animation in local space. If animated via skinning, f set for numframes
				*/
				char tempChar;

				//[string] string giving bone name, null terminated
				do
				{
					sin.read( &tempChar, 1 );
					if ( tempChar != 0 ) boneName += tempChar;
				}
				while ( tempChar != 0 );

				//[string] string giving name of bone parent, null terminated. This is “_is_root_” if there is no parent.
				do
				{
					sin.read( &tempChar, 1 );
					if ( tempChar != 0 ) boneParent += tempChar;
				}
				while ( tempChar != 0 );

				//[float][16] transformation matrix of the bone in world space. Supposed to be column-major.
				float dataArrT [10];
				sin.read( (char*)(dataArrT), sizeof( float )*10 );

				// ===== Create a new bone with the given info =====
				// Create a new bone
				glBone* newBone = new glBone( NULL );
				newBone->name = boneName;
				newBone->transform.name = boneName;
				newBone->index = (unsigned char)( i );
				newBone->lod = 0;

				// Check for valid information
				if ( newBone->name.size() == 0 ) {
					throw Core::CorruptedDataException();
				}

				// Set the bone's transform (bind pose)
				if ( boneParent == "_is_root_" ) {
					bone_rootScaling = Vector3d(dataArrT[7],dataArrT[8],dataArrT[9]);
				}

				newBone->transform.SetTransform(		// This is in world space
					Vector3d(dataArrT[0],dataArrT[1],dataArrT[2]),
					Quaternion(dataArrT[3],dataArrT[4],dataArrT[5],dataArrT[6]),
					Vector3d(dataArrT[7],dataArrT[8],dataArrT[9]) );
				newBone->transform.LateUpdate();
				newBone->xBindPose = XTransform(
					newBone->transform.localPosition,
					Quaternion(newBone->transform.localRotation),
					newBone->transform.localScale );
				newBone->xBindPoseModel = XTransform(
					Vector3d(dataArrT[0],dataArrT[1],dataArrT[2]),
					Quaternion(dataArrT[3],dataArrT[4],dataArrT[5],dataArrT[6]),
					Vector3d(dataArrT[7],dataArrT[8],dataArrT[9]) );

				// Find its parent
				if ( boneParent != "_is_root_" )
				{
					// Should already be in the list
					for ( vector<glBone*>::iterator it = vSkeleton.begin(); it != vSkeleton.end(); it++ )
					{
						// If we found it
						if ( (*it)->name == boneParent )
						{
							// Set it
							newBone->transform.SetParent( &((*it)->transform) );
							newBone->transform.LateUpdate();

							// Break it
							it = vSkeleton.end()-1;
						}
					}
				}
				// And that should be it for this bone
				// Add the bone to the list
				vSkeleton.push_back( newBone );

				// ===== Create a new track for the current bone =====
				if ( !CAnimation::useHavok )
				{
					CArrayAnimationCurve<XTransform>* newCurve = new CArrayAnimationCurve<XTransform>();

					//[float][16][f] transformation matrix of the bone for animation in local space. If animated via skinning, f set for numframes
					for ( uint32_t frame = 0; frame < iFrameNum; frame++ )
					{
						float dataArr [10];
						sin.read( (char*)(dataArr), sizeof( float )*10 );

						XTransform fMatx;
						fMatx.position = Vector3d( dataArr[0], dataArr[1], dataArr[2] );
						fMatx.rotation = Quaternion( -dataArr[3], -dataArr[4], -dataArr[5], dataArr[6] );
						fMatx.scale	   = Vector3d( dataArr[7], dataArr[8], dataArr[9] );

						newCurve->AddValue( fMatx );
					}
					
					// Add the new track to the animation set
					newAnimSet->Add( newCurve );
				}
				else
				{
					// Read in all the animation data
					/*XTransform *dataArr = new XTransform [iFrameNum];
					sin.read( (char*)(dataArr), sizeof(float)*10*iFrameNum );
					for ( uint32_t frame = 0; frame < iFrameNum; ++frame )
					{
						// Flip the animation horizontally
						dataArr[frame].rotation.x = -dataArr[frame].rotation.x;
						dataArr[frame].rotation.y = -dataArr[frame].rotation.y;
						dataArr[frame].rotation.z = -dataArr[frame].rotation.z;
						// Add the value to the animation set
						((CHKAnimationSet*)newAnimSet)->AddValue( dataArr[frame] );
					}
					delete [] dataArr;*/
					for ( uint32_t frame = 0; frame < iFrameNum; ++frame )
					{
						XTransform data;
						sin.read( (char*)(&data), sizeof(float)*10 );
						// Flip the animation horizontally
						data.rotation.x = -data.rotation.x;
						data.rotation.y = -data.rotation.y;
						data.rotation.z = -data.rotation.z;
						// Add the value to the animation set
						((CHKAnimationSet*)newAnimSet)->AddValue( data );
					}
				}
			} // -End Bone Chunk-
		}
		if ( CAnimation::useHavok )
		{
			((CHKAnimationSet*)newAnimSet)->OrderizeValues();	// Reorders values in memory to work with Havok
			((CHKAnimationSet*)newAnimSet)->Initialize();		// Sends values to Havok
		}

		// Grab zeroth bone
		if ( vSkeleton.size() > 0 )
		{
			rootBone = vSkeleton[0];
			SetSkeletonBindPose( rootBone );
		}
		else {
			rootBone = NULL;
		}
		//===End Reading Bone Block===
		//===============================


		//===============================
		//==Read in the Mesh Block==
		uint32_t* pCollideIndices = new uint32_t [iMeshNum];
		for ( uint32_t i = 0; i < iMeshNum; i += 1 )
		{
			// Create Mesh
			glMesh* newMesh = new glSkinnedMesh ();
			((glSkinnedMesh*)newMesh)->SetSkeleton( &vSkeleton );

			// Create Model Data
			CModelData* newModelData = new CModelData();
			uint32_t	modelMaterial;
			string		meshName;
			string		parentName;

			// Read in the section data
			//sin.seekg( 0, ios::beg );
			//sin.str( "" );
			//sin << "$_MESH" << i;
			//if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
			char strt [16];
			sprintf( strt, "$_MESH%d", i );
			if ( !inputFile->GetSectionData( string(strt), sOutput ) )
			{
				cout << "Bad section find ($_MESH):: " << sin.str() << endl;
				continue;
			}

			// Move the data to a stream
			sin.str("");
			sin.clear();
			sin.seekg( 0, ios::beg );
			sin.str( sOutput );
			sin.seekg( 0, ios::beg );
			
			{
				// Read in data
				char		animatedPerVertex;
				uint32_t	materialIndex;
				float		transformMatx [16];
				uint32_t	vertexNum;
				uint32_t	triangleNum;
				uint32_t	collisionMesh;
				/*[string] string giving mesh name
				[char]{bool} animated per-vertex
				[uint32] material index
				[float][16][f] transformation matrix of the mesh. Supposed to be column-major. If animated per-vertex, f set for numframes
				[uint32] vertexnum
				[CModelVertex][n][f] vertex data where n is an integer from 0 to vertexnum
				[uint32] trianglenum
				[CModelTriangle][n][f] triangle data where n is an integer from 0 to trianglenum
				[uint32] index of convex collision mesh. signed(-1) if no separate collision mesh*/

				//[string] string giving mesh name
				char tempChar;
				do
				{
					sin.read( &tempChar, 1 );
					meshName += tempChar;
				}
				while ( tempChar != 0 );

				//[char]{bool} animated per-vertex
				sin.read( &animatedPerVertex, sizeof( char ) );

				//[uint32] material index
				sin.read( (char*)(&materialIndex), sizeof( uint32_t ) );
				modelMaterial = materialIndex;
				
				//[float][16][f] transformation matrix of the mesh. Supposed to be column-major. If animated per-vertex, f set for numframes
				sin.read( (char*)(transformMatx), sizeof( float )*16 );

				//[uint32] vertexnum
				sin.read( (char*)(&vertexNum), sizeof( uint32_t ) );

				//[CModelVertex][n][f] vertex data where n is an integer from 0 to vertexnum
				newModelData->vertexNum = vertexNum;
				newModelData->vertices = new CModelVertex [vertexNum];
				sin.read( (char*)(newModelData->vertices), sizeof( CModelVertex ) * vertexNum );

				// Convert the vertex coordinates
				for ( uint32_t vert = 0; vert < vertexNum; vert += 1 )
				{
					Vector3d vPos = Vector3d( newModelData->vertices[vert].x, newModelData->vertices[vert].y, newModelData->vertices[vert].z );
					Matrix4x4 mTransform = Matrix4x4( transformMatx );
					mTransform = mTransform.transpose(); // TODO OPTIMIZE
					vPos = mTransform * vPos;
					newModelData->vertices[vert].x = vPos.x;
					newModelData->vertices[vert].y = vPos.y;
					newModelData->vertices[vert].z = vPos.z;
				}
				
				//[uint32] trianglenum
				sin.read( (char*)(&triangleNum), sizeof( uint32_t ) );

				//[CModelTriangle][n][f] triangle data where n is an integer from 0 to trianglenum
				newModelData->triangleNum = triangleNum;
				newModelData->triangles = new CModelTriangle [triangleNum];
				sin.read( (char*)(newModelData->triangles), sizeof( CModelTriangle ) * triangleNum );

				//[uint32] index of convex collision mesh. signed(-1) if no separate collision mesh
				sin.read( (char*)(&collisionMesh), sizeof( uint32_t ) );
				pCollideIndices[i] = collisionMesh;

				//[string] string giving parent node name. Meshes and bones are valid. This is “_is_root_” if there is no parent.
				do
				{
					sin.read( &tempChar, 1 );
					parentName += tempChar;
				}
				while ( tempChar != 0 );
				// todo: do something with this parent!
				// mesh should have a bone* maybe?
				// bones all should be loaded already
				// so we look for the bone
				//for ( unsigned int i = 0; i < vSkeleton.size(); ++i ){if ( parentName == vSkeleton[i]->name ){}};
				// incorrect, skeletons are not unique to a mesh.
				// so store name of parent in the mesh?
				{
					newMesh->pbData = (void*)( new char [parentName.length()+1] );
					strcpy( (char*)(newMesh->pbData), parentName.c_str() );
					newMesh->ibDataType = glMesh::USERDATA_CSTRING;
				}
				//Seriously since this is skinned....nah, not going to do it.
				// So, store name of parent in the mesh
				// lets look for the bone and then make it ours since i mean there's only one skeleton if we're loading here (so uniqueness satisfied)
				for ( unsigned int i = 0; i < vSkeleton.size(); ++i )
				{
					if ( parentName == vSkeleton[i]->name )
					{
						//cout << "Got " << parentName << endl;
						for ( uint32_t vert = 0; vert < vertexNum; vert += 1 )
						{
							newModelData->vertices[vert].bone[0] = i;
							newModelData->vertices[vert].bone[1] = 255;
							newModelData->vertices[vert].bone[2] = 255;
							newModelData->vertices[vert].bone[3] = 255;
							newModelData->vertices[vert].weight[0] = 1;
							newModelData->vertices[vert].weight[1] = 0;
							newModelData->vertices[vert].weight[2] = 0;
							newModelData->vertices[vert].weight[3] = 0;
						}
					}
				}
			}

			// Init mesh with new model data
			newMesh->Initialize( meshName, newModelData );

			// Load material for this new mesh
			if ( modelMaterial < vMaterialFiles.size() ) {
				glMaterial* newMat = new glMaterial;
				newMat->m_isSkinnedShader = true;
				newMat->loadFromFile( vMaterialFiles[modelMaterial].c_str() );
				newMesh->pmMat = newMat;
			}
			else {
				newMesh->pmMat = glMaterial::Default;
			}

			// Put the mesh into the render list
			m_glMeshlist.push_back( newMesh );

			// ========= COLLISION MESH IMPORTING ===============
			// Import collision 
			if ( pCollideIndices[i] != unsigned(-1) )
			{
				// Create Model Data
				CPhysicsData* newPhysData = new CPhysicsData();

				// Read in the section data
				sin.seekg( 0, ios::beg );
				sin.str( "" );
				sin << "$_COLLISIONMESH" << pCollideIndices[i];
				if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
				{
					cout << "Bad section find ($_COLLISIONMESH):: " << sin.str() << endl;
					continue;
				}

				// Move the data to a stream
				sin.seekg( 0, ios::beg );
				sin.str( sOutput );
				sin.seekg( 0, ios::beg );

				{
					// Read in data
					string		meshName;
					float		transformMatx [16];
					uint32_t	vertexNum;
					uint32_t	triangleNum;
					/*[string] string giving mesh name, null terminated
					[float][16] transformation matrix of the mesh. Supposed to be column-major.
					[uint32] vertexnum
					[CPhysicsVertex][n] vertex data where n is an integer from 0 to vertexnum
					[uint32] trianglenum
					[CModelTriangle][n] triangle data where n is an integer from 0 to trianglenum
					*/

					//[string] string giving mesh name
					char tempChar;
					do
					{
						sin.read( &tempChar, 1 );
						meshName += tempChar;
					}
					while ( tempChar != 0 );

					//[float][16][f] transformation matrix of the mesh. Supposed to be column-major. If animated per-vertex, f set for numframes
					sin.read( (char*)(transformMatx), sizeof( float )*16 );

					//[uint32] vertexnum
					sin.read( (char*)(&vertexNum), sizeof( uint32_t ) );

					//[CModelVertex][n][f] vertex data where n is an integer from 0 to vertexnum
					newPhysData->vertexNum = vertexNum;
					newPhysData->vertices = new CPhysicsVertex [vertexNum];
					sin.read( (char*)(newPhysData->vertices), sizeof( CPhysicsVertex ) * vertexNum );

					// Convert the vertex coordinates
					for ( uint32_t vert = 0; vert < vertexNum; vert += 1 )
					{
						Vector3d vPos = Vector3d( newPhysData->vertices[vert].x, newPhysData->vertices[vert].y, newPhysData->vertices[vert].z );
						Matrix4x4 mTransform = Matrix4x4( transformMatx );
						vPos = mTransform * vPos;
						newPhysData->vertices[vert].x = vPos.x;
						newPhysData->vertices[vert].y = vPos.y;
						newPhysData->vertices[vert].z = vPos.z;
					}

					//[uint32] trianglenum
					sin.read( (char*)(&triangleNum), sizeof( uint32_t ) );

					//[CModelTriangle][n][f] triangle data where n is an integer from 0 to trianglenum
					newPhysData->triangleNum = triangleNum;
					newPhysData->triangles = new CModelTriangle [triangleNum];
					sin.read( (char*)(newPhysData->triangles), sizeof( CModelTriangle ) * triangleNum );
				}

				// Create the new mesh with the physics data
				physMesh* newPhysMesh = new physMesh ();
				newPhysMesh->Initialize( newPhysData );

				// Put the mesh into the collide list
				m_physMeshlist.push_back( newPhysMesh );
			}

			// ========= MORPH MESH IMPORTING ===============
			if ( (iMorphNum>0) && (i == iMorphMesh) )
			{
				bDoMorphing = true;
				iMorphTarget = iMorphMesh;
				// Make a morph object for this mesh and make the skeleton point to it
				// oh god this is too easy XD
				CMorpherSet* newMorphSet = new CMorpherSet( (newModelData->vertexNum), iMorphNum );
				newMorphSet->iMorphTarget = iMorphTarget;

				CMorpher* newMorpher = new CMorpher( sFilename, newMorphSet );

				// Loop through all morphs and load them into a nice big array of delta mesh data
				for ( uint morphNum = 0; morphNum < iMorphNum; ++morphNum )
				{
					char strt [16];
					sprintf( strt, "$_MORPH%d", morphNum );
					if ( !inputFile->GetSectionData( string(strt), sOutput ) ) {
						cout << "Bad section find ($_MORPH):: " << sin.str() << endl;
						continue;
					}
					
					// Move the data to a stream
					sin.str("");
					sin.clear();
					sin.seekg( 0, ios::beg );
					sin.str( sOutput );
					sin.seekg( 0, ios::beg );

					//CMorphAction newAction;
					//[string] string giving morph name, null terminated
					string sMorphName;
					char tempChar;
					do
					{
						sin.read( &tempChar, 1 );
						sMorphName += tempChar;
					}
					while ( tempChar != 0 );
					sMorphName = sMorphName.substr( 0, sMorphName.length()-1 );

					// Add morph to list
					CMorphAction newAction( sMorphName );
					newAction.index = morphNum;
					newMorpher->AddAction( newAction );

					// Load in morph mesh data
					sin.read( (char*)(newMorphSet->GetMorphData(morphNum)), sizeof( CModelVertex ) * (newModelData->vertexNum) );
				}
			}
		}
		delete [] pCollideIndices;
		//===End Reading Mesh Block===
		//===============================

		// Set mesh skeletons
		/*for ( unsigned int n = 0; n < vMeshes.size(); n++ )
		{
			((glSkinnedMesh*)vMeshes[n])->SetSkeleton( &vSkeleton );
		}*/

		//===============================
		//==Read in the IK Block==
		{
			// First, search for an IK block
			if ( CAnimation::useHavok && inputFile->GetSectionData( string("$_IK"), sOutput ) )
			{
				// Move the data to a stream
				sin.seekg( 0, ios::beg );
				sin.str( sOutput );
				sin.seekg( 0, ios::beg );

				// newAnimation is the object we want to copy the IK info to.
				// when CAnimation is copied, it shall copy its chains as well

				//============================================================================
				//[uint32] ik num
				//============================================================================
				uint32_t iIKNum;
				sin.read( (char*)(&iIKNum), sizeof( uint32_t ) );

				// Skip if no events found
				if ( (iIKNum == 0) || sin.eof() ) {
					// Don't need to do anything
				}
				else {
					//============================================================================
					//[uint32,string,uint32,string[i]][n] ik type, null-terminated string giving name, data count, null-terminated strings for data
					//============================================================================
					char tempChar;
					for ( uint32_t iCurChain = 0; iCurChain < iIKNum; ++iCurChain )
					{
						// create new ik type
						ikinfo_t newIK;
						memset( newIK.subinfo, 0, sizeof(ftype)*4 );
						memset( newIK.bone, 0, sizeof(ftype)*5 );

						//[uint32] ik type
						uint32_t iIKType;
						sin.read( (char*)(&iIKType), sizeof( uint32_t ) );

						//[string] string giving chain name, null terminated
						string sChainName;
						do {
							sin.read( &tempChar, 1 );
							sChainName += tempChar;
						}
						while ( tempChar != 0 );
						sChainName = sChainName.substr( 0, sChainName.length()-1 );

						//[uint32] data count
						uint32_t iIKDataCount;
						sin.read( (char*)(&iIKDataCount), sizeof( uint32_t ) );

						for ( uint32_t iDataIndex = 0; iDataIndex < iIKDataCount; ++iDataIndex )
						{
							// [string[i]] data name
							string sArgumentName;
							do {
								sin.read( &tempChar, 1 );
								sArgumentName += tempChar;
							}
							while ( tempChar != 0 );
							sArgumentName = sArgumentName.substr( 0, sArgumentName.length()-1 );


							// Need to find bone name in original skeleton reference
							uint32_t target = (uint32_t)(-1);
							// WHATEVER WE'RE IN THE EFFIN LOADER SO JUST USE VSKELETON
							for ( uint32_t bone = 0; bone < vSkeleton.size(); ++bone ) {
								if ( vSkeleton[bone]->name.find( sArgumentName ) != string::npos ) {
									target = bone;
									//cout << "IK NAME MATCH: " << sArgumentName << " == " << vSkeleton[bone]->name << endl;
									break;
								}
							}
							// Print if DIDN'T find it
							if ( target == (uint32_t)(-1) ) { 
								cout << "IK NAME MISMATCH: " << sArgumentName << endl;
							}
							newIK.bone[iDataIndex] = target;
						}

						// Add IK chain
						/*CAnimActionEvent newEvent;
						newEvent.frame = iFrame;
						newEvent.name = sEventName;
						(*newAnimation)[iTargetAction].AddEvent( newEvent );*/
						newIK.type = (eAnimIKType)iIKType;
						newIK.enabled = false;
						newIK.input = Vector3d(0,0,0);
						newIK.name = sChainName;

						newAnimation->AddIKInfo( newIK );

						// Print info
						//cout << " Event: " << sEventName << " F: " << iFrame << " ta: " << iTargetAction << endl;
					}
				}
			}
		}
		//===End Reading IK Block===
		//===============================

		//===============================
		//==Read in the hitbox block==
		if ( inputFile->GetSectionData( "$_HITBOXES", sOutput ) )
		{
			// Move the data to a stream
			sin.seekg( 0, ios::beg );
			sin.str( sOutput );
			sin.seekg( 0, ios::beg );

			uint32_t hitboxCount;
			//[uint32] hitbox info count
			sin.read( (char*)(&hitboxCount), sizeof(uint32_t) );

			//cout << "Panis: " << hitboxCount << endl;
			
			sHitbox hitbox;
			for ( uint32_t hb = 0; hb < hitboxCount; ++hb )
			{
				// [uchar] boneindex
				sin.read( (char*)(&hitbox.indexLink), sizeof(uchar) );
				// [uchar] parentbone
				sin.read( (char*)(&hitbox.parentIndex), sizeof(uchar) );
				// [Vector3d] center
				sin.read( (char*)(&hitbox.center.x), sizeof(ftype)*3 );
				// [Vector3d] extents
				sin.read( (char*)(&hitbox.extents.x), sizeof(ftype)*3 );

				//cout << "aa: " << hitbox.center << "  " << hitbox.extents << endl;
				// [string] hitbox name
				uint namei = 0;
				char tempChar;
				do
				{
					sin.read( &tempChar, 1 );
					if ( tempChar != '"' && tempChar != '`' && tempChar != '\'' ) {
						hitbox.name[namei] = tempChar;
						namei += 1;
					}
				}
				while ( tempChar != 0 );
				hitbox.name[namei] = 0;

				// Add the hitbox to the list
				vHitboxes.push_back( hitbox );
			}
		}
		//===End Reading Hitbox Block===
		//===============================

		//iMiscNum
		//===========================
		//==Read in the Misc blocks==
		for ( uint32_t miscIndex = 0; miscIndex < iMiscNum; ++miscIndex )
		{
			char miscIndexId [11];
			sprintf( miscIndexId, "$_MISC%d", miscIndex );
			if ( inputFile->GetSectionData( miscIndexId, sOutput ) )
			{
				// Move the data to a stream
				sin.seekg( 0, ios::beg );
				sin.str( sOutput );
				sin.seekg( 0, ios::beg );

				char inputBufferName [64];
				char inputBufferData [128];

				uint readi;
				char tempChar;

				readi = 0;
				do
				{
					sin.read( &tempChar, 1 );
					if ( tempChar != '"' && tempChar != '`' && tempChar != '\'' ) {
						inputBufferName[readi] = tempChar;
						readi += 1;
					}
				}
				while ( tempChar != 0 );
				inputBufferName[readi] = 0; 

				readi = 0;
				do
				{
					sin.read( &tempChar, 1 );
					if ( tempChar != '"' && tempChar != '`' && tempChar != '\'' ) {
						inputBufferData[readi] = tempChar;
						readi += 1;
					}
				}
				while ( tempChar != 0 );
				inputBufferData[readi] = 0; 

				// Now parse the data
				if ( strcmp( inputBufferName, "$softbody" ) == 0 ) {
					// find the bone and marke "effect"
					// Need to find bone name in original skeleton reference
					uint32_t target = (uint32_t)(-1);
					// WHATEVER WE'RE IN THE EFFIN LOADER SO JUST USE VSKELETON
					for ( uint32_t bone = 0; bone < vSkeleton.size(); ++bone ) {
						if ( vSkeleton[bone]->name.find( inputBufferData ) != string::npos ) {
							target = bone;
							//cout << "IK NAME MATCH: " << sArgumentName << " == " << vSkeleton[bone]->name << endl;
							break;
						}
					}
					// Print if DIDN'T find it
					if ( target == (uint32_t)(-1) ) { 
						cout << "MISC NAME MISMATCH: " << inputBufferData << endl;
						throw std::exception();
					}
					else {
						vSkeleton[target]->effect = 1;
					}
				}
				// Now parse the data
				if ( strcmp( inputBufferName, "$jiggle" ) == 0 ) {
					// read in the bone name (read until the !)
					string inputString = inputBufferData;
					string boneName = inputString.substr( 0, inputString.find_first_of( '!' ) );
					string typeName = inputString.substr( inputString.find_first_of( '!' )+2 );
					// find the bone and marke "effect"
					// Need to find bone name in original skeleton reference
					uint32_t target = (uint32_t)(-1);
					// WHATEVER WE'RE IN THE EFFIN LOADER SO JUST USE VSKELETON
					for ( uint32_t bone = 0; bone < vSkeleton.size(); ++bone ) {
						if ( vSkeleton[bone]->name.find( boneName ) != string::npos ) {
							target = bone;
							//cout << "IK NAME MATCH: " << sArgumentName << " == " << vSkeleton[bone]->name << endl;
							break;
						}
					}
					// Print if DIDN'T find it
					if ( target == (uint32_t)(-1) ) { 
						cout << "MISC NAME MISMATCH: " << inputBufferData << endl;
						throw std::exception();
					}
					else
					{
						if ( typeName == "_sys_jiggle_test" ) {
							vSkeleton[target]->effect = 11;
						}
						else if ( typeName == "_sys_jiggle_breast" ) {
							vSkeleton[target]->effect = 12;
						}
						else if ( typeName == "_sys_tailwaggle" ) {
							vSkeleton[target]->effect = 25;
						}
						else if ( typeName.find( "general" ) != string::npos )
						{
							// Parse the typeName to provide additional properties
							auto m_parameters = StringUtils::Split( typeName, " ", false );
							for ( uint param = 0; param < 9; ++param )
							{
								// If input valid, then grab the input parameter
								if ( param < m_parameters.size() ) {
									vSkeleton[target]->effect_v[param] = (Real) std::stod( m_parameters[param+1] );
								}
								else {
									vSkeleton[target]->effect_v[param] = 0.0f;
								}
							}
							// Mark as general jiggle type
							vSkeleton[target]->effect = 35;
						}
					}
					// End name match
				}
			}
		}
		//===End Reading Misc Blocks===
		//=============================

		// Add the animation set to the model master now
		if ( newAnimation != NULL ) {
			ModelMaster.AddReference( sFilename, newAnimation );
		}
		else {
			throw Core::NullReferenceException();
		}

		//===============================
	}
	else
	{
		cout << "Warning: Could not open model " << sTargetFilename << endl;
		LoadSkinnedModel( "models/missing.FBX" );
	}

	delete inputFile;
}


void CSkinnedModel::SetSkeletonBindPose ( glBone* bone )
{
	if ( bone != NULL )
	{
		bone->SetBindPose();
		// loop through children
		for ( vector<Transform*>::iterator it = bone->transform.children.begin(); it != bone->transform.children.end(); it++ )
		{
			SetSkeletonBindPose( (glBone*)((*it)->owner) );
		}
	}
}
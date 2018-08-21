
#include "CModel.h"
#include "CModelLoader.h"

#include "core-ext/system/io/assets/ModelLoader.h"

#include "core/utils/string.h"
#include "core/system/io/CSegmentedFile.h"
#include "core/debug/CDebugConsole.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/FileUtils.h"

//#include "physical/physics/shapes/physMesh.h"
#include "physical/physics/shapes/PrMesh.h"

#include "renderer/resource/CModelMaster.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/object/mesh/system/rrMesh.h"

#include <sstream>

#include "core/os.h"


void CModel::LoadModel ( const string& sFilename )
{
	// Create loader and set options for the load
	core::ModelLoader loader;
	loader.m_loadMesh = true;
	loader.m_loadSkeleton = true;

	// Attempt load
	if ( !loader.LoadModel( sFilename.c_str() ) )
	{
		throw core::MissingFileException();
	}
	else
	{
		// Model needs the mesh loaded. We need to create a std::vector<rrMesh*> and pass it in
		std::vector<rrMesh*> meshList;
		for ( size_t i = 0; i < loader.meshes.size(); ++i )
		{
			// Create Mesh
			rrMesh* newMesh = new rrMesh ();

			// Create the mesh object
			arModelData* modeldata = new arModelData();
			*modeldata = loader.meshes[i].model;	// Shallow copy the data over
			// Ownership of pointers are now in modeldata
			loader.meshes[i].model.triangles = NULL;
			loader.meshes[i].model.vertices = NULL;

			// Init mesh with new model data
			newMesh->Initialize( loader.meshes[i].name.c_str(), modeldata );

			// Load material for this new mesh
			if ( loader.meshes[i].material_index < loader.materials.size() )
			{
				RrMaterial* newMat = new RrMaterial;
				newMat->m_isSkinnedShader = true;
				newMat->loadFromFile( loader.materials[loader.meshes[i].material_index].filename );
				//newMesh->pmMat = newMat;
				throw core::NotYetImplementedException();
			}
			else
			{
				//newMesh->pmMat = RrMaterial::Default;
			}

			// Update mesh's data
			newMesh->RecalculateTangents(); // TODO: Move to model conversion

			// Put the mesh into the render list
			meshList.push_back(newMesh);
		}
		RenderResources::Active()->AddMeshSet( sFilename.c_str(), meshList );
	}
}

//	bool haveConverter = IO::FileExists("_devtools/FBXtoPAD.exe");
//
//	// First check for needed file conversion!
//	string sTargetFilename = sFilename;//core::Resources::PathTo( sFilename );
//	string sFileExtention = core::utils::string::GetLower( core::utils::string::GetFileExtension( sTargetFilename ) );
//	//cout << sFileExtention << endl;
//
//	if ( sFileExtention == "fbx" )
//	{
//		// Get the vanilla filename
//		sTargetFilename = sTargetFilename.substr( 0, sTargetFilename.length()-(sFileExtention.length()+1) );
//		
//		// Run the converter program
//		if ( haveConverter )
//		{
//#ifdef _WIN32
//			string sArgument;
//			sArgument = sTargetFilename + ".FBX " + sTargetFilename + ".PAD";
//
//			LPTSTR lpCommandLine = new CHAR [1024];
//			strcpy( lpCommandLine, (string("_devtools/FBXtoPAD.exe ") + sArgument).c_str() );
//			STARTUPINFO startupInfo;
//			ZeroMemory( &startupInfo, sizeof( STARTUPINFO ) );
//			startupInfo.cb = sizeof( STARTUPINFO );
//			PROCESS_INFORMATION procInfo;
//			ZeroMemory( &procInfo, sizeof(PROCESS_INFORMATION) );
//
//			int result = CreateProcess( NULL, lpCommandLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &procInfo );
//
//			if ( !result )
//			{
//				cout << "Cannot convert model without FBX conversion devtool! (loading PAD anyways)" << endl;
//			}
//			else
//			{
//				// Wait for it to finish conversion
//				WaitForSingleObject( procInfo.hProcess, INFINITE );
//
//				// Close process and thread handles. 
//				CloseHandle( procInfo.hProcess );
//				CloseHandle( procInfo.hThread );
//			}
//#elif
//			cout << "Model conversion on other platforms not yet supported. (loading PAD anyways)" << endl;
//
//			// Get the vanilla filename
//			sTargetFilename = sTargetFilename.substr( 0, sTargetFilename.length()-4 );
//			// Set the fileobject
//			sTargetFilename = sTargetFilename + ".PAD";
//#endif
//		}
//
//		// Set the fileobject
//		sTargetFilename = sTargetFilename + ".PAD";
//
//		// Look for the valid resource to load
//		sTargetFilename = core::Resources::PathTo( sTargetFilename );
//	}
//	// End Conversion
//	//--------------------
//	
//	// Read in the file
//	CSegmentedFile* inputFile = new CSegmentedFile ( sTargetFilename );
//	if ( inputFile->ReadData() )
//	{
//		// Temporary buffer stuff
//		stringstream sin;
//		string sOutput;
//
//		/*
//		Segment Name: $_INFO
//		Keys:
//			(see documentation)
//		*/
//		//===============================
//		//==Read in the header block==
//		inputFile->GetSectionData( string("$_INFO"), sOutput );
//		sin.str( sOutput );
//		sin.seekg( 0, ios::beg );
//
//		ModelHeader header;
//		sin.read( (char*)&header, sizeof( ModelHeader ) );
//
//		uint32_t iMeshNum		= header.meshnum;
//		uint32_t iUniqueMeshNum	= header.uniquemeshnum;
//		uint32_t bAnimated		= header.animated;
//		uint32_t iFrameNum		= header.framenumber;
//		uint32_t iMatNum		= header.materialnum;
//		uint32_t iBoneNum		= header.bonenum;
//		uint32_t iSplineNum		= header.splinenum;
//		uint32_t iVersion		= header.m_version;
//		uint32_t iCameraNum		= header.cameranum;
//		uint32_t iMiscNum		= header.miscnum;
//		uint32_t iMorphNum		= header.morphnum;
//		uint32_t iMorphMesh		= header.morphmesh;
//
//		//===End Reading Header Block===
//		//===============================
//
//		vector<string> vMaterialFiles;
//		if ( iMatNum > 0 )
//		{
//			// Read in the material info
//			for ( uint32_t i = 0; i < iMatNum; i += 1 )
//			{
//				// Read in the section data
//				sin.seekg( 0, ios::beg );
//				sin.str( "" );
//				sin << "$_MATERIAL" << i;
//				if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
//				{
//					cout << "Bad section find:: " << sin.str() << endl;
//					continue;
//				}
//
//				// Move the data to a stream
//				sin.seekg( 0, ios::beg );
//				sin.str( sOutput );
//				sin.seekg( 0, ios::beg );
//
//				{
//					// Read in the material info
//					string		materialName;
//					char		isReferenced;
//					string		materialFileName;
//
//					/*[string] string giving material name, null terminated
//					[char]{bool} is referenced?
//					If it's referenced
//					[string] string giving the material file, null terminated
//					*/
//					
//					//[string] string giving mesh name
//					char tempChar;
//					do
//					{
//						sin.read( &tempChar, 1 );
//						materialName += tempChar;
//					}
//					while ( tempChar != 0 );
//
//					//[char]{bool} animated per-vertex
//					sin.read( &isReferenced, sizeof( char ) );
//
//					if ( isReferenced )
//					{
//						//[string] string giving the material file, null terminated
//						do
//						{
//							sin.read( &tempChar, 1 );
//							materialFileName += tempChar;
//						}
//						while ( tempChar != 0 );
//
//						// Add to list
//						vMaterialFiles.push_back( materialFileName );
//					}
//				}
//			}
//		}
//
//		// Read in models
//		uint32_t* pCollideIndices = new uint32_t [iMeshNum];
//		for ( uint32_t i = 0; i < iMeshNum; i += 1 )
//		{
//			// Create Model Data
//			arModelData* newModelData = new arModelData();
//			uint32_t	modelMaterial;
//			string		meshName;
//			string		parentName;
//
//			// Read in the section data
//			sin.seekg( 0, ios::beg );
//			sin.str( "" );
//			sin << "$_MESH" << i;
//			if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
//			{
//				cout << "Bad section find:: " << sin.str() << endl;
//				continue;
//			}
//
//			// Move the data to a stream
//			sin.seekg( 0, ios::beg );
//			sin.str( sOutput );
//			sin.seekg( 0, ios::beg );
//			
//			{
//				// Read in data
//				char		animatedPerVertex;
//				uint32_t	materialIndex;
//				float		transformMatx [16];
//				uint32_t	vertexNum;
//				uint32_t	triangleNum;
//				uint32_t	collisionMesh;
//				/*[string] string giving mesh name
//				[char]{bool} animated per-vertex
//				[uint32] material index
//				[float][16][f] transformation matrix of the mesh. Supposed to be column-major. If animated per-vertex, f set for numframes
//				[uint32] vertexnum
//				[arModelVertex][n][f] vertex data where n is an integer from 0 to vertexnum
//				[uint32] trianglenum
//				[arModelTriangle][n][f] triangle data where n is an integer from 0 to trianglenum
//				[uint32] index of convex collision mesh. signed(-1) if no separate collision mesh*/
//
//				//[string] string giving mesh name
//				char tempChar;
//				do
//				{
//					sin.read( &tempChar, 1 );
//					meshName += tempChar;
//				}
//				while ( tempChar != 0 );
//
//				//[char]{bool} animated per-vertex
//				sin.read( &animatedPerVertex, sizeof( char ) );
//
//				//[uint32] material index
//				sin.read( (char*)(&materialIndex), sizeof( uint32_t ) );
//				modelMaterial = materialIndex;
//				
//				//[float][16][f] transformation matrix of the mesh. Supposed to be column-major. If animated per-vertex, f set for numframes
//				sin.read( (char*)(transformMatx), sizeof( float )*16 );
//
//				//[uint32] vertexnum
//				sin.read( (char*)(&vertexNum), sizeof( uint32_t ) );
//
//				//[arModelVertex][n][f] vertex data where n is an integer from 0 to vertexnum
//				newModelData->vertexNum = vertexNum;
//				newModelData->vertices = new arModelVertex [vertexNum];
//				sin.read( (char*)(newModelData->vertices), sizeof( arModelVertex ) * vertexNum );
//
//				// Convert the vertex coordinates
//				for ( uint32_t vert = 0; vert < vertexNum; vert += 1 )
//				{
//					Vector3d vPos = Vector3d( newModelData->vertices[vert].x, newModelData->vertices[vert].y, newModelData->vertices[vert].z );
//					Matrix4x4 mTransform = Matrix4x4( transformMatx );
//					vPos = mTransform * vPos;
//					newModelData->vertices[vert].x = vPos.x;
//					newModelData->vertices[vert].y = vPos.y;
//					newModelData->vertices[vert].z = vPos.z;
//				}
//
//				//[uint32] trianglenum
//				sin.read( (char*)(&triangleNum), sizeof( uint32_t ) );
//
//				//[arModelTriangle][n][f] triangle data where n is an integer from 0 to trianglenum
//				newModelData->triangleNum = triangleNum;
//				newModelData->triangles = new arModelTriangle [triangleNum];
//				sin.read( (char*)(newModelData->triangles), sizeof( arModelTriangle ) * triangleNum );
//
//				//[uint32] index of convex collision mesh. signed(-1) if no separate collision mesh
//				sin.read( (char*)(&collisionMesh), sizeof( uint32_t ) );
//				pCollideIndices[i] = collisionMesh;
//
//				//[string] string giving parent node name. Meshes and bones are valid. This is “_is_root_” if there is no parent.
//				do
//				{
//					sin.read( &tempChar, 1 );
//					parentName += tempChar;
//				}
//				while ( tempChar != 0 );
//			}
//
//			// Create new mesh with the model data
//			rrMesh* newMesh = new rrMesh ();
//			newMesh->Initialize( meshName, newModelData );
//			//newMesh->RecalculateNormals();
//
//			// Put the mesh into the render list
//			m_glMeshlist.push_back( newMesh );
//			//vMaterials.push_back( NULL );
//
//			// Load material for this new mesh
//			if ( modelMaterial < vMaterialFiles.size() ) {
//				/*RrMaterial* newMat = new RrMaterial;
//				newMat->loadFromFile( vMaterialFiles[modelMaterial] );
//				this->SetMeshMaterial( newMat, 1, i );
//				debug::Console->PrintWarning( "Set material!\n" );*/
//				debug::Console->PrintWarning( "Set material!\n" );
//				RrMaterial* newMat = new RrMaterial;
//				newMat->loadFromFile( vMaterialFiles[modelMaterial].c_str() );
//				newMesh->pmMat = newMat;
//			}
//			else {
//				newMesh->pmMat = RrMaterial::Default;
//			}
//
//			// Import collision 
//			if ( pCollideIndices[i] != unsigned(-1) )
//			{
//				// Create Model Data
//				arModelPhysicsData* newPhysData = new arModelPhysicsData();
//
//				// Read in the section data
//				sin.seekg( 0, ios::beg );
//				sin.str( "" );
//				sin << "$_COLLISIONMESH" << pCollideIndices[i];
//				if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
//				{
//					cout << "Bad section find:: " << sin.str() << endl;
//					continue;
//				}
//
//				// Move the data to a stream
//				sin.seekg( 0, ios::beg );
//				sin.str( sOutput );
//				sin.seekg( 0, ios::beg );
//
//				{
//					// Read in data
//					string		meshName;
//					float		transformMatx [16];
//					uint32_t	vertexNum;
//					uint32_t	triangleNum;
//					/*[string] string giving mesh name, null terminated
//					[float][16] transformation matrix of the mesh. Supposed to be column-major.
//					[uint32] vertexnum
//					[arPhysicsVertex][n] vertex data where n is an integer from 0 to vertexnum
//					[uint32] trianglenum
//					[arModelTriangle][n] triangle data where n is an integer from 0 to trianglenum
//					*/
//
//					//[string] string giving mesh name
//					char tempChar;
//					do
//					{
//						sin.read( &tempChar, 1 );
//						meshName += tempChar;
//					}
//					while ( tempChar != 0 );
//
//					//[float][16][f] transformation matrix of the mesh. Supposed to be column-major. If animated per-vertex, f set for numframes
//					sin.read( (char*)(transformMatx), sizeof( float )*16 );
//
//					//[uint32] vertexnum
//					sin.read( (char*)(&vertexNum), sizeof( uint32_t ) );
//
//					//[arModelVertex][n][f] vertex data where n is an integer from 0 to vertexnum
//					newPhysData->vertexNum = vertexNum;
//					newPhysData->vertices = new arPhysicsVertex [vertexNum];
//					sin.read( (char*)(newPhysData->vertices), sizeof( arPhysicsVertex ) * vertexNum );
//
//					// Convert the vertex coordinates
//					for ( uint32_t vert = 0; vert < vertexNum; vert += 1 )
//					{
//						Vector3d vPos = Vector3d( newPhysData->vertices[vert].x, newPhysData->vertices[vert].y, newPhysData->vertices[vert].z );
//						Matrix4x4 mTransform = Matrix4x4( transformMatx );
//						vPos = mTransform * vPos;
//						newPhysData->vertices[vert].x = vPos.x;
//						newPhysData->vertices[vert].y = vPos.y;
//						newPhysData->vertices[vert].z = vPos.z;
//					}
//
//					//[uint32] trianglenum
//					sin.read( (char*)(&triangleNum), sizeof( uint32_t ) );
//
//					//[arModelTriangle][n][f] triangle data where n is an integer from 0 to trianglenum
//					newPhysData->triangleNum = triangleNum;
//					newPhysData->triangles = new arModelTriangle [triangleNum];
//					sin.read( (char*)(newPhysData->triangles), sizeof( arModelTriangle ) * triangleNum );
//				}
//
//				// Create the new mesh with the physics data
//				physMesh* newPhysMesh = new physMesh ();
//				newPhysMesh->Initialize( newPhysData );
//
//				// Put the mesh into the collide list
//				m_physMeshlist.push_back( newPhysMesh );
//			}
//		} // End mesh loop
//		delete [] pCollideIndices;
//
//		// Read in the hitboxes
//		if ( inputFile->GetSectionData( "$_HITBOXES", sOutput ) )
//		{
//			// Move the data to a stream
//			sin.seekg( 0, ios::beg );
//			sin.str( sOutput );
//			sin.seekg( 0, ios::beg );
//
//			uint32_t hitboxCount;
//			//[uint32] hitbox info count
//			sin.read( (char*)(&hitboxCount), sizeof(uint32_t) );
//			
//			sHitbox hitbox;
//			for ( uint32_t hb = 0; hb < hitboxCount; ++hb )
//			{
//				// [uchar] boneindex
//				sin.read( (char*)(&hitbox.indexLink), sizeof(uchar) );
//				// [uchar] parentbone
//				sin.read( (char*)(&hitbox.parentIndex), sizeof(uchar) );
//				// [Vector3d] center
//				sin.read( (char*)(&hitbox.center.x), sizeof(Real)*3 );
//				// [Vector3d] extents
//				sin.read( (char*)(&hitbox.extents.x), sizeof(Real)*3 );
//				// [string] hitbox name
//				uint namei = 0;
//				char tempChar;
//				do
//				{
//					sin.read( &tempChar, 1 );
//					if ( tempChar != '"' && tempChar != '`' && tempChar != '\'' ) {
//						hitbox.name[namei] = tempChar;
//						namei += 1;
//					}
//				}
//				while ( tempChar != 0 );
//				hitbox.name[namei] = 0;
//
//				// Add the hitbox to the list
//				vHitboxes.push_back( hitbox );
//			}
//		}
//
//	}
//	else
//	{
//		cout << "Warning: Could not open model \"" << sTargetFilename << "\"" << endl;
//		LoadModel( "models/missing.FBX" );
//	}
//
//
//	delete inputFile;
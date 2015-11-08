
#include "CModel.h"
#include "CModelLoader.h"

#include "core/utils/StringUtils.h"
#include "core/system/io/CSegmentedFile.h"
#include "core/debug/CDebugConsole.h"

#include "physical/physics/shapes/physMesh.h"

#include "renderer/material/glMaterial.h"
#include "renderer/object/mesh/system/glMesh.h"

#include <sstream>

#ifdef _WIN32
#	ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#endif

using namespace std;

void CModel::LoadModel ( const string& sFilename )
{
	// First check for needed file conversion!
	string sTargetFilename = sFilename;
	string sFileExtention = StringUtils::ToLower( StringUtils::GetFileExtension( sTargetFilename ) );
	//cout << sFileExtention << endl;
	if ( sFileExtention == "fbx" )
	{
		// Get the vanilla filename
		sTargetFilename = sTargetFilename.substr( 0, sTargetFilename.length()-4 );
		
		// Run the converter program
#ifdef _WIN32
		string sArgument;
		sArgument = sTargetFilename + ".FBX " + sTargetFilename + ".PAD";

		LPTSTR lpCommandLine = new CHAR [1024];
		strcpy( lpCommandLine, (string("_devtools\\FBXtoPAD.exe ") + sArgument).c_str() );
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

		// Set the fileobject
		sTargetFilename = sTargetFilename + ".PAD";

#elif
		cout << "Model conversion on other platforms not yet supported. (loading PAD anyways)" << endl;

		// Get the vanilla filename
		sTargetFilename = sTargetFilename.substr( 0, sTargetFilename.length()-4 );
		// Set the fileobject
		sTargetFilename = sTargetFilename + ".PAD";
#endif
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
					cout << "Bad section find:: " << sin.str() << endl;
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

		// Read in models
		uint32_t* pCollideIndices = new uint32_t [iMeshNum];
		for ( uint32_t i = 0; i < iMeshNum; i += 1 )
		{
			// Create Model Data
			CModelData* newModelData = new CModelData();
			uint32_t	modelMaterial;
			string		meshName;
			string		parentName;

			// Read in the section data
			sin.seekg( 0, ios::beg );
			sin.str( "" );
			sin << "$_MESH" << i;
			if ( !inputFile->GetSectionData( sin.str(), sOutput ) )
			{
				cout << "Bad section find:: " << sin.str() << endl;
				continue;
			}

			// Move the data to a stream
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
			}

			// Create new mesh with the model data
			glMesh* newMesh = new glMesh ();
			newMesh->Initialize( meshName, newModelData );
			//newMesh->RecalculateNormals();

			// Put the mesh into the render list
			m_glMeshlist.push_back( newMesh );
			//vMaterials.push_back( NULL );

			// Load material for this new mesh
			if ( modelMaterial < vMaterialFiles.size() ) {
				/*glMaterial* newMat = new glMaterial;
				newMat->loadFromFile( vMaterialFiles[modelMaterial] );
				this->SetMeshMaterial( newMat, 1, i );
				Debug::Console->PrintWarning( "Set material!\n" );*/
				Debug::Console->PrintWarning( "Set material!\n" );
				glMaterial* newMat = new glMaterial;
				newMat->loadFromFile( vMaterialFiles[modelMaterial].c_str() );
				newMesh->pmMat = newMat;
			}
			else {
				newMesh->pmMat = glMaterial::Default;
			}

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
					cout << "Bad section find:: " << sin.str() << endl;
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
		} // End mesh loop
		delete [] pCollideIndices;

		// Read in the hitboxes
		if ( inputFile->GetSectionData( "$_HITBOXES", sOutput ) )
		{
			// Move the data to a stream
			sin.seekg( 0, ios::beg );
			sin.str( sOutput );
			sin.seekg( 0, ios::beg );

			uint32_t hitboxCount;
			//[uint32] hitbox info count
			sin.read( (char*)(&hitboxCount), sizeof(uint32_t) );
			
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

	}
	else
	{
		cout << "Warning: Could not open model \"" << sTargetFilename << "\"" << endl;
		LoadModel( ".res/models/missing.FBX" );
	}


	delete inputFile;
}
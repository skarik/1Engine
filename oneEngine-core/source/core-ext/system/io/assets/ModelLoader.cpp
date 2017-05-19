
#include "core/system/System.h"
#include "core/system/io/FileUtils.h"
#include "core/utils/StringUtils.h"
#include "core/debug/console.h"

#include "core/os.h"
#include "core/system/io/CSegmentedFile.h"
#include "core/system/io/CBufferIO.h"

#include "ModelLoader.h"

#include <string>

using std::string;
using core::ModelLoader;

ModelLoader::ModelLoader ( void )
	: m_loadMesh(false), m_loadMorphs(false), m_loadActions(false), m_loadAnimation(false), m_loadSkeleton(false)
{
	;
}
ModelLoader::~ModelLoader ( void )
{
	// Free up all allocated memory if data has not been grabbed
	for ( size_t i = 0; i < meshes.size(); ++i )
	{
		if ( meshes[i].model.triangles != NULL ) {
			delete [] meshes[i].model.triangles;
			meshes[i].model.triangles = NULL;
		}
		if ( meshes[i].model.vertices != NULL ) {
			delete [] meshes[i].model.vertices;
			meshes[i].model.vertices = NULL;
		}
	}
	for ( size_t i = 0; i < collisions.size(); ++i )
	{
		if ( collisions[i].model.triangles != NULL ) {
			delete [] collisions[i].model.triangles;
			collisions[i].model.triangles = NULL;
		}
		if ( collisions[i].model.vertices != NULL ) {
			delete [] collisions[i].model.vertices;
			collisions[i].model.vertices = NULL;
		}
	}
	for ( size_t i = 0; i < morphs.size(); ++i )
	{
		if ( morphs[i].vertices != NULL ) {
			delete [] morphs[i].vertices;
			morphs[i].vertices = NULL;
		}
	}
}

//	LoadModel ( resource name )
// Attempts to load a model with the given resource name.
// If the file could not be loaded, will return false, and default model data will be loaded instead.
bool ModelLoader::LoadModel ( const char * n_resourcename )
{
	// First, create the resource name
	string model_rezname = n_resourcename;
	string model_filename = model_rezname;
	{
		string file_extension = StringUtils::ToLower( StringUtils::GetFileExtension( n_resourcename ) );
		string fbx_rezname = model_rezname;
		if ( file_extension == "" )
		{
			fbx_rezname += ".fbx";
			model_filename += ".pad";
		}
		else if ( file_extension != "fbx" )
		{
			fbx_rezname = fbx_rezname.substr( 0, fbx_rezname.find_last_of( "." ) ) + ".fbx";
		}

		// Find the file now
		string fbx_filename = core::Resources::PathTo( fbx_rezname );
		if ( ConvertFile( fbx_filename.c_str() ) == false )
		{
			debug::Console->PrintError( "ModelLoader::LoadModel : Error occurred in ModelLoader::ConvertFile call\n" );
		}

		// Make sure loading in a pad file now
		if ( file_extension == "" )
		{
			model_filename += ".pad";
		}
		else if ( file_extension != "pad" )
		{
			model_filename = model_filename.substr( 0, model_filename.find_last_of( "." ) ) + ".pad";
		}
		model_filename = core::Resources::PathTo( model_filename );
	}

	// Read in the file
	CSegmentedFile* segment_file = new CSegmentedFile ( model_filename );
	if ( !segment_file->ReadData() )
	{
		// Count not read in the file.
		debug::Console->PrintError( "ModelLoader::LoadModel : could not load model file\n" );
		delete segment_file;
		return false;
	}

	//===============================
	// Read in the header segment
	CBufferIO buffer (NULL);
	buffer = segment_file->GetSectionStream( MODELFMT_HEADER );
	if ( buffer.Valid() )
	{
		buffer.ReadData( (char*) &header, sizeof(modelFmtHeader_t) );
	}
	else
	{
		throw core::CorruptedDataException();
	}

	// Based on that data, read in the other segments

	//===============================
	// Read in the material segments
	if ( m_loadMesh )
	{
		for ( uint32_t n = 0; n < header.materialnum; ++n )
		{
			buffer = segment_file->GetSectionStream( MODELFMT_MATERIAL_PREFIX + std::to_string(n) );
			if ( buffer.Valid() )
			{
				modelFmtMaterialEntry_t entry;

				buffer.ReadString( entry.name.data );
				entry.referenced = buffer.ReadChar();
				buffer.ReadString( entry.filename.data );

				materials.push_back( entry );
			}
			else
			{
				materials.push_back( modelFmtMaterialEntry_t() );
				debug::Console->PrintError( "ModelLoader::LoadModel : could not load find material segment!\n" );
			}
		}
	}

	//===============================
	// Read in the action segment
	if ( m_loadActions )
	{
		buffer = segment_file->GetSectionStream( MODELFMT_ACTIONS );
		if ( buffer.Valid() )
		{
			uint32_t actionCount = buffer.ReadUInt32();
			for ( uint32_t n = 0; n < actionCount; ++n )
			{
				modelFmtActionEntry_t entry;

				entry.start	= buffer.ReadUInt32();
				entry.end	= buffer.ReadUInt32();
				buffer.ReadString( entry.name.data );
				entry.flags	= buffer.ReadChar();

				actions.push_back( entry );
			}
		}
	}

	//===============================
	// Read in the bone information
	if ( m_loadSkeleton )
	{
		for ( uint32_t n = 0; n < header.bonenum; ++n )
		{
			buffer = segment_file->GetSectionStream( MODELFMT_BONE_PREFIX + std::to_string(n), 1024 ); // Read in 1K for the first block. Hope to heaven that it's enough.
			if ( buffer.Valid() )
			{
				modelFmtBoneEntry_t entry;

				buffer.ReadString( entry.name.data );
				entry.parent = buffer.ReadInt32();
				buffer.ReadData( (char*)(&entry.world_transform),		sizeof(float)*10 );
				buffer.ReadData( (char*)(&entry.firstframe_transform),	sizeof(float)*10 );

				skeleton.push_back( entry );
			}
			else
			{
				skeleton.push_back( modelFmtBoneEntry_t() );
				debug::Console->PrintError( "ModelLoader::LoadModel : could not load find bone segment!\n" );
			}
		}
	}

	//===============================
	// Read in the animation information
	if ( m_loadAnimation )
	{
		for ( uint32_t n = 0; n < header.bonenum; ++n )
		{
			buffer = segment_file->GetSectionStream( MODELFMT_BONE_PREFIX + std::to_string(n) );
			if ( buffer.Valid() )
			{
				modelFmtBoneEntry_t entry;
				buffer.ReadString( entry.name.data );
				entry.parent = buffer.ReadInt32();
				buffer.ReadData( (char*)(&entry.world_transform), sizeof(float)*10 );

				// Add new animation track
				animation.push_back( modelFmtAnimationList_t() );
				animation.end()->reserve(header.framenumber);

				// Read in all the frames for the animation data.
				for ( uint32_t frame = 0; frame < header.framenumber; ++frame )
				{
					XTransform frameTransform;
					buffer.ReadData( (char*)(&frameTransform), sizeof(float)*10 );
					animation.end()->push_back(frameTransform);
				}
			}
			else
			{
				debug::Console->PrintError( "ModelLoader::LoadModel : could not load find bone segment for animation!\n" );
			}
		}
	}

	//===============================
	// Read in the mesh segment
	if ( m_loadMesh )
	{
		for ( uint32_t n = 0; n < header.meshnum; ++n )
		{
			buffer = segment_file->GetSectionStream( MODELFMT_MESH_PREFIX + std::to_string(n) );
			if ( buffer.Valid() )
			{
				modelFmtMeshEntry_t entry;

				buffer.ReadString( entry.name.data );
				entry.apv				= buffer.ReadChar();
				entry.material_index	= buffer.ReadUInt32();
				buffer.ReadData( (char*)(&entry.transform), sizeof(float)*16 );

				entry.model.vertexNum	= buffer.ReadUInt32();
				entry.model.vertices	= new arModelVertex [entry.model.vertexNum];
				buffer.ReadData( (char*)(entry.model.vertices),  sizeof(arModelVertex)*entry.model.vertexNum );

				entry.model.triangleNum	= buffer.ReadUInt32();
				entry.model.triangles	= new arModelTriangle [entry.model.triangleNum];
				buffer.ReadData( (char*)(entry.model.triangles), sizeof(arModelTriangle)*entry.model.triangleNum );

				entry.collision_index	= buffer.ReadUInt32();
				buffer.ReadString( entry.parent.data );

				meshes.push_back( entry );
			}
			else
			{
				meshes.push_back( modelFmtMeshEntry_t() );
				debug::Console->PrintError( "ModelLoader::LoadModel : could not find mesh to load!\n" );
			}
		}
	}

	//===============================
	// Read in the physmesh segment
	if ( m_loadMesh )
	{
		for ( uint32_t n = 0; n < header.meshnum; ++n )
		{
			if ( meshes[n].collision_index != unsigned(-1) )
			{
				buffer = segment_file->GetSectionStream( MODELFMT_PHYSMESH_PREFIX + std::to_string(meshes[n].collision_index) );
				if ( buffer.Valid() )
				{
					modelFmtCollisionEntry_t entry;

					buffer.ReadString( entry.name.data );
					buffer.ReadData( (char*)(&entry.transform), sizeof(float)*16 );

					entry.model.vertexNum	= buffer.ReadUInt32();
					entry.model.vertices	= new arPhysicsVertex [entry.model.vertexNum];
					buffer.ReadData( (char*)(entry.model.vertices),  sizeof(arPhysicsVertex)*entry.model.vertexNum );

					entry.model.triangleNum	= buffer.ReadUInt32();
					entry.model.triangles	= new arModelTriangle [entry.model.triangleNum];
					buffer.ReadData( (char*)(entry.model.triangles), sizeof(arModelTriangle)*entry.model.triangleNum );

					collisions.push_back( entry );
				}
				else
				{
					collisions.push_back( modelFmtCollisionEntry_t() );
					debug::Console->PrintError( "ModelLoader::LoadModel : could not find collision entry to load!\n" );
				}
			}
		}
	}

	//===============================
	// Read in the morpher segment
	if ( m_loadMorphs )
	{
		for ( uint32_t n = 0; n < header.morphnum; ++n )
		{
			buffer = segment_file->GetSectionStream( MODELFMT_MORPH_PREFIX + std::to_string(n) );
			if ( buffer.Valid() )
			{
				modelFmtMorphEntry_t entry;

				buffer.ReadString( entry.name.data );
				entry.vertices	= new arModelVertex [meshes[header.morphmesh].model.vertexNum];
				buffer.ReadData( (char*)(entry.vertices),  sizeof(arModelVertex)*meshes[header.morphmesh].model.vertexNum );

				morphs.push_back( entry );
			}
			else
			{
				morphs.push_back( modelFmtMorphEntry_t() );
				debug::Console->PrintError( "ModelLoader::LoadModel : could not find morph to load!\n" );
			}
		}
	}

	//===============================
	// Read in the IK segment
	if ( m_loadSkeleton )
	{
		buffer = segment_file->GetSectionStream( MODELFMT_IK );
		if ( buffer.Valid() )
		{
			uint32_t ikCount = buffer.ReadUInt32();
			for ( uint32_t n = 0; n < ikCount; ++n )
			{
				modelFmtIkEntry_t entry;

				entry.type		= buffer.ReadUInt32();
				buffer.ReadString( entry.name.data );
				entry.datacount	= buffer.ReadUInt32();
				for ( uint32_t i = 0; i < entry.datacount; ++i )
				{
					buffer.ReadString( entry.data[i].data );
				}

				ik.push_back( entry );
			}
		}
	}

	//===============================
	// Read in the hitbox segment
	if ( m_loadSkeleton )
	{
		buffer = segment_file->GetSectionStream( MODELFMT_HITBOXES );
		if ( buffer.Valid() )
		{
			uint32_t hitboxCount = buffer.ReadUInt32();
			for ( uint32_t n = 0; n < hitboxCount; ++n )
			{
				modelFmtHitboxEntry_t entry;

				entry.bone			= buffer.ReadChar();
				entry.parentbone	= buffer.ReadChar();
				buffer.ReadData( (char*)(&entry.center.x),  sizeof(float)*3 );
				buffer.ReadData( (char*)(&entry.extents.x), sizeof(float)*3 );
				buffer.ReadString( entry.name.data );

				hitboxes.push_back( entry );
			}
		}
	}

	//===============================
	// Read in the fx segment
	if ( m_loadSkeleton || m_loadAnimation )
	{
		for ( uint32_t n = 0; n < header.fxnum; ++n )
		{
			buffer = segment_file->GetSectionStream( MODELFMT_FX_PREFIX + std::to_string(n) );
			if ( buffer.Valid() )
			{
				modelFmtFxEntry_t entry;

				buffer.ReadData( (char*)(&entry),  sizeof(modelFmtFxEntry_t) );

				effects.push_back( entry );
			}
			else
			{
				effects.push_back( modelFmtFxEntry_t() );
				debug::Console->PrintError( "ModelLoader::LoadModel : could not find FX to load!\n" );
			}
		}
	}

	// Free up the final file
	delete segment_file;

	// Return successful read!
	return true;
}

//	ConvertFile ( filename )
// Attempts to convert file. Returns true on no convert or successful convert.
// Returns false on issue.
bool ModelLoader::ConvertFile ( const char* n_filename )
{
	bool haveConverter = IO::FileExists("_devtools/FBXtoPAD.exe");

	// Check filename's extension to see if should convert
	string targetFilename = n_filename;
	string file_extension = StringUtils::ToLower( StringUtils::GetFileExtension( targetFilename ) );

	if ( file_extension == "fbx" )
	{
		// Get the vanilla filename
		targetFilename = targetFilename.substr( 0, targetFilename.length()-(file_extension.length()+1) );

		// Run the converter program
		if ( haveConverter )
		{
#		ifdef _WIN32
			string command;
			command = "_devtools/FBXtoPAD.exe " + targetFilename + ".FBX " + targetFilename + ".PAD";

			LPTSTR lpCommandLine = new CHAR [1024];
			strcpy( lpCommandLine, command.c_str() );
			STARTUPINFO startupInfo;
			ZeroMemory( &startupInfo, sizeof( STARTUPINFO ) );
			startupInfo.cb = sizeof( STARTUPINFO );
			PROCESS_INFORMATION procInfo;
			ZeroMemory( &procInfo, sizeof(PROCESS_INFORMATION) );

			int result = CreateProcess( NULL, lpCommandLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &procInfo );

			if ( !result )
			{
				debug::Console->PrintWarning( "Cannot convert model: no FBX conversion devtool!\n" );
			}
			else
			{
				// Wait for it to finish conversion
				WaitForSingleObject( procInfo.hProcess, INFINITE );

				// Close process and thread handles. 
				CloseHandle( procInfo.hProcess );
				CloseHandle( procInfo.hThread );
			}

			delete[] lpCommandLine;
#		elif
			debug::Console->PrintWarning( "Cannot convert model: conversion can only be performed on Windows.\n" );
#		endif
		}

		// Set the fileobject
		targetFilename = targetFilename + ".PAD";

		// Look for the valid resource to load
		targetFilename = core::Resources::PathTo( targetFilename );

		// Return proper conversion
		return IO::FileExists( targetFilename );
	}
	else if ( file_extension == "pad" )
	{
		// Nothing.
		return true;
	}
	return false;
}

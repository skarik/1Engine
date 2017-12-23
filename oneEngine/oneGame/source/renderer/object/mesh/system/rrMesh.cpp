#include "rrMesh.h"
#include "core-ext/utils/MeshAttributes.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"

// Constructor
rrMesh::rrMesh ( void )
	: name("unnamed")
{
	bReady	= false;
	bShaderSetup = false;

	iVBOverts	= 0;
	iVBOfaces	= 0;

	modeldata	= NULL;

	userdata		= NULL;
	userdata_type	= kUserdataTypeNone;
}

// Destructor
rrMesh::~rrMesh ( void )
{
	// We gotta free the model
	FreeVBOData();
	FreeRAMData();

	// Based on the ibDataType, also free that
	if ( userdata_type == kUserdataTypeString ) 
	{	// String data
		delete[] ((char*)userdata);
		userdata = NULL;
		userdata_type = kUserdataTypeNone;
	}
}

const char* rrMesh::GetName ( void ) const
{
	return name.c_str();
}


// Create a new VBO associated with this rrMesh
// Removes any old VBO data.
// This object gains ownership of the model data. Well, it should, or you'll get memory errors and shit. And that's bad.
void rrMesh::Initialize ( const char* nNewName, arModelData* const pNewModelData, unsigned int frames, bool willStream )
{
	// Ensure only single tpye of frame created
	if (frames != 0 && frames != 1)
	{
		throw core::NotYetImplementedException();
	}

	name = nNewName;

	// Free the old data, duh
	FreeVBOData ( );

	modeldata = pNewModelData;

	glBindVertexArray( 0 );

	// Create new buffers
	glGenBuffers( 1, &iVBOverts );
	glGenBuffers( 1, &iVBOfaces );
	bShaderSetup = false; // With making new buffers, shader is now not ready

	// Bind to some buffer objects
	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes

	// Copy data to the buffer
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(arModelVertex) * (modeldata->vertexNum),
		modeldata->vertices,
		willStream ? GL_STREAM_DRAW : GL_STATIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(arModelTriangle) * (modeldata->triangleNum),
		modeldata->triangles,
		willStream ? GL_STREAM_DRAW : GL_STATIC_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
// Sends new VBO data to the mesh
void rrMesh::Restream ( void )
{
	// Bind to some buffer objects
	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes

	// Tell data to be discarded
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(arModelVertex) * (modeldata->vertexNum),
		NULL,
		GL_STREAM_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(arModelTriangle) * (modeldata->triangleNum),
		NULL,
		GL_STREAM_DRAW );

	// Copy data to the buffer
	void* memoryData;
	size_t bufferSize;

	bufferSize = sizeof(arModelVertex) * (modeldata->vertexNum);
	memoryData = glMapBufferRange( GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( memoryData ) {
		memcpy( memoryData, modeldata->vertices, bufferSize );
	}
	bufferSize = sizeof(arModelTriangle) * (modeldata->triangleNum);
	memoryData = glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( memoryData ) {
		memcpy( memoryData, modeldata->triangles, bufferSize );
	}
	
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );


	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

// Reputs the data into the VBO
void rrMesh::Reinitialize ( void )
{
	Initialize( name, modeldata, 1 );
}
// Recalculates normals for all meshes
void rrMesh::RecalculateNormals ( void )
{
	core::mesh::RecalculateNormals( modeldata );
	Reinitialize();
}
// Recalculates the tangents and binormals
void rrMesh::RecalculateTangents ( void )
{
	core::mesh::RecalculateTangents( modeldata );
	Reinitialize();
}

// == Private Routines ==
void rrMesh::FreeVBOData ( void )
{
	// If the mesh is ready to render, then we gotta free the model
	if ( bReady )
	{
		bReady = false;
		bShaderSetup = false;

		if ( iVBOverts )
			glDeleteBuffers( 1, &iVBOverts );
		iVBOverts = 0;
		if ( iVBOfaces )
			glDeleteBuffers( 1, &iVBOfaces );
		iVBOfaces = 0;
	}
	// Otherwise, do nothing really
}

void rrMesh::FreeRAMData ( void )
{
	if ( modeldata != NULL )
	{
		// Delete the data
		delete[] modeldata->triangles;
		delete[] modeldata->vertices;

		// Delete the mesh containers
		delete modeldata;

		// Set final result
		modeldata = NULL;
	}
}

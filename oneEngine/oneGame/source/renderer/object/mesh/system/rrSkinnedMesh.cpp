#include "rrSkinnedMesh.h"
//#include "renderer/system/glMainSystem.h"

// Constructor
rrSkinnedMesh::rrSkinnedMesh ( void )
	: rrMesh()
{
	iRenderMode = RD_GPU;
	iQualityType	= QD_MED;
	skinning_data = {0};
}
// Destructor
rrSkinnedMesh::~rrSkinnedMesh ( void )
{
	// Nothing.
}

void rrSkinnedMesh::Initialize ( const char* nNewName, arModelData* const pNewModelData, unsigned int frames, bool isStreamed )
{
	rrMesh::Initialize( nNewName, pNewModelData, frames, false );

	// normalize weights
	for ( unsigned int vert = 0; vert < modeldata->vertexNum; vert++ )
	{
		float ft = (
			modeldata->vertices[vert].weight[0] +
			modeldata->vertices[vert].weight[1] +
			modeldata->vertices[vert].weight[2] +
			modeldata->vertices[vert].weight[3]);
		modeldata->vertices[vert].weight[0] /= ft;
		modeldata->vertices[vert].weight[1] /= ft;
		modeldata->vertices[vert].weight[2] /= ft;
		modeldata->vertices[vert].weight[3] /= ft;
	}
}

rrSkinnedMesh* rrSkinnedMesh::Copy ( void )
{
	rrSkinnedMesh* newmesh = new rrSkinnedMesh();

	newmesh->bReady	= bReady;
	newmesh->iVBOverts	= iVBOverts;
	newmesh->iVBOfaces	= iVBOfaces;
	newmesh->modeldata	= modeldata;
	newmesh->userdata	= userdata;
	newmesh->userdata_type = userdata_type;
	newmesh->name = name;

	newmesh->iRenderMode = iRenderMode;
	newmesh->iQualityType	= iQualityType;
	newmesh->useSkinning	= useSkinning;

	// normalize weights
	for ( unsigned int vert = 0; vert < modeldata->vertexNum; vert++ )
	{
		float ft = (
			modeldata->vertices[vert].weight[0] +
			modeldata->vertices[vert].weight[1] +
			modeldata->vertices[vert].weight[2] +
			modeldata->vertices[vert].weight[3]);
		modeldata->vertices[vert].weight[0] /= ft;
		modeldata->vertices[vert].weight[1] /= ft;
		modeldata->vertices[vert].weight[2] /= ft;
		modeldata->vertices[vert].weight[3] /= ft;
	}

	glBindVertexArray( 0 );
	// New output buffers need to be generated
	glGenBuffers( 1, &newmesh->iVBOverts );
	glGenBuffers( 1, &newmesh->iVBOfaces );
	{
		// Bind to some buffer objects
		glBindBuffer( GL_ARRAY_BUFFER,			newmesh->iVBOverts ); // for vertex coordinates
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	newmesh->iVBOfaces ); // for face vertex indexes

															 // Copy data to the buffer
		glBufferData( GL_ARRAY_BUFFER,
			sizeof(arModelVertex) * (modeldata->vertexNum),
			modeldata->vertices,
			GL_STATIC_DRAW );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER,
			sizeof(arModelTriangle) * (modeldata->triangleNum),
			modeldata->triangles,
			GL_STATIC_DRAW );

		// bind with 0, so, switch back to normal pointer operation
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

	// Check errors
	GL_ACCESS GL.CheckError();

	return newmesh;
}

void rrSkinnedMesh::SetSkinningData ( const skinningData_t& n_skinning_data )
{
	skinning_data = n_skinning_data;
}

void rrSkinnedMesh::UpdateVBO ( void )
{
	// Only need to update the vertex VBO
	// Bind to some buffer objects
	glBindBuffer( GL_ARRAY_BUFFER, iVBOverts ); // for vertex coordinates

	// Copy data to the buffer
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(arModelVertex) * (modeldata->vertexNum),
		modeldata->vertices,
		GL_STREAM_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

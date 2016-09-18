
// Includes
#include "glMesh.h"
#include "renderer/material/glMaterial.h"

#include "renderer/system/glMainSystem.h"

// Constructor
glMesh::glMesh ( void )
	: mName("unnamed")
{
	bReady	= false;
	bShaderSetup = false;

	iVBOverts	= 0;
	iVBOfaces	= 0;

	pmData	= NULL;
	meshNum	= 0;

	pbData = NULL;
	ibDataType = 0;

	pmMat = NULL;
}

// Destructor
glMesh::~glMesh ( void )
{
	// We gotta free the model
	FreeVBOData();
	FreeRAMData();
	// Free up the used material reference
	if ( pmMat != NULL )
	{
		pmMat->removeReference();
		pmMat = NULL;
	}
	// Based on the ibDataType, also free that
	if ( ibDataType == USERDATA_CSTRING ) 
	{	// String data
		delete [] ((char*)pbData);
		pbData = NULL;
		ibDataType = USERDATA_NONE;
	}
}

const string& glMesh::GetName ( void ) const
{
	return mName;
}


// Create a new VBO associated with this glMesh
// Removes any old VBO data.
// This object gains ownership of the model data. Well, it should, or you'll get memory errors and shit. And that's bad.
void glMesh::Initialize ( const string& nNewName, CModelData* const pNewModelData, unsigned int frames, bool willStream )
{
	mName = nNewName;

	// Free the old data, duh
	FreeVBOData ( );

	pmData = pNewModelData;
	meshNum = frames;

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
		sizeof(CModelVertex) * (pmData->vertexNum),
		pmData->vertices,
		willStream ? GL_STREAM_DRAW : GL_STATIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(CModelTriangle) * (pmData->triangleNum),
		pmData->triangles,
		willStream ? GL_STREAM_DRAW : GL_STATIC_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
// Sends new VBO data to the mesh
void glMesh::Restream ( void )
{
	// Bind to some buffer objects
	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes

	// Tell data to be discarded
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(CModelVertex) * (pmData->vertexNum),
		NULL,
		GL_STREAM_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(CModelTriangle) * (pmData->triangleNum),
		NULL,
		GL_STREAM_DRAW );

	// Copy data to the buffer
	void* memoryData;
	size_t bufferSize;

	bufferSize = sizeof(CModelVertex) * (pmData->vertexNum);
	memoryData = glMapBufferRange( GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( memoryData ) {
		memcpy( memoryData, pmData->vertices, bufferSize );
	}
	bufferSize = sizeof(CModelTriangle) * (pmData->triangleNum);
	memoryData = glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( memoryData ) {
		memcpy( memoryData, pmData->triangles, bufferSize );
	}
	
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );


	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

// Reputs the data into the VBO
void glMesh::Reinitialize ( void )
{
	Initialize( mName, pmData, meshNum );
}
// Recalculates normals for all meshes
void glMesh::RecalculateNormals ( void )
{
	for ( unsigned int i = 0; i < meshNum; i += 1 )
	{
		RecalculateNormals( &(pmData[i]) );
	}
	Reinitialize();
}

// Render the mesh
//void glMesh::Render ( void )
//{
//	// Draw current Mesh
//	// Bind to the buffer objects
//	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes
//	
//	// Tell where the vertex coordinates are in the array
//	/*glVertexPointer( 3, GL_FLOAT, sizeof(CModelVertex), 0 ); 
//	glNormalPointer( GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*6) );
//	glTexCoordPointer( 3, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*3) );
//	glMaterial::current->setShaderAttributesDefault();
//	glColorPointer( 4, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*15) );*/
//	// Check if material has been initialized for this fucker.
//	//if ( !glMaterial::current->hasInitializedMeshAttributes() )
//
//	/*glDisableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);*/
//	/*
//	if ( glMaterial::current == NULL ) {
//		throw std::exception();
//	}
//	if ( bShaderSetup == false )
//	{
//		glMaterial::current->initializeMeshAttributes();
//		GL.CheckError();
//		bShaderSetup = true;
//	}
//	else {
//		glMaterial::current->forwardMeshAttributes();
//		GL.CheckError();
//		bShaderSetup = true;
//	}*/
//	throw std::exception("BLEH");
//	/*
//	glVertexPointer( 3, GL_FLOAT, sizeof(CModelVertex), 0 ); 
//	glNormalPointer( GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*6) );
//	glTexCoordPointer( 3, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*3) );
//	glColorPointer( 4, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*15) );
//	*/
//	/*if ( glMaterial::current->useSkinning )
//	{
//		GLint weightLoc, boneLoc;
//		weightLoc = glGetAttribLocation( glMaterial::current->pShader->get_program(),"sys_BoneWeights" );
//		glEnableVertexAttribArray( weightLoc );
//		glVertexAttribPointer( weightLoc, 4, GL_FLOAT, false, sizeof(CModelVertex), ((char*)4) + (sizeof(float)*19) );
//		boneLoc = glGetAttribLocation( glMaterial::current->pShader->get_program(),"sys_BoneIndices" );
//		glEnableVertexAttribArray( boneLoc );
//		glVertexAttribPointer( boneLoc, 4, GL_UNSIGNED_BYTE, false, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*19) );
//		//cout << unsigned int( pmData[0].vertices[pmData[0].vertexNum-1].bone[3] ) << endl;
//	}*/
//
//	//glDisable(GL_CULL_FACE);
//	
//	// Draw the sutff
//	glDrawElements( GL_TRIANGLES, pmData->triangleNum*3, GL_UNSIGNED_INT, 0 );
//
//	// bind with 0, so, switch back to normal pointer operation
//	glBindBuffer( GL_ARRAY_BUFFER, 0 );
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//
//	//glEnable(GL_CULL_FACE);
//	
//	/*GLd.BeginPrimitive GL_LINES );
//		for ( int i = 0; i < pmData->vertexNum; i += 1 )
//			glVertex3fv( &(pmData->vertices[i].x) );
//	GLd.EndPrimitive();*/
//	//glDisable(GL_CULL_FACE);
//	/*glBegin ( GL_TRIANGLES );
//	for ( unsigned int i = 0; i < pmData->triangleNum; i += 1 )
//		for ( int j = 0; j < 3; j += 1 )
//		{
//			glVertex3fv( &(pmData->vertices[pmData->triangles[i].vert[j]].x) );
//			glNormal3fv( &(pmData->vertices[pmData->triangles[i].vert[j]].nx) );
//			//glTexCoord2fv( &(pmData->vertices[pmData->triangles[i].vert[j]].u) );
//			glTexCoord2fv( &(pmData->vertices[pmData->triangles[i].vert[j]].x) + 3 );
//		}
//	GLd.EndPrimitive();*/
//
//	//cout << pmData->vertices[1].x << " : ";
//}

// == Private Routines ==
void glMesh::FreeVBOData ( void )
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

void glMesh::FreeRAMData ( void )
{
	if ( pmData != NULL )
	{
		// Delete the data
		for ( unsigned int i = 0; i < meshNum; i += 1 )
		{
			delete [] pmData[i].triangles;
			delete [] pmData[i].vertices;
		}

		// Delete the mesh containers
		if ( meshNum == 1 )
			delete pmData;
		else if ( meshNum > 1 )
			delete [] pmData;

		// Set final result
		pmData = NULL;
	}
}


// Recalculate Normals
void glMesh::RecalculateNormals ( CModelData* modelData )
{
	// Some standard mesh information that you should have lying around.
	// vertex is your vertex structure that just contains a normal and position here.
	// vertices is a pointer to the first vertex
	// indices is a pointer to the first index
	// num_verts is number of vertices
	// num_indices is number of indices
	// each face of the mesh is made up of three vertices.

	std::vector<Vector3d>* normal_buffer = new std::vector<Vector3d> [modelData->vertexNum];

	//for( int i = 0; i < modelData->vertexNum; i += 3 )
	for ( unsigned int i = 0; i < modelData->triangleNum; i += 1 )
	{
		// get the three vertices that make the face
		Vector3d p1 = _VertexToVect( modelData->vertices[modelData->triangles[i].vert[0]] );
		Vector3d p2 = _VertexToVect( modelData->vertices[modelData->triangles[i].vert[1]] );
		Vector3d p3 = _VertexToVect( modelData->vertices[modelData->triangles[i].vert[2]] );

		// Calculate the face normal
		Vector3d v1 = p2 - p1;
		Vector3d v2 = p3 - p1;
		Vector3d normal = v1.cross( v2 );

		normal = normal.normal();

		// Store the face's normal for each of the vertices that make up the face.
		normal_buffer[modelData->triangles[i].vert[0]].push_back( normal );
		normal_buffer[modelData->triangles[i].vert[1]].push_back( normal );
		normal_buffer[modelData->triangles[i].vert[2]].push_back( normal );
	}

	// Now loop through each vertex vector, and avarage out all the normals stored.
	for( unsigned int i = 0; i < modelData->vertexNum; ++i )
	{
		Vector3d normalResult ( 0,0,0 );
		/*modelData->vertices[i].nx = 0;
		modelData->vertices[i].ny = 0;
		modelData->vertices[i].nz = 0;*/
		for( unsigned int j = 0; j < normal_buffer[i].size(); ++j )
		{
			//vertices[i].normal += normal_buffer[i][j];
			/*modelData->vertices[i].nx += normal_buffer[i][j].x;
			modelData->vertices[i].ny += normal_buffer[i][j].y;
			modelData->vertices[i].nz += normal_buffer[i][j].z;*/
			normalResult += normal_buffer[i][j];
		}
		//vertices[i].normal /= normal_buffer[i].size();
		/*modelData->vertices[i].nx /= normal_buffer[i].size();
		modelData->vertices[i].ny /= normal_buffer[i].size();
		modelData->vertices[i].nz /= normal_buffer[i].size();*/
		normalResult = normalResult.normal();
		modelData->vertices[i].nx = normalResult.x;
		modelData->vertices[i].ny = normalResult.y;
		modelData->vertices[i].nz = normalResult.z;
	}

	delete [] normal_buffer;
}

#include "glTMeshContainer.h"
#include "renderer/system/glMainSystem.h"

using namespace renderer;

glTMeshContainer::glTMeshContainer ( void )
{
	vbo_verts = 0;
	vbo_faces = 0;
	element_count = 0;
}
glTMeshContainer::~glTMeshContainer ( void )
{
	GL_ACCESS;
	if ( vbo_verts ) GL.FreeBuffer( &vbo_verts ); vbo_verts = 0;
	if ( vbo_faces ) GL.FreeBuffer( &vbo_faces ); vbo_faces = 0;
}

void glTMeshContainer::UploadVerts ( const arTerrainVertex* verts, const size_t vert_count )
{
	GL_ACCESS; if ( vbo_verts == 0 ) GL.CreateBuffer( &vbo_verts );

	size_t newBufferSize = sizeof(arTerrainVertex)*vert_count;
	glBindBuffer( GL_ARRAY_BUFFER, vbo_verts );
	glBufferData( GL_ARRAY_BUFFER, newBufferSize, NULL, GL_DYNAMIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, newBufferSize, verts ); GL.CheckError();
	/*void* p = glMapBufferRange( GL_ARRAY_BUFFER, 0, newBufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	//void* p = glMapBufferRange( GL_ARRAY_BUFFER, 0, newBufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT );
	if ( p )
	{
		memcpy( p, verts, newBufferSize );
		glUnmapBuffer( GL_ARRAY_BUFFER );
	}*/
}
void glTMeshContainer::UploadFaces ( const arModelTriangle* faces, const size_t face_count )
{
	GL_ACCESS; if ( vbo_faces == 0 ) GL.CreateBuffer( &vbo_faces );

	size_t newBufferSize = sizeof(arTerrainVertex)*face_count;
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_faces );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, newBufferSize, NULL, GL_DYNAMIC_DRAW );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, newBufferSize, faces ); GL.CheckError();

	// Set number of the triangles as well
	element_count = face_count * 3;
	/*void* p = glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, newBufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( p )
	{
		memcpy( p, faces, newBufferSize );
		glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
	}*/
}
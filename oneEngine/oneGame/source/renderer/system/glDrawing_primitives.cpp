#include "glDrawing.h"

#include "renderer/material/RrMaterial.h"

#define GL_MAX_PRIMITIVE_VERTEX_COUNT 2048

//===============================================================================================//
// Primitive Buffer handling
//===============================================================================================//

glHandle glDrawing::BeginPrimitive ( unsigned int n_primitiveType, RrMaterial* n_material )
{
	// Allocate mesh area
	if ( prim_vertex_list == NULL ) {
		prim_vertex_list = new arModelVertex [GL_MAX_PRIMITIVE_VERTEX_COUNT];
	}

	// First unused in prim_list is at index prim_count
	glHandle primHandle = prim_count;

	// Check if need to add new request to prim_list
	if ( primHandle >= prim_list.size() )
	{
		prim_list.push_back( rrVBufferStreaming() );
	}

	// Check to generate VBO
	if ( prim_list[primHandle].vboObject == 0 ) {
		glGenBuffers( 1, &(prim_list[primHandle].vboObject) );
	}
	// Check to either generate or clear VAO
	if ( prim_list[primHandle].vaoObject == 0 ) {
		glGenVertexArrays( 1, &(prim_list[primHandle].vaoObject ) );
	}

	// Reset element count
	prim_list[primHandle].elementCount = 0;
	// Set object type
	prim_list[primHandle].drawMode = n_primitiveType;
	// Set material
	prim_list[primHandle].material = n_material;

	// Prepare material constants now
	prim_list[primHandle].material->prepareShaderConstants(NULL, false);

	return primHandle;
}

void glDrawing::EndPrimitive ( const glHandle n_primitive )
{
	if ( prim_list[n_primitive].elementCount > 0 )
	{
		// First bind the VAO
		glBindVertexArray( prim_list[n_primitive].vaoObject );
		// Then disable all values in the VAO (this part of the engine is the only place where they get reused)
		for ( uchar i = 0; i < 16; ++i ) {
			if ( prim_list[n_primitive].enabledAttributes[i] ) {			// TODO: DO MATCHING WITH CURRENT MATERIAL.
				prim_list[n_primitive].enabledAttributes[i] = false;
				glDisableVertexAttribArray(i);
			}
		}

		// Now, bind the VBO and stream the data to it
		glBindBuffer( GL_ARRAY_BUFFER, prim_list[prim_count].vboObject );
		//glBufferData( GL_ARRAY_BUFFER, sizeof(arModelVertex) * prim_list[prim_count].elementCount, prim_vertex_list, GL_STREAM_DRAW );

		GLsizeiptr bufferSize = sizeof(arModelVertex) * prim_list[prim_count].elementCount;
		glBufferData( GL_ARRAY_BUFFER, bufferSize, NULL, GL_STREAM_DRAW );
		//void* memoryData = glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
		void* memoryData = glMapBufferRange( GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
		if ( memoryData ) {
			memcpy( memoryData, prim_vertex_list, bufferSize );
			glUnmapBuffer( GL_ARRAY_BUFFER );
		}

		/*GLsizeiptr bufferSize = sizeof(arModelVertex) * prim_list[prim_count].elementCount;
		glBufferData( GL_ARRAY_BUFFER, bufferSize, NULL, GL_STREAM_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, bufferSize, prim_vertex_list );*/
		
		// Now, send the material attributes
		prim_list[n_primitive].material->bindPassAtrribs();
		//RrMaterial::current->bindPassAtrribs(RrMaterial::current_pass);
		//RrMaterial::current->setShaderConstants( NULL, false );
		// Mark the enabled attributes
		for ( uchar i = 0; i < 16; ++i ) {
			prim_list[prim_count].enabledAttributes[i] = RrPassForward::enabled_attributes[i];
		}
		// Draw the current primitive
		glDrawArrays( prim_list[prim_count].drawMode, 0, prim_list[prim_count].elementCount );
	}
	// Increment prim_count
	prim_count += 1;
}


void glDrawing::P_AddVertex	( const Vector3d& n_position )
{
	int i = prim_list[prim_count].elementCount;
	if ( i >= GL_MAX_PRIMITIVE_VERTEX_COUNT ) {
		return;
	}

	prim_vertex_list[i].x = n_position.x;
	prim_vertex_list[i].y = n_position.y;
	prim_vertex_list[i].z = n_position.z;

	prim_vertex_list[i].u = prim_next_texcoord.x;
	prim_vertex_list[i].v = prim_next_texcoord.y;
	prim_vertex_list[i].w = prim_next_texcoord.z;

	prim_vertex_list[i].nx = prim_next_normal.x;
	prim_vertex_list[i].ny = prim_next_normal.y;
	prim_vertex_list[i].nz = prim_next_normal.z;

	prim_vertex_list[i].tx = prim_next_tangent.x;
	prim_vertex_list[i].ty = prim_next_tangent.y;
	prim_vertex_list[i].tz = prim_next_tangent.z;

	Vector3d binormal = prim_next_normal.cross( prim_next_tangent );
	prim_vertex_list[i].bx = binormal.x;
	prim_vertex_list[i].by = binormal.y;
	prim_vertex_list[i].bz = binormal.z;

	prim_vertex_list[i].r = prim_next_color.red;
	prim_vertex_list[i].g = prim_next_color.green;
	prim_vertex_list[i].b = prim_next_color.blue;
	prim_vertex_list[i].a = prim_next_color.alpha;

	prim_list[prim_count].elementCount += 1;
}
void glDrawing::P_PushTexcoord	( const Vector3d& n_texcoord )
{
	prim_next_texcoord = n_texcoord;
}
void glDrawing::P_PushColor		( const Color& n_color )
{
	prim_next_color = n_color;
}
void glDrawing::P_PushNormal	( const Vector3d& n_normal )
{
	prim_next_normal = n_normal;
}
void glDrawing::P_PushTangent	( const Vector3d& n_tangent )
{
	prim_next_tangent = n_tangent;
}

#undef GL_MAX_PRIMITIVE_VERTEX_COUNT
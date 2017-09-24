#include "RrScopedMeshRenderer.h"

#include "renderer/system/glSystem.h"
#include "renderer/material/RrMaterial.h"

RrScopedMeshRenderer::RrScopedMeshRenderer ( void )
{

}

static GLenum _primitiveModeToGlMode( renderer::rrPrimitiveMode mode )
{
	switch (mode)
	{
	case renderer::kPrimitiveModeLineList:
		return GL_LINES;
	case renderer::kPrimitiveModeLineStrip:
		return GL_LINE_STRIP;
	case renderer::kPrimitiveModePointList:
		return GL_POINTS;
	case renderer::kPrimitiveModeTriangleList:
		return GL_TRIANGLES;
	case renderer::kPrimitiveModeTriangleStrip:
		return GL_TRIANGLE_STRIP;
	case renderer::kPrimitiveModeTriangleFan:
		return GL_TRIANGLE_FAN;
	}
	return GL_INVALID_ENUM;
}

//	render() : Renders the given mesh builder.
void RrScopedMeshRenderer::render (
	CRenderableObject* owner, 
	RrMaterial* material,
	uchar pass,
	const IrrMeshBuilder& meshBuilder )
{
	arModelData model = meshBuilder.getModelData();

	// Bind the material
	material->bindPass(pass);

	// First, generate the mesh.
	glHandle vbuf = 0;
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	// Stream OpenGL style
	GLsizeiptr bufferSize = model.vertexNum * sizeof(arModelVertex);
	glBufferData( GL_ARRAY_BUFFER, bufferSize, NULL, GL_STREAM_DRAW );
	void* memoryData = glMapBufferRange( GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( memoryData )
	{
		memcpy( memoryData, model.vertices, bufferSize );
		glUnmapBuffer( GL_ARRAY_BUFFER );
	}

	// Create the VAO
	glHandle vao = 0;
	glGenVertexArrays(1, &vao);

	// First bind the VAO
	glBindVertexArray( vao );
	// Then disable all values in the VAO (this part of the engine is the only place where they get reused)
	//for ( uchar i = 0; i < 16; ++i ) {
	//	if ( prim_list[n_primitive].enabledAttributes[i] ) {			// TODO: DO MATCHING WITH CURRENT MATERIAL.
	//		prim_list[n_primitive].enabledAttributes[i] = false;
	//		glDisableVertexAttribArray(i);
	//	}
	//}
	// Now, bind the VBO and stream the data to it
	glBindBuffer( GL_ARRAY_BUFFER, vbuf );
	// Bind the material attributes with the VAO and vertex buffer
	material->bindPassAtrribs();

	// Draw the mesh.
	glEnum mode = _primitiveModeToGlMode( meshBuilder.getPrimitiveMode() );
	glDrawArrays( mode, 0, model.vertexNum );

	// Free the buffer and VAO
	glDeleteBuffers(1, &vbuf);
	glDeleteVertexArrays(1, &vao);
}
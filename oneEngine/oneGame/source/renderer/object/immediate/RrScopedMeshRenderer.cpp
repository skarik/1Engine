#include "RrScopedMeshRenderer.h"

//#include "renderer/system/glSystem.h"
//#include "renderer/material/RrMaterial.h"

RrScopedMeshRenderer::RrScopedMeshRenderer ( void )
{

}

static GLenum _primitiveModeToGlMode( renderer::rrPrimitiveMode mode )
{
	switch (mode)
	{
	case renderer::kPrimitiveModeLineList:
	case renderer::kPrimitiveModeLineList_Indexed:
		return GL_LINES;
	case renderer::kPrimitiveModeLineStrip:
		return GL_LINE_STRIP;
	case renderer::kPrimitiveModePointList:
		return GL_POINTS;
	case renderer::kPrimitiveModeTriangleList:
	case renderer::kPrimitiveModeTriangleList_Indexed:
		return GL_TRIANGLES;
	case renderer::kPrimitiveModeTriangleStrip:
		return GL_TRIANGLE_STRIP;
	case renderer::kPrimitiveModeTriangleFan:
		return GL_TRIANGLE_FAN;
	}
	return GL_INVALID_ENUM;
}
static bool _primiteModeIndexed( renderer::rrPrimitiveMode mode )
{
	if (mode == renderer::kPrimitiveModeTriangleList_Indexed ||
		mode == renderer::kPrimitiveModeLineList_Indexed)
	{
		return true;
	}
	return false;
}

//	render() : Renders the given mesh builder.
void RrScopedMeshRenderer::render (
	RrRenderObject* owner, 
	RrMaterial* material,
	uchar pass,
	const IrrMeshBuilder& meshBuilder )
{
	arModelData model = meshBuilder.getModelData();
	const glEnum mode = _primitiveModeToGlMode( meshBuilder.getPrimitiveMode() );
	const bool indexed = _primiteModeIndexed( meshBuilder.getPrimitiveMode() );

	// Skip if we have an empty model.
	if (model.vertexNum <= 0) {
		return;
	}

	// Bind the material
	material->bindPass(pass);

	// First, generate the mesh.
	glHandle vbuf = 0;
	{
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
	}
	// Generate the IBO if needed
	glHandle ibuf = 0;
	if (indexed)
	{
		glGenBuffers(1, &ibuf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
		// Stream OpenGL style
		GLsizeiptr bufferSize = model.triangleNum * sizeof(arModelTriangle);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, bufferSize, NULL, GL_STREAM_DRAW );
		void* memoryData = glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
		if ( memoryData )
		{
			memcpy( memoryData, model.triangles, bufferSize );
			glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
		}
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
	// Now, bind the VBO for the attribs and the IBO for drawing.
	glBindBuffer( GL_ARRAY_BUFFER, vbuf );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibuf );
	// Bind the material attributes with the VAO and vertex buffer
	material->bindPassAtrribs();

	// Draw the mesh.
	if (indexed)
	{
		glDrawElements( mode, model.triangleNum * 3, GL_UNSIGNED_INT, NULL );
	}
	else
	{
		glDrawArrays( mode, 0, model.vertexNum );
	}

	// Free the buffer and VAO
	glDeleteBuffers(1, &vbuf);
	if (indexed)
	{
		glDeleteBuffers(1, &ibuf);
	}
	glDeleteVertexArrays(1, &vao);
}
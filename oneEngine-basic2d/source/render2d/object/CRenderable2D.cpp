
#include "CRenderable2D.h"

#include "renderer/material/glShader.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

#include "renderer/system/glMainSystem.h"

CRenderable2D::CRenderable2D ( void )
	: CRenderableObject()
{
	// Use a default 2D material
	m_material = new glMaterial();
	m_material->setTexture( 0, new CTexture("null") );
	m_material->passinfo.push_back( glPass() );
	m_material->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	m_material->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	m_material->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	m_material->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;

	// Start with empty buffers
	m_buffer_verts = NIL;
	m_buffer_tris = NIL;
}

CRenderable2D::~CRenderable2D ()
{ GL_ACCESS
	// Material reference released automatically

	// Still have to release buffers
	if ( m_buffer_verts != NIL ) {
		GL.FreeBuffer( &m_buffer_verts );
		m_buffer_verts = NIL;
	}
	if ( m_buffer_tris != NIL ) {
		GL.FreeBuffer( &m_buffer_tris );
		m_buffer_tris = NIL;
	}

}

//		SetSpriteFile ( c-string sprite filename )
// Sets the sprite filename to load or convert
void CRenderable2D::SetSpriteFile ( const char* n_sprite_filename )
{
	// TODO: Convert the texture. For now, set the material based on the input file.
	m_material->setTexture(
		0,
		new CTexture (
			n_sprite_filename, 
			Texture2D, RGBA8,
			1024,1024, Clamp,Clamp,
			MipmapNone,SamplingPoint
		)
	);
}

//		PushModelData()
// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
void CRenderable2D::PushModeldata ( void )
{ GL_ACCESS

	GL.BindVertexArray( 0 );

	// Create new buffers
	if ( m_buffer_verts == NIL )
		GL.CreateBuffer( &m_buffer_verts );
	if ( m_buffer_tris == NIL )
		GL.CreateBuffer( &m_buffer_tris );
	//bShaderSetup = false; // With making new buffers, shader is now not ready

	// Bind to some buffer objects
	GL.BindBuffer( GL_ARRAY_BUFFER,			m_buffer_verts ); // for vertex coordinates
	GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER,	m_buffer_tris ); // for face vertex indexes

	// Copy data to the buffer
	GL.UploadBuffer( GL_ARRAY_BUFFER,
		sizeof(CModelVertex) * (m_modeldata.vertexNum),
		m_modeldata.vertices,
		GL_STATIC_DRAW );
	GL.UploadBuffer( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(CModelTriangle) * (m_modeldata.triangleNum),
		m_modeldata.triangles,
		GL_STATIC_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	GL.UnbindBuffer( GL_ARRAY_BUFFER );
	GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );
}

//		Render()
// Render the model using the 2D engine's style
bool CRenderable2D::Render ( const char pass )
{ GL_ACCESS

	//GL.beginOrtho( 0,0, 640,480, -100,100 );

	// For now, we will render the same way as the 3d meshes render
	GL.Transform( &(transform) );
	m_material->m_bufferSkeletonSize = 0;
	m_material->m_bufferMatricesSkinning = 0;
		GL.CheckError();
	m_material->bindPass(pass);
		GL.CheckError();
	//parent->SendShaderUniforms(this);
	//	GL.CheckError();
	BindVAO( pass, m_buffer_verts, m_buffer_tris );
		GL.CheckError();
	GL.DrawElements( GL_TRIANGLES, m_modeldata.triangleNum*3, GL_UNSIGNED_INT, 0 );
		GL.CheckError();

	//GL.endOrtho();
	// Success!
	return true;
}
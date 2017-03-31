
#include "Box2DDebugger.h"

#include "physical/physics/CPhysics.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"

#include "renderer/debug/CDebugDrawer.h"

Box2DDebugger::Box2DDebugger ( void )
	:b2Draw(), CRenderableObject()
{
	Physics::SetDebugRenderer(this);

	RrMaterial* new_material = RrMaterial::Default->copy();
	new_material->deferredinfo.clear();
	SetMaterial( new_material );
	new_material->removeReference();

	memset(&m_modeldata, 0, sizeof(ModelData));

	SetFlags( b2Draw::e_shapeBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit );
}

Box2DDebugger::~Box2DDebugger ()
{
	Physics::SetDebugRenderer(NULL);
}

//		PostRender()
// Push the current model information to the GPU.
bool Box2DDebugger::EndRender ( void ) 
{ GL_ACCESS
	Physics::RenderDebugInfo();
	/*

	// Make sure we don't destroy an existing VAO with the GL_ELEMENT_ARRAY_BUFFER binding.
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

	// Success!
	return true;*/
	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool Box2DDebugger::Render ( const char pass ) 
{ GL_ACCESS
	// Do not render if no buffer to render with
	if ( m_buffer_verts == 0 || m_buffer_tris == 0 || m_modeldata.triangleNum == 0 )
	{
		return true;
	}

	// For now, we will render the same way as the 3d meshes render
	GL.Transform( &(transform.world) );
	m_material->m_bufferSkeletonSize = 0;
	m_material->m_bufferMatricesSkinning = 0;
	m_material->bindPass(pass);
	BindVAO( pass, m_buffer_verts, m_buffer_tris );
	GL.DrawElements( GL_TRIANGLES, m_modeldata.triangleNum*3, GL_UNSIGNED_INT, 0 );

	// Success!
	return true;
}

// Box2D Interface:

/// Draw a closed polygon provided in CCW order.
void Box2DDebugger::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	for (int32 i = 0; i < vertexCount; ++i)
	{
		DrawSegment( vertices[i], vertices[(i + 1) % vertexCount], color );
	}
}

/// Draw a solid closed polygon provided in CCW order.
void Box2DDebugger::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{

}

/// Draw a circle.
void Box2DDebugger::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	int divs = (int)(8 + radius / 16.0F);
	for ( int i = 0; i < divs; ++i )
	{
		float angle0 = i * 2.0F * (Real)PI / divs;
		float angle1 = (i + 1) * 2.0F * (Real)PI / divs;
		b2Vec2 pos0 = center + b2Vec2(radius * cos(angle0), radius * sin(angle0));
		b2Vec2 pos1 = center + b2Vec2(radius * cos(angle1), radius * sin(angle1));

		DrawSegment( pos0, pos1, color );
	}
}

/// Draw a solid circle.
void Box2DDebugger::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{

}

/// Draw a line segment.
void Box2DDebugger::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	debug::Drawer->DrawLine( Vector3d(p1.x, p1.y, 0), Vector3d(p2.x, p2.y, 0), Color(color.r, color.g, color.b, color.a) );
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
void Box2DDebugger::DrawTransform(const b2Transform& xf)
{
	DrawSegment( xf.p + b2Vec2( xf.q.c * 16, xf.q.s * 16 ), xf.p - b2Vec2( xf.q.c * 16, xf.q.s * 16 ), b2Color( 1.0F, 0.0F, 0.0F ) );
	DrawSegment( xf.p + b2Vec2( xf.q.s * 16, xf.q.c * 16 ), xf.p - b2Vec2( xf.q.s * 16, xf.q.c * 16 ), b2Color( 0.0F, 1.0F, 0.0F ) );
}
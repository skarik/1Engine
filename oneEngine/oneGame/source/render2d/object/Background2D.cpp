#include "Background2D.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/material/RrMaterial.h"

renderer::Background2D::Background2D ( void )
	: CRenderable2D()
{
	// Create a model for a simple quad
	m_modeldata.triangleNum = 2;
	m_modeldata.triangles = new arModelTriangle[2];
	{
		m_modeldata.triangles[0].vert[0] = 0;
		m_modeldata.triangles[0].vert[1] = 1;
		m_modeldata.triangles[0].vert[2] = 2;
		m_modeldata.triangles[1].vert[0] = 0;
		m_modeldata.triangles[1].vert[1] = 2;
		m_modeldata.triangles[1].vert[2] = 3;
	}
	m_modeldata.vertexNum = 4;
	m_modeldata.vertices = new arModelVertex[4];
	memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);
	{
		for ( int i = 0; i < 4; ++i )
		{
			m_modeldata.vertices[i].color = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		}

		m_modeldata.vertices[0].position = Vector2f(-1.0F, -1.0F);
		m_modeldata.vertices[1].position = Vector2f(-1.0F, +1.0F);
		m_modeldata.vertices[2].position = Vector2f(+1.0F, +1.0F);
		m_modeldata.vertices[3].position = Vector2f(+1.0F, -1.0F);
	}
	// Upload the model
	PushModeldata();
	// Set the sprite to use to render the background
	SetSpriteFile( "textures/black.jpg" );

	// Update material to disable depth write
	m_material->passinfo[0].b_depthmask = false;
	m_material->passinfo[0].b_depthtest = true;

	// Remove deferred pass from the shader so it only renders in forward mode
	//m_material->passinfo.clear();
	m_material->deferredinfo.clear();
}
renderer::Background2D::~Background2D ( void )
{
	delete [] m_modeldata.triangles;
	delete [] m_modeldata.vertices;
}

bool renderer::Background2D::PreRender ( void )
{
	// Set the position to follow the camera
	transform.world.position = RrCamera::activeCamera->transform.position;
	transform.world.position.z = 495;
	// Set the scale to fill the screen
	transform.world.scale.x = RrCamera::activeCamera->ortho_size.x;
	transform.world.scale.y = RrCamera::activeCamera->ortho_size.y;

	return CRenderable2D::PreRender();
}
bool renderer::Background2D::Render ( const char pass )
{
	// Render normally otherwise
	return CRenderable2D::Render(pass);
}


#include "Background2D.h"
#include "renderer/camera/CCamera.h"

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
	{
		for ( int i = 0; i < 4; ++i ) {
			m_modeldata.vertices[i].r = 1.0F;
			m_modeldata.vertices[i].g = 1.0F;
			m_modeldata.vertices[i].b = 1.0F;
			m_modeldata.vertices[i].a = 1.0F;
			m_modeldata.vertices[i].z = 0;
		}

		m_modeldata.vertices[0].x = -1;
		m_modeldata.vertices[0].y = -1;

		m_modeldata.vertices[1].x = -1;
		m_modeldata.vertices[2].y = +1;

		m_modeldata.vertices[2].x = +1;
		m_modeldata.vertices[2].y = +1;

		m_modeldata.vertices[3].x = +1;
		m_modeldata.vertices[3].y = -1;
	}
	// Upload the model
	PushModeldata();
	// Set the sprite to use to render the background
	SetSpriteFile( "textures/black.jpg" );
}
renderer::Background2D::~Background2D ( void )
{
	delete [] m_modeldata.triangles;
	delete [] m_modeldata.vertices;
}

bool renderer::Background2D::Render ( const char pass )
{
	// Set the position to follow the camera
	transform.world.position = CCamera::activeCamera->transform.position;
	transform.world.position.z = 495;
	// Set the scale to fill the screen
	transform.world.scale.x = CCamera::activeCamera->ortho_size.x;
	transform.world.scale.y = CCamera::activeCamera->ortho_size.y;

	// Render normally otherwise
	return CRenderable2D::Render(pass);
}

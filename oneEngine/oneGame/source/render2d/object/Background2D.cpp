#include "Background2D.h"
#include "renderer/camera/RrCamera.h"
//#include "renderer/material/RrMaterial.h"

renderer::Background2D::Background2D ( void )
	: CRenderable2D()
{
	// Create a model for a simple quad
	m_modeldata.indexNum = 6;
	m_modeldata.indices = new uint16_t[6];
	{
		m_modeldata.indices[0] = 0;
		m_modeldata.indices[1] = 1;
		m_modeldata.indices[2] = 2;
		m_modeldata.indices[3] = 0;
		m_modeldata.indices[4] = 2;
		m_modeldata.indices[5] = 3;
	}
	m_modeldata.vertexNum = 4;
	m_modeldata.position = new Vector3f[4];
	m_modeldata.color = new Vector4f[4];
	memset(m_modeldata.position, 0, sizeof(Vector3f) * m_modeldata.vertexNum);
	memset(m_modeldata.color, 0, sizeof(Vector4f) * m_modeldata.vertexNum);
	{
		for ( int i = 0; i < 4; ++i )
		{
			m_modeldata.color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		}

		m_modeldata.position[0] = Vector2f(-1.0F, -1.0F);
		m_modeldata.position[1] = Vector2f(-1.0F, +1.0F);
		m_modeldata.position[2] = Vector2f(+1.0F, +1.0F);
		m_modeldata.position[3] = Vector2f(+1.0F, -1.0F);
	}
	// Upload the model
	PushModeldata();

	// Set the sprite to use to render the background
	SetSpriteFile( "textures/black", NULL );

	// Update material to disable depth write
	/*m_material->passinfo[0].b_depthmask = false;
	m_material->passinfo[0].b_depthtest = true;

	// Remove deferred pass from the shader so it only renders in forward mode
	//m_material->passinfo.clear();
	m_material->deferredinfo.clear();*/
}
renderer::Background2D::~Background2D ( void )
{
	delete[] m_modeldata.indices;
	delete[] m_modeldata.position;
	delete[] m_modeldata.color;
}

bool renderer::Background2D::PreRender ( rrCameraPass* cameraPass )
{
	// Set the position to follow the camera
	transform.world.position = RrCamera::activeCamera->transform.position;
	transform.world.position.z = 495;
	// Set the scale to fill the screen
	transform.world.scale.x = RrCamera::activeCamera->orthoSize.x;
	transform.world.scale.y = RrCamera::activeCamera->orthoSize.y;

	return CRenderable2D::PreRender(cameraPass);
}
bool renderer::Background2D::Render ( const rrRenderParams* params )
{
	// Render normally otherwise
	return CRenderable2D::Render(params);
}

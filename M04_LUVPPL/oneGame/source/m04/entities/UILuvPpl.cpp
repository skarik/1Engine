#include "UILuvPpl.h"

#include "core/system/Screen.h"
#include "core-ext/utils/MeshBuilder.h"

using namespace M04;

UILuvPpl::UILuvPpl ( void )
	: CGameBehavior(), CStreamedRenderable2D()
{
	memset( &m_modeldata, 0, sizeof(arModelData) );

	// Set up the material and sprite:
	SetSpriteFile( "sprites/ui/ui_hudparts_tex.gal" );
}

UILuvPpl::~UILuvPpl ( void )
{
	if ( m_modeldata.vertices )
	{
		delete [] m_modeldata.vertices;
		m_modeldata.vertices = NULL;
	}
	if ( m_modeldata.triangles )
	{
		delete [] m_modeldata.triangles;
		m_modeldata.triangles = NULL;
	}
}

void UILuvPpl::Update ( void )
{
	// generate mesh:
	
	// Estimate needed amount of vertices for the text:

	if (m_modeldata.triangles == NULL)
	{
		delete [] m_modeldata.triangles;
		delete [] m_modeldata.vertices;

		m_modeldata.triangles = new arModelTriangle [512];
		m_modeldata.vertices = new arModelVertex [1024];

		memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * 1024);
		for ( uint i = 0; i < 1024; ++i )
		{
			m_modeldata.vertices[i].r = 1.0F;
			m_modeldata.vertices[i].g = 1.0F;
			m_modeldata.vertices[i].b = 1.0F;
			m_modeldata.vertices[i].a = 1.0F;
		}
	}

	m_modeldata.triangleNum = 0;
	m_modeldata.vertexNum = 0;

	Real dx, dy;

	dx = Screen::Info.width * -0.25F + 16;
	dy = Screen::Info.height * -0.25F + 16;
	for ( int i = 0; i < 3; ++i )
	{
		core::meshbuilder::Quad( &m_modeldata, Rect(dx + 16*i,dy,16,16), Rect(0,0,1.0F/8,1.0F/8) );
	}

	// Now with the mesh built, push it to the modeldata :)
	//PushModeldata();
	StreamLockModelData();
}

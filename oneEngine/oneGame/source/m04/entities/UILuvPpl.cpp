#include "UILuvPpl.h"

#include "core/system/Screen.h"
#include "core-ext/utils/MeshBuilder.h"

using namespace M04;

UILuvPpl::UILuvPpl ( void )
	: CGameBehavior(), CStreamedRenderable2D()
{
	memset( &m_modeldata, 0, sizeof(arModelData) );

	// Set up the material and sprite:
	SetSpriteFile( "sprites/ui/ui_hudparts_tex.gal", NULL );
}

UILuvPpl::~UILuvPpl ( void )
{
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.indices);
}

void UILuvPpl::Update ( void )
{
	// generate mesh:
	
	// Estimate needed amount of vertices for the text:

	if (m_modeldata.indices == NULL)
	{
		delete[] m_modeldata.position;
		delete[] m_modeldata.color;
		delete[] m_modeldata.texcoord0;
		delete[] m_modeldata.indices;

		m_modeldata.indices = new uint16_t [512 * 3];
		m_modeldata.position = new Vector3f [1024];
		m_modeldata.color = new Vector4f [1024];
		m_modeldata.texcoord0 = new Vector3f [1024];

		for ( uint i = 0; i < 1024; ++i )
		{
			m_modeldata.color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		}
	}

	m_modeldata.indexNum = 0;
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


#include "m04/entities/TalkerBox.h"

#include "core/math/Rect.h"
#include "core/math/Math.h"
#include "core/math/Easing.h"

#include "render2d/object/CTextMesh.h"

using namespace M04;



class TalkerBox::TalkerBoxBackground : public CRenderable2D
{
public:
	TalkerBoxBackground ( void )
		: CRenderable2D()
	{
		SetSpriteFile( "sprites/ui/ui_msgbox.gal" );

		memset( &m_modeldata, 0, sizeof(ModelData) );
	}
	TalkerBoxBackground::~TalkerBoxBackground ( void )
	{
		if ( m_modeldata.triangles )
		{
			delete [] m_modeldata.triangles;
		}
		if ( m_modeldata.vertices )
		{
			delete [] m_modeldata.vertices;
		}
	}

	void GenerateMesh ( const Vector2d& size )
	{
		// Estimate needed amount of vertices for the text:

		if (m_modeldata.triangles == NULL)
		{
			delete [] m_modeldata.triangles;
			delete [] m_modeldata.vertices;

			m_modeldata.triangleNum = 2 * 16;
			m_modeldata.vertexNum = 4 * 16;

			m_modeldata.triangles = new CModelTriangle [m_modeldata.triangleNum];
			m_modeldata.vertices = new CModelVertex [m_modeldata.vertexNum];
		}

		// Reset mesh data:

		memset(m_modeldata.vertices, 0, sizeof(CModelVertex) * m_modeldata.vertexNum);
		for ( uint i = 0; i < m_modeldata.vertexNum; ++i )
		{
			m_modeldata.vertices[i].r = 1.0F;
			m_modeldata.vertices[i].g = 1.0F;
			m_modeldata.vertices[i].b = 1.0F;
			m_modeldata.vertices[i].a = 1.0F;
		}

		m_modeldata.vertices[0 + 0].x = 0;
		m_modeldata.vertices[0 + 0].y = 0;
		m_modeldata.vertices[0 + 0].u = 0;
		m_modeldata.vertices[0 + 0].v = 0;
		//m_spriteInfo.fullsize.x

		uint32_t current_vertex = 0;
		uint32_t current_triangle = 0;

#define PUSH_QUAD( position, uvs ) \
		m_modeldata.vertices[current_vertex + 0].x = position.pos.x; \
		m_modeldata.vertices[current_vertex + 0].y = position.pos.y; \
		m_modeldata.vertices[current_vertex + 0].u = uvs.pos.x / m_spriteInfo.fullsize.x; \
		m_modeldata.vertices[current_vertex + 0].v = uvs.pos.y / m_spriteInfo.fullsize.y; \
		m_modeldata.vertices[current_vertex + 1].x = position.pos.x; \
		m_modeldata.vertices[current_vertex + 1].y = position.pos.y + position.size.y; \
		m_modeldata.vertices[current_vertex + 1].u = uvs.pos.x / m_spriteInfo.fullsize.x; \
		m_modeldata.vertices[current_vertex + 1].v = (uvs.pos.y + uvs.size.y) / m_spriteInfo.fullsize.y; \
		m_modeldata.vertices[current_vertex + 2].x = position.pos.x + position.size.x; \
		m_modeldata.vertices[current_vertex + 2].y = position.pos.y + position.size.y; \
		m_modeldata.vertices[current_vertex + 2].u = (uvs.pos.x + uvs.size.x) / m_spriteInfo.fullsize.x; \
		m_modeldata.vertices[current_vertex + 2].v = (uvs.pos.y + uvs.size.y) / m_spriteInfo.fullsize.y; \
		m_modeldata.vertices[current_vertex + 3].x = position.pos.x + position.size.x; \
		m_modeldata.vertices[current_vertex + 3].y = position.pos.y; \
		m_modeldata.vertices[current_vertex + 3].u = (uvs.pos.x + uvs.size.x) / m_spriteInfo.fullsize.x; \
		m_modeldata.vertices[current_vertex + 3].v = uvs.pos.y / m_spriteInfo.fullsize.y; \
		m_modeldata.triangles[current_triangle + 0].vert[0] = current_vertex + 0; \
		m_modeldata.triangles[current_triangle + 0].vert[1] = current_vertex + 1; \
		m_modeldata.triangles[current_triangle + 0].vert[2] = current_vertex + 2; \
		m_modeldata.triangles[current_triangle + 1].vert[0] = current_vertex + 0; \
		m_modeldata.triangles[current_triangle + 1].vert[1] = current_vertex + 2; \
		m_modeldata.triangles[current_triangle + 1].vert[2] = current_vertex + 3; \
		current_vertex += 4; \
		current_triangle += 2;

		// Top of the box:

		PUSH_QUAD( Rect(0,0, 16,16),
			Rect(0,0,16,16) );
		PUSH_QUAD( Rect(16,0, size.x/2 - 24,16),
			Rect(16,0,8,16) );

		PUSH_QUAD( Rect(size.x/2 - 8,0, 16,16),
			Rect(24,0,m_spriteInfo.fullsize.x - 48,16) );

		PUSH_QUAD( Rect(size.x/2 + 8,0, size.x/2 - 24,16),
			Rect(m_spriteInfo.fullsize.x - 24,0,8,16) );
		PUSH_QUAD( Rect(size.x - 16,0, 16,16),
			Rect(m_spriteInfo.fullsize.x - 16,0,16,16) );

		// Middle of the box:

		PUSH_QUAD( Rect(0,16, 16,size.y/2 - 24),
			Rect(0,16,16,8) );
		PUSH_QUAD( Rect(0,size.y/2 - 8, 16,16),
			Rect(0,24,16,m_spriteInfo.fullsize.x - 48) );
		PUSH_QUAD( Rect(0,size.y/2 + 8, 16,size.y/2 - 24),
			Rect(0,m_spriteInfo.fullsize.x - 24,16,8) );

		PUSH_QUAD( Rect(size.x - 16,16, 16,size.y/2 - 24),
			Rect(m_spriteInfo.fullsize.x - 16,16,16,8) );
		PUSH_QUAD( Rect(size.x - 16,size.y/2 - 8, 16,16),
			Rect(m_spriteInfo.fullsize.x - 16,24,16,m_spriteInfo.fullsize.x - 48) );
		PUSH_QUAD( Rect(size.x - 16,size.y/2 + 8, 16,size.y/2 - 24),
			Rect(m_spriteInfo.fullsize.x - 16,m_spriteInfo.fullsize.x - 24,16,8) );

		// Bottom of the box:

		PUSH_QUAD( Rect(0,size.y - 16, 16,16),
			Rect(0,m_spriteInfo.fullsize.y-16,16,16) );
		PUSH_QUAD( Rect(16,size.y - 16, size.x/2 - 24,16),
			Rect(16,m_spriteInfo.fullsize.y-16,8,16) );

		PUSH_QUAD( Rect(size.x/2 - 8,size.y - 16, 16,16),
			Rect(24,m_spriteInfo.fullsize.y-16,m_spriteInfo.fullsize.x - 48,16) );

		PUSH_QUAD( Rect(size.x/2 + 8,size.y - 16, size.x/2 - 24,16),
			Rect(m_spriteInfo.fullsize.x - 24,m_spriteInfo.fullsize.y-16,8,16) );
		PUSH_QUAD( Rect(size.x - 16,size.y - 16, 16,16),
			Rect(m_spriteInfo.fullsize.x - 16,m_spriteInfo.fullsize.y-16,16,16) );

#undef PUSH_QUAD


		// Now with the mesh built, push it to the modeldata :)
		PushModeldata();
	}
};


TalkerBox::TalkerBox ( void )
	: TalkerBase()
{
	width = 128;

	m_textmesh = new CTextMesh();
	m_textmesh->SetFont("ComicNeue-Angular-Bold.ttf", 12, FW_BOLD);
	m_textmesh->m_text = "";
	m_textmesh->UpdateText();
	m_textmesh->m_max_width = width;

	m_boxbackground = new TalkerBoxBackground();

	m_displayTickerTimer = 0;
	m_displayLength = 0;
	m_displayString = "";
}

TalkerBox::~TalkerBox ( void )
{
	delete_safe(m_textmesh);
}

void TalkerBox::Update ( void )
{
	int display_length_previous = m_displayString.length();


	// Fade in the box
	m_fadeLerp += Time::deltaTime * 2.0F;
	m_fadeLerp = Math::saturate(m_fadeLerp);
	Real m_fadeLerpX = Math::saturate((m_fadeLerp - 0.5F) * 2.0F);
	Real m_fadeLerpY = Math::saturate(m_fadeLerp * 2.0F);

	if ( m_fadeLerp > 0.5F )
	{
		// Lerp the text (30 characters per second)
		m_displayTickerTimer += 30.0F * Time::deltaTime;
		while ( m_displayTickerTimer > 1.0F )
		{
			m_displayTickerTimer -= 1.0F;
			m_displayLength += 1;
		}

		m_displayString = text.substr( 0, std::min<int>(m_displayLength, (int)text.length()) );
	}

	// If there's a change in the string, update the boxes
	if ( display_length_previous != m_displayString.length() )
	{
		// Update text
		m_textmesh->m_text = m_displayString;
		m_textmesh->UpdateText();
	}

	// Lerp in the box
	Vector2d estimatedWidth;
	estimatedWidth.x = Math::lerp( Easing::back_out(m_fadeLerpX), 16, width + 14 );
	estimatedWidth.y = Math::lerp( Easing::back_out(m_fadeLerpY), 16, 8 + 14 * Math::round(0.5F + text.length() / 32.0F)  );

	// Update box size
	m_boxbackground->GenerateMesh( estimatedWidth );

	// Update text position
	m_textmesh->transform.position		= Vector3d( position.x, position.y, -100 );
	m_boxbackground->transform.position	= Vector3d(
		position.x - 4.0F + (width + 14 - estimatedWidth.x) * 0.5F,
		position.y - 16.0F,
		-99 );
}


void TalkerBox::Show ( void )
{
	m_fadeLerp = 0.0F;
}
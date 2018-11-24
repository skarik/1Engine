
#include "m04/entities/TalkerBox.h"

#include "core/math/Rect.h"
#include "core/math/Math.h"
#include "core/math/Easing.h"
#include "core/math/random/Random.h"

#include "renderer/material/RrMaterial.h"
#include "render2d/object/CTextMesh.h"

#include <cctype>

using namespace M04;

class TalkerBox::TalkerBoxBackground : public CRenderable2D
{
public:
	TalkerBoxBackground ( void )
		: CRenderable2D()
	{
		SetSpriteFile( "sprites/ui/ui_msgbox.gal" );

		memset( &m_modeldata, 0, sizeof(arModelData) );
	}
	~TalkerBoxBackground ( void )
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

	void GenerateMesh ( const Vector2f& size )
	{
		// Estimate needed amount of vertices for the text:

		if (m_modeldata.triangles == NULL)
		{
			delete [] m_modeldata.triangles;
			delete [] m_modeldata.vertices;

			m_modeldata.triangleNum = 2 * 16;
			m_modeldata.vertexNum = 4 * 16;

			m_modeldata.triangles = new arModelTriangle [m_modeldata.triangleNum];
			m_modeldata.vertices = new arModelVertex [m_modeldata.vertexNum];
		}

		// Reset mesh data:

		memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);
		for ( uint i = 0; i < m_modeldata.vertexNum; ++i )
		{
			m_modeldata.vertices[i].r = 1.0F;
			m_modeldata.vertices[i].g = 1.0F;
			m_modeldata.vertices[i].b = 1.0F;
			m_modeldata.vertices[i].a = 1.0F;
		}

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

		PUSH_QUAD( Rect(0,0, 16.0F,16.0F),
			Rect(0,0,16.0F,16.0F) );
		PUSH_QUAD( Rect(16.0F,0, size.x/2 - 24.0F,16.0F),
			Rect(16.0F,0,8.0F,16.0F) );

		PUSH_QUAD( Rect(size.x/2 - 8,0, 16.0F,16.0F),
			Rect(24.0F,0,m_spriteInfo.fullsize.x - 48.0F,16.0F) );

		PUSH_QUAD( Rect(size.x/2 + 8.0F,0, size.x/2 - 24.0F,16.0F),
			Rect(m_spriteInfo.fullsize.x - 24.0F,0,8.0F,16.0F) );
		PUSH_QUAD( Rect(size.x - 16.0F,0, 16.0F,16.0F),
			Rect(m_spriteInfo.fullsize.x - 16.0F,0,16.0F,16.0F) );

		// Middle of the box:

		PUSH_QUAD( Rect(0,16.0F, 16.0F,size.y/2 - 24.0F),
			Rect(0,16.0F,16.0F,8.0F) );
		PUSH_QUAD( Rect(0,size.y/2 - 8.0F, 16.0F,16.0F),
			Rect(0,24.0F,16.0F,m_spriteInfo.fullsize.x - 48.0F) );
		PUSH_QUAD( Rect(0,size.y/2 + 8.0F, 16.0F,size.y/2 - 24.0F),
			Rect(0,m_spriteInfo.fullsize.x - 24.0F,16.0F,8.0F) );

		PUSH_QUAD( Rect(size.x - 16.0F,16.0F, 16.0F,size.y/2 - 24.0F),
			Rect(m_spriteInfo.fullsize.x - 16.0F,16.0F,16.0F,8.0F) );
		PUSH_QUAD( Rect(size.x - 16.0F,size.y/2 - 8.0F, 16.0F,16.0F),
			Rect(m_spriteInfo.fullsize.x - 16.0F,24.0F,16.0F,m_spriteInfo.fullsize.x - 48.0F) );
		PUSH_QUAD( Rect(size.x - 16.0F,size.y/2 + 8.0F, 16.0F,size.y/2 - 24.0F),
			Rect(m_spriteInfo.fullsize.x - 16.0F,m_spriteInfo.fullsize.x - 24.0F,16.0F,8.0F) );

		// Bottom of the box:

		PUSH_QUAD( Rect(0,size.y - 16.0F, 16.0F,16.0F),
			Rect(0,m_spriteInfo.fullsize.y-16.0F,16.0F,16.0F) );
		PUSH_QUAD( Rect(16.0F,size.y - 16.0F, size.x/2 - 24.0F,16.0F),
			Rect(16.0F,m_spriteInfo.fullsize.y-16.0F,8.0F,16.0F) );

		PUSH_QUAD( Rect(size.x/2 - 8.0F,size.y - 16.0F, 16.0F,16.0F),
			Rect(24.0F,m_spriteInfo.fullsize.y-16.0F,m_spriteInfo.fullsize.x - 48.0F,16.0F) );

		PUSH_QUAD( Rect(size.x/2 + 8.0F,size.y - 16.0F, size.x/2 - 24.0F,16.0F),
			Rect(m_spriteInfo.fullsize.x - 24.0F,m_spriteInfo.fullsize.y-16.0F,8.0F,16.0F) );
		PUSH_QUAD( Rect(size.x - 16.0F,size.y - 16.0F, 16.0F,16.0F),
			Rect(m_spriteInfo.fullsize.x - 16.0F,m_spriteInfo.fullsize.y-16.0F,16.0F,16.0F) );

#undef PUSH_QUAD


		// Now with the mesh built, push it to the modeldata :)
		PushModeldata();
	}
};

class TalkerBox::TalkerBoxBackgroundColor : public CRenderable2D
{
public:
	TalkerBoxBackgroundColor ( void )
		: CRenderable2D()
	{
		SetSpriteFile( "textures/white.jpg" );
		m_material->passinfo[0].m_blend_mode = renderer::BM_MULTIPLY_X2;

		memset( &m_modeldata, 0, sizeof(arModelData) );
	}
	~TalkerBoxBackgroundColor ( void )
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

	void GenerateMesh ( const Vector2f& size )
	{
		// Estimate needed amount of vertices for the text:

		if (m_modeldata.triangles == NULL)
		{
			delete [] m_modeldata.triangles;
			delete [] m_modeldata.vertices;

			m_modeldata.triangleNum = 2;
			m_modeldata.vertexNum = 4;

			m_modeldata.triangles = new arModelTriangle [m_modeldata.triangleNum];
			m_modeldata.vertices = new arModelVertex [m_modeldata.vertexNum];
		}

		// Reset mesh data:

		memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * m_modeldata.vertexNum);
		for ( uint i = 0; i < m_modeldata.vertexNum; ++i )
		{
			m_modeldata.vertices[i].r = 0.3F;
			m_modeldata.vertices[i].g = 0.2F;
			m_modeldata.vertices[i].b = 0.4F;
			m_modeldata.vertices[i].a = 1.0F;
		}

		// Build the quad:
		m_modeldata.vertices[0].x = 0;
		m_modeldata.vertices[0].y = 0;

		m_modeldata.vertices[1].x = 0;
		m_modeldata.vertices[1].y = size.y;

		m_modeldata.vertices[2].x = size.x;
		m_modeldata.vertices[2].y = size.y;

		m_modeldata.vertices[3].x = size.x;
		m_modeldata.vertices[3].y = 0;

		// Create quad's triangles:
		m_modeldata.triangles[0].vert[0] = 0; 
		m_modeldata.triangles[0].vert[1] = 1; 
		m_modeldata.triangles[0].vert[2] = 2; 

		m_modeldata.triangles[1].vert[0] = 0;
		m_modeldata.triangles[1].vert[1] = 2; 
		m_modeldata.triangles[1].vert[2] = 3; 

		// Now with the mesh built, push it to the modeldata :)
		PushModeldata();
	}
};


TalkerBox::TalkerBox ( void )
	: TalkerBase()
{
	// Default box setup
	width = 128;
	scramble_lerp = false;

	// Create vizuallllz
	m_textmesh = new CTextMesh();
	m_textmesh->SetFont("Aldrich-Regular.ttf", 12, FW_BOLD);
	m_textmesh->m_text = "";
	m_textmesh->UpdateText();
	m_textmesh->m_max_width = width;

	m_boxbackground = new TalkerBoxBackground();
	m_boxbackgroundblend = new TalkerBoxBackgroundColor();

	// Init talker shit
	InitializeTalkerValues();
}
void TalkerBox::InitializeTalkerValues ( void )
{
	m_fadeIn = true;

	m_displayTickerTimer = 0;
	m_displayLength = 0;
	m_displayString = "";

	m_scrambleFixTimer = 0;
	m_scrambleTimer = 0;
	m_scrambleLength = 0;
	m_scrambleUpdate = false;
}


TalkerBox::~TalkerBox ( void )
{
	delete_safe(m_textmesh);
	delete_safe(m_boxbackground);
	delete_safe(m_boxbackgroundblend);
}

void TalkerBox::Update ( void )
{
	int display_length_previous = m_displayString.length();

	// Fade in the box
	if ( m_fadeIn )
	{
		m_fadeLerp += Time::deltaTime * 2.0F;
	}
	else
	{
		m_fadeLerp -= Time::deltaTime * 2.0F;
	}
	m_fadeLerp = math::saturate(m_fadeLerp);
	Real m_fadeLerpX = math::saturate((m_fadeLerp - 0.5F) * 2.0F);
	Real m_fadeLerpY = math::saturate(m_fadeLerp * 2.0F);

	if ( m_fadeLerp > 0.5F )
	{
		// Lerp the text (30 characters per second)
		m_displayTickerTimer += 30.0F * Time::deltaTime;
		while ( m_displayTickerTimer > 1.0F && m_displayLength < (int)text.length() )
		{
			m_displayTickerTimer -= 1.0F;
			m_displayLength += 1;
			m_scrambleLength += 1;
		}

		m_displayString = text.substr( 0, std::min<int>(m_displayLength, (int)text.length()) );

		if ( scramble_lerp )
		{
			// Reduce the scrambling at 25 characters per second 
			m_scrambleFixTimer += 25.0F * Time::deltaTime;
			while ( m_scrambleFixTimer > 1.0F )
			{
				m_scrambleFixTimer -= 1.0F;
				if ( m_scrambleLength > 0 )
				{
					m_scrambleLength -= 1;
				}
			}
			// Scramble at 24 FPS
			m_scrambleTimer += 24.0F * Time::deltaTime;
			if ( m_scrambleTimer > 1.0F )
			{
				m_scrambleTimer = std::min( 1.0F, m_scrambleTimer - 1.0F );
				m_scrambleUpdate = true;

				for ( int i = 0; i < m_scrambleLength; ++i )
				{
					int index = ((int)m_displayString.length()) - 1 - i;
					if ( index > 0 && !std::isspace(m_displayString[index]) )
					{
						m_displayString[index] = 'a' + (Random.Next() % 26);
					}
				}
			}
			else
			{
				m_scrambleUpdate = false;
			}
		}
	}

	// If there's a change in the string, update the boxes
	if ( !scramble_lerp )
	{
		if ( display_length_previous != m_displayString.length() )
		{
			// Update text
			m_textmesh->m_text = m_displayString;
			m_textmesh->UpdateText();
		}
	}
	else
	{
		if ( m_scrambleUpdate )
		{
			// Update text
			m_textmesh->m_text = m_displayString;
			m_textmesh->UpdateText();
		}
	}

	// Lerp in the box
	Vector2f estimatedSize;
	estimatedSize.x = math::lerp( easing::back_out(m_fadeLerpX), 16, width + 14 );
	estimatedSize.y = math::lerp( easing::back_out(m_fadeLerpY), 16, 10 + m_textmesh->GetLineHeight() * (Real)math::round(0.5F + text.length() / 23.0F)  );

	// Update box size
	if (m_boxbackground)		m_boxbackground->GenerateMesh( estimatedSize );
	if (m_boxbackgroundblend)	m_boxbackgroundblend->GenerateMesh( estimatedSize - Vector2f(2,2) );

	// Update text position
	m_textmesh->transform.world.position	= Vector3d( position.x, position.y, -100 );

	// Update text alpha
	m_textmesh->GetMaterial()->m_diffuse.alpha = m_fadeLerpX;

	// Update background positions
	if (m_boxbackground)
	{
		m_boxbackground->transform.world.position	= Vector3d(
			position.x - 4.0F + (width + 14 - estimatedSize.x) * 0.5F,
			position.y - 15.0F,
			-99 );
	}
	if (m_boxbackgroundblend)
	{
		m_boxbackgroundblend->transform.world.position = Vector3d(
			position.x - 3.0F + (width + 14 - estimatedSize.x) * 0.5F,
			position.y - 14.0F,
			-98 );
	}
}


void TalkerBox::Show ( void )
{
	m_fadeLerp = 0.0F;
	m_fadeIn = true;
}
void TalkerBox::Hide ( void )
{
	m_fadeIn = false;
}

bool TalkerBox::IsHidden ( void )
{
	if (m_fadeIn == false && m_fadeLerp <= FTYPE_PRECISION)
	{
		return true;
	}
	return false;
}
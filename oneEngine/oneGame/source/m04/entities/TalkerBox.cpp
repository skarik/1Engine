#include <cctype>

#include "core/math/Rect.h"
#include "core/math/Math.h"
#include "core/math/Easing.h"
#include "core/math/random/Random.h"

#include "renderer/material/Material.h"
#include "renderer/types/fontEnums.h"
#include "render2d/object/CTextMesh.h"

#include "m04/entities/TalkerBox.h"

using namespace M04;

class TalkerBox::TalkerBoxBackground : public CRenderable2D
{
public:
	TalkerBoxBackground ( void )
		: CRenderable2D()
	{
		SetSpriteFile( "sprites/ui/ui_msgbox.gal", NULL );

		memset( &m_modeldata, 0, sizeof(arModelData) );
	}
	~TalkerBoxBackground ( void )
	{
		delete_safe_array(m_modeldata.position);
		delete_safe_array(m_modeldata.color);
		delete_safe_array(m_modeldata.texcoord0);
		delete_safe_array(m_modeldata.indices);
	}

	void GenerateMesh ( const Vector2f& size )
	{
		// Estimate needed amount of vertices for the text:

		if (m_modeldata.indices == NULL)
		{
			delete [] m_modeldata.position;
			delete [] m_modeldata.color;
			delete [] m_modeldata.texcoord0;
			delete [] m_modeldata.indices;

			m_modeldata.indexNum = 2 * 16 * 3;
			m_modeldata.vertexNum = 4 * 16;

			m_modeldata.indices = new uint16_t [m_modeldata.indexNum];
			m_modeldata.position = new Vector3f [m_modeldata.vertexNum];
			m_modeldata.color = new Vector4f [m_modeldata.vertexNum];
			m_modeldata.texcoord0 = new Vector3f [m_modeldata.vertexNum];
		}

		// Reset mesh data:

		for ( uint i = 0; i < m_modeldata.vertexNum; ++i )
		{
			m_modeldata.color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		}

		uint32_t current_vertex = 0;
		uint32_t current_index = 0;

#define PUSH_QUAD( rect_position, rect_uvs ) \
		m_modeldata.position[current_vertex + 0] = rect_position.pos; \
		m_modeldata.texcoord0[current_vertex + 0] = Vector2f(rect_uvs.pos.x / m_spriteInfo.fullsize.x, rect_uvs.pos.y / m_spriteInfo.fullsize.y); \
		m_modeldata.position[current_vertex + 1] = Vector2f(rect_position.pos.x, rect_position.pos.y + rect_position.size.y); \
		m_modeldata.texcoord0[current_vertex + 1] = Vector2f(rect_uvs.pos.x / m_spriteInfo.fullsize.x, (rect_uvs.pos.y + rect_uvs.size.y) / m_spriteInfo.fullsize.y); \
		m_modeldata.position[current_vertex + 2] = rect_position.pos + rect_position.size; \
		m_modeldata.texcoord0[current_vertex + 2] = Vector2f((rect_uvs.pos.x + rect_uvs.size.x) / m_spriteInfo.fullsize.x, (rect_uvs.pos.y + rect_uvs.size.y) / m_spriteInfo.fullsize.y); \
		m_modeldata.position[current_vertex + 3] = Vector2f(rect_position.pos.x + rect_position.size.x, rect_position.pos.y); \
		m_modeldata.texcoord0[current_vertex + 3] = Vector2f((rect_uvs.pos.x + rect_uvs.size.x) / m_spriteInfo.fullsize.x, rect_uvs.pos.y / m_spriteInfo.fullsize.y); \
		m_modeldata.indices[current_index + 0] = current_vertex + 0; \
		m_modeldata.indices[current_index + 1] = current_vertex + 1; \
		m_modeldata.indices[current_index + 2] = current_vertex + 2; \
		m_modeldata.indices[current_index + 3] = current_vertex + 0; \
		m_modeldata.indices[current_index + 4] = current_vertex + 2; \
		m_modeldata.indices[current_index + 5] = current_vertex + 3; \
		current_vertex += 4; \
		current_index += 6;

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
		SetSpriteFile( "textures/white.jpg", NULL );
		
		PassAccess(0).setHLBlendMode(renderer::kHLBlendModeMultiplyX2);

		memset( &m_modeldata, 0, sizeof(arModelData) );
	}
	~TalkerBoxBackgroundColor ( void )
	{
		delete_safe_array(m_modeldata.position);
		delete_safe_array(m_modeldata.color);
		delete_safe_array(m_modeldata.texcoord0);
		delete_safe_array(m_modeldata.indices);
	}

	void GenerateMesh ( const Vector2f& size )
	{
		// Estimate needed amount of vertices for the text:

		if (m_modeldata.indices == NULL)
		{
			delete [] m_modeldata.position;
			delete [] m_modeldata.color;
			delete [] m_modeldata.texcoord0;
			delete [] m_modeldata.indices;

			m_modeldata.indexNum = 2 * 3;
			m_modeldata.vertexNum = 4;

			m_modeldata.indices = new uint16_t [m_modeldata.indexNum];
			m_modeldata.position = new Vector3f [m_modeldata.vertexNum];
			m_modeldata.color = new Vector4f [m_modeldata.vertexNum];
			m_modeldata.texcoord0 = new Vector3f [m_modeldata.vertexNum];
		}

		// Reset mesh data:

		for ( uint i = 0; i < m_modeldata.vertexNum; ++i )
		{
			m_modeldata.color[i] = Vector4f(0.3F, 0.2F, 0.4F, 1.0F);
		}

		// Build the quad:
		m_modeldata.position[0].x = 0;
		m_modeldata.position[0].y = 0;

		m_modeldata.position[1].x = 0;
		m_modeldata.position[1].y = size.y;

		m_modeldata.position[2].x = size.x;
		m_modeldata.position[2].y = size.y;

		m_modeldata.position[3].x = size.x;
		m_modeldata.position[3].y = 0;

		// Create quad's triangles:
		m_modeldata.indices[0] = 0; 
		m_modeldata.indices[1] = 1; 
		m_modeldata.indices[2] = 2; 

		m_modeldata.indices[3] = 0;
		m_modeldata.indices[4] = 2; 
		m_modeldata.indices[5] = 3; 

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
	m_textmesh->SetFont("Aldrich-Regular.ttf", 12, kFW_Bold);
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
	size_t display_length_previous = m_displayString.length();

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
	estimatedSize.x = math::lerp<Real>( easing::back_out(m_fadeLerpX), 16, width + 14 );
	estimatedSize.y = math::lerp<Real>( easing::back_out(m_fadeLerpY), 16, 10 + m_textmesh->GetLineHeight() * (Real)math::round(0.5F + text.length() / 23.0F)  );

	// Update box size
	if (m_boxbackground)		m_boxbackground->GenerateMesh( estimatedSize );
	if (m_boxbackgroundblend)	m_boxbackgroundblend->GenerateMesh( estimatedSize - Vector2f(2,2) );

	// Update text position
	m_textmesh->transform.world.position	= Vector3f( position.x, position.y, -100 );

	// Update text alpha
	m_textmesh->PassGetSurface(0).diffuseColor.w = m_fadeLerpX;

	// Update background positions
	if (m_boxbackground)
	{
		m_boxbackground->transform.world.position	= Vector3f(
			position.x - 4.0F + (width + 14 - estimatedSize.x) * 0.5F,
			position.y - 15.0F,
			-99 );
	}
	if (m_boxbackgroundblend)
	{
		m_boxbackgroundblend->transform.world.position = Vector3f(
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
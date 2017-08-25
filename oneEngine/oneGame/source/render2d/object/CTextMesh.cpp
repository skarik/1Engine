#include "core/math/Rect.h"
#include "core/math/Math.h"
#include "core/math/Easing.h"

#include "render2d/object/CTextMesh.h"

#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/RrMaterial.h"

#include <cctype>

CTextMesh::CTextMesh ( void )
	: CRenderable2D(),
	m_text(""), m_max_width(-1),
	m_font_texture(NULL), m_text_triangle_count(0)
{
	// Clear out deferred info. We're not using that pass.
	m_material->deferredinfo.clear();
}

CTextMesh::~CTextMesh ( void )
{
	if ( m_font_texture )
	{
		m_font_texture->RemoveReference();
	}

	if ( m_modeldata.triangles != NULL )
	{
		delete [] m_modeldata.triangles;
	}
	if ( m_modeldata.vertices != NULL )
	{
		delete [] m_modeldata.vertices;
	}
}

//		SetFont ( ) 
// Loads a font as a texture for use with this text mesh
void CTextMesh::SetFont ( const char* fontFile, int fontSize, int fontWeight )
{
	m_font_texture = new CBitmapFont( fontFile, fontSize, fontWeight );
	m_font_texture->SetFilter( SamplingPoint );

	m_material->setTexture( TEX_DIFFUSE, m_font_texture );
}

//		GetLineHeight ( )
// Returns approximate line height based on letter M, plus 3 pixels
Real CTextMesh::GetLineHeight ( void )
{
	auto fontInfo	= m_font_texture->GetFontInfo();
	return fontInfo.fontSizes['M' - fontInfo.startCharacter].y + 3.0F;
}

//		UpdateText ( ) 
// Updates the mesh with the text in m_text
void CTextMesh::UpdateText ( void )
{
	// Estimate needed amount of vertices for the text:

	if (m_text_triangle_count < m_text.length() * 2)
	{
		delete [] m_modeldata.triangles;
		delete [] m_modeldata.vertices;

		m_text_triangle_count = m_text.length() * 2;
		m_modeldata.vertexNum = (uint16_t)(m_text.length() * 4);

		m_modeldata.triangles = new arModelTriangle [m_text_triangle_count];
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

	// Get the font info:

	auto fontInfo	= m_font_texture->GetFontInfo();
	Real baseScale	= (Real)m_font_texture->GetWidth();

	// Set up information for the text passes:

	Vector2d pen (0,0);
	const int maxLength = m_text.length();
	uint32_t vertex_index = 0;
	uint32_t triangle_index = 0;
	int c_lookup;
	// Always use 'M' as the base case font size, because it's huge
	Vector2d font_max_size = fontInfo.fontSizes['M' - fontInfo.startCharacter];	

	// Pass 1: Build the mesh:

	for ( int c = 0; c < maxLength; ++c )
	{
		// Get the current character offset into the existing character set
		c_lookup = m_text[c] - fontInfo.startCharacter;

		// Check that character is in set
		if ( c_lookup < 0 || c_lookup >= (int)fontInfo.setLength )
		{
			continue; // Skip quad
		}
		// Don't make quads for whitespace
		else if ( std::isspace(m_text[c]) )
		{
			// Move the pen along
			pen += fontInfo.fontOffsets[c_lookup];

			// Check for max width
			if ( m_max_width > 0 )
			{
				float pen_m_x = pen.x;
				int cf_lookup;

				// Check forward for the next letter
				for ( int cf = c + 1; cf < maxLength; ++cf )
				{
					// Get the current character offset into the existing character set
					cf_lookup = m_text[cf] - fontInfo.startCharacter;
					// Check that character is in set
					if ( cf_lookup < 0 || cf_lookup >= (int)fontInfo.setLength )
					{
						continue; // Skip check
					}
					// Not a space, draw a virtual letter
					else if ( !std::isspace(m_text[cf]) && cf != maxLength - 1 )
					{
						pen_m_x += fontInfo.fontOffsets[cf_lookup].x;
					}
					// If it's a space, check the current location of the virtual letter
					else 
					{
						if ( pen_m_x >= m_max_width )
						{
							// Go to next line
							pen.x = 0;
							pen.y += font_max_size.y + 3.0F;
						}
						break;
					}
				}
			}

			continue; // Skip quad
		}

		// Check for max width
		/*if ( m_max_width > 0 && pen.x + fontInfo.fontOffsets[c_lookup].x > m_max_width )
		{
			// Go to next line
			pen.x = 0;
			pen.y += font_max_size.y + 2.0F;
		}*/
		
		// Grab the UV rect of the character
		Rect uv ( fontInfo.charPositions[c_lookup] / baseScale, fontInfo.fontSizes[c_lookup] / baseScale );


		// Create the quad for the character:

		// Set up final drawing position
		Vector2d drawPos = pen - fontInfo.fontOrigins[c_lookup];
		drawPos.x = (Real)math::round( drawPos.x );
		drawPos.y = (Real)math::round( drawPos.y );

		// 0
		m_modeldata.vertices[vertex_index + 0].u = uv.pos.x;
		m_modeldata.vertices[vertex_index + 0].v = uv.pos.y;
		m_modeldata.vertices[vertex_index + 0].nx = -1;
		m_modeldata.vertices[vertex_index + 0].ny = -1;
		m_modeldata.vertices[vertex_index + 0].nz = uv.size.y / uv.size.x;
		m_modeldata.vertices[vertex_index + 0].x = drawPos.x;
		m_modeldata.vertices[vertex_index + 0].y = drawPos.y;
		// 1
		m_modeldata.vertices[vertex_index + 1].u = uv.pos.x;
		m_modeldata.vertices[vertex_index + 1].v = uv.pos.y + uv.size.y;
		m_modeldata.vertices[vertex_index + 1].nx = -1;
		m_modeldata.vertices[vertex_index + 1].ny = 1;
		m_modeldata.vertices[vertex_index + 1].nz = uv.size.y / uv.size.x;
		m_modeldata.vertices[vertex_index + 1].x = drawPos.x;
		m_modeldata.vertices[vertex_index + 1].y = drawPos.y + uv.size.y * baseScale;
		// 2
		m_modeldata.vertices[vertex_index + 2].u = uv.pos.x + uv.size.x;
		m_modeldata.vertices[vertex_index + 2].v = uv.pos.y + uv.size.y;
		m_modeldata.vertices[vertex_index + 2].nx = 1;
		m_modeldata.vertices[vertex_index + 2].ny = 1;
		m_modeldata.vertices[vertex_index + 2].nz = uv.size.y / uv.size.x;
		m_modeldata.vertices[vertex_index + 2].x = drawPos.x + uv.size.x * baseScale;
		m_modeldata.vertices[vertex_index + 2].y = drawPos.y + uv.size.y * baseScale;
		// 3
		m_modeldata.vertices[vertex_index + 3].u = uv.pos.x + uv.size.x;
		m_modeldata.vertices[vertex_index + 3].v = uv.pos.y;
		m_modeldata.vertices[vertex_index + 3].nx = 1;
		m_modeldata.vertices[vertex_index + 3].ny = -1;
		m_modeldata.vertices[vertex_index + 3].nz = uv.size.y / uv.size.x;
		m_modeldata.vertices[vertex_index + 3].x = drawPos.x + uv.size.x * baseScale;
		m_modeldata.vertices[vertex_index + 3].y = drawPos.y;

		// Triangle
		m_modeldata.triangles[triangle_index + 0].vert[0] = vertex_index + 0;
		m_modeldata.triangles[triangle_index + 0].vert[1] = vertex_index + 1;
		m_modeldata.triangles[triangle_index + 0].vert[2] = vertex_index + 2;
		m_modeldata.triangles[triangle_index + 1].vert[0] = vertex_index + 0;
		m_modeldata.triangles[triangle_index + 1].vert[1] = vertex_index + 2;
		m_modeldata.triangles[triangle_index + 1].vert[2] = vertex_index + 3;

		// Move the array indices along
		vertex_index += 4;
		triangle_index += 2;

		// Move the pen along
		pen += fontInfo.fontOffsets[c_lookup];
	}
	
	// Update triangle count
	m_modeldata.triangleNum = triangle_index;


	// Now with the mesh built, push it to the modeldata :)
	PushModeldata();
}
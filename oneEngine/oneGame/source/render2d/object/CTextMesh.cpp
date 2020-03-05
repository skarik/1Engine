#include "core/math/Rect.h"
#include "core/math/Math.h"
#include "core/math/Easing.h"

#include "render2d/object/CTextMesh.h"

#include "renderer/texture/RrFontTexture.h"

#include <cctype>

CTextMesh::CTextMesh ( void )
	: CRenderable2D(),
	m_text(""), m_max_width(-1),
	m_font_texture(NULL), m_text_glyph_count(0)
{
	// A forward-rendered fullbright pass is in alpha-test mode is set by CRenderable2D().
}

CTextMesh::~CTextMesh ( void )
{
	if ( m_font_texture )
	{
		m_font_texture->RemoveReference();
	}

	delete_safe_array(m_modeldata.indices);
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.normal);
}

//		SetFont ( ) 
// Loads a font as a texture for use with this text mesh
void CTextMesh::SetFont ( const char* fontFile, int fontSize, int fontWeight )
{
	// Grab the texture since we need the extra information to make the mesh
	m_font_texture = RrFontTexture::Load( fontFile, fontSize, fontWeight );
	//m_font_texture->SetFilter( SamplingPoint );

	// Edit the current pass with the new texture
	PassAccess(0).setTexture( TEX_DIFFUSE, m_font_texture );
}

//		GetLineHeight ( )
// Returns approximate line height based on letter M, plus 3 pixels
Real CTextMesh::GetLineHeight ( void )
{
	auto fontInfo	= m_font_texture->GetFontInfo();
	return fontInfo->glyphSize['M' - m_font_texture->GetFontGlyphStart()].y + 3.0F;
}

//		UpdateText ( ) 
// Updates the mesh with the text in m_text
void CTextMesh::UpdateText ( void )
{
	// a lot of this is copy-pasted from rrTextBuilder2D

	// Estimate needed amount of vertices for the text:

	//if (m_text_triangle_count < m_text.length() * 2)
	if (m_text_glyph_count < m_text.length())
	{
		delete_safe_array(m_modeldata.indices);
		delete_safe_array(m_modeldata.position);
		delete_safe_array(m_modeldata.color);
		delete_safe_array(m_modeldata.texcoord0);
		delete_safe_array(m_modeldata.normal);

		m_text_glyph_count = m_text.length();

		m_modeldata.indices = new uint16_t [m_text_glyph_count * 5]; // 5 indices per character
		m_modeldata.position = new Vector3f [m_text_glyph_count * 4];
		m_modeldata.color = new Vector4f [m_text_glyph_count * 4];
		m_modeldata.texcoord0 = new Vector3f [m_text_glyph_count * 4];
		m_modeldata.normal = new Vector3f [m_text_glyph_count * 4];
	}

	// Reset mesh data:

	for ( uint i = 0; i < m_modeldata.vertexNum; ++i )
	{
		m_modeldata.color[i] = {1.0F, 1.0F, 1.0F, 1.0F};
	}

	// Get the font info:

	auto fontInfo		= m_font_texture->GetFontInfo();
	auto fontStartGlyph	= m_font_texture->GetFontGlyphStart();
	auto fontGlyphCount	= m_font_texture->GetFontGlyphCount();
	Real baseScale		= (Real)m_font_texture->GetWidth();

	// Set up information for the text passes:

	Vector2f pen (0,0);
	const size_t maxLength = m_text.length();
	uint32_t vertex_index = 0;
	uint32_t index_index = 0;
	int c_lookup;
	// Always use 'M' as the base case font size, because it's huge
	Vector2i font_max_size = fontInfo->glyphSize['M' - fontStartGlyph];	

	// Pass 1: Build the mesh:

	for ( int c = 0; c < maxLength; ++c )
	{
		// Get the current character offset into the existing character set
		c_lookup = m_text[c] - fontStartGlyph;

		// Check that character is in set
		if ( c_lookup < 0 || c_lookup >= (int)fontGlyphCount )
		{
			continue; // Skip quad
		}
		// Don't make quads for whitespace
		else if ( std::isspace(m_text[c]) )
		{
			// Move the pen along
			pen.x += fontInfo->glyphAdvance[c_lookup].x;
			pen.y += fontInfo->glyphAdvance[c_lookup].y;

			// Check for max width
			if ( m_max_width > 0 )
			{
				float pen_m_x = pen.x;
				int cf_lookup;

				// Check forward for the next letter
				for ( int cf = c + 1; cf < maxLength; ++cf )
				{
					// Get the current character offset into the existing character set
					cf_lookup = m_text[cf] - m_font_texture->GetFontGlyphStart();
					// Check that character is in set
					if ( cf_lookup < 0 || cf_lookup >= (int)m_font_texture->GetFontGlyphCount() )
					{
						continue; // Skip check
					}
					// Not a space, draw a virtual letter
					else if ( !std::isspace(m_text[cf]) && cf != maxLength - 1 )
					{
						pen_m_x += fontInfo->glyphAdvance[cf_lookup].x;
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
		Rect uv ( Vector2f( fontInfo->glyphTexelPosition[c_lookup].x / baseScale,
							fontInfo->glyphTexelPosition[c_lookup].y / baseScale),
				  Vector2f( fontInfo->glyphSize[c_lookup].x / baseScale,
							fontInfo->glyphSize[c_lookup].y / baseScale ) );


		// Create the quad for the character:

		// Set up final drawing position
		Vector2f drawPos = pen - Vector2f(fontInfo->glyphOrigin[c_lookup].x / baseScale, fontInfo->glyphOrigin[c_lookup].y / baseScale);
		drawPos.x = (Real)math::round( drawPos.x );
		drawPos.y = (Real)math::round( drawPos.y );

		// 0
		m_modeldata.texcoord0[vertex_index + 0] = uv.pos;
		m_modeldata.normal[vertex_index + 0] = Vector3f(-1.0F, -1.0F, uv.size.y / uv.size.x);
		m_modeldata.position[vertex_index + 0] = drawPos;
		// 1
		m_modeldata.texcoord0[vertex_index + 1] = uv.pos + Vector2f(0.0F, uv.size.y);
		m_modeldata.normal[vertex_index + 1] = Vector3f(-1.0F, +1.0F, uv.size.y / uv.size.x);
		m_modeldata.position[vertex_index + 1] = drawPos + Vector2f(0.0F, uv.size.y * baseScale);
		// 2
		m_modeldata.texcoord0[vertex_index + 2] = uv.pos + uv.size;
		m_modeldata.normal[vertex_index + 2] = Vector3f(+1.0F, +1.0F, uv.size.y / uv.size.x);
		m_modeldata.position[vertex_index + 2] = drawPos + uv.size * baseScale;
		// 3
		m_modeldata.texcoord0[vertex_index + 3] = uv.pos + Vector2f(uv.size.x, 0.0F);
		m_modeldata.normal[vertex_index + 3] = Vector3f(+1.0F, -1.0F, uv.size.y / uv.size.x);
		m_modeldata.position[vertex_index + 3] = drawPos + Vector2f(uv.size.x * baseScale, 0.0F);

		// Quad
		m_modeldata.indices[index_index + 0] = vertex_index + 0;
		m_modeldata.indices[index_index + 1] = vertex_index + 3;
		m_modeldata.indices[index_index + 2] = vertex_index + 1;
		m_modeldata.indices[index_index + 3] = vertex_index + 2;
		m_modeldata.indices[index_index + 4] = 0xFFFF;

		// Move the array indices along
		vertex_index += 4;
		index_index += 5;

		// Move the pen along
		pen += Vector2f(fontInfo->glyphAdvance[c_lookup].x / baseScale, fontInfo->glyphAdvance[c_lookup].y / baseScale);
	}
	
	// Update triangle count
	m_modeldata.vertexNum = vertex_index;
	m_modeldata.indexNum = index_index;


	// Now with the mesh built, push it to the modeldata :)
	PushModeldata();
}
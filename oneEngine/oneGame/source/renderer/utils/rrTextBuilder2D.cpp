#include "rrTextBuilder2D.h"

#include "core/math/Math.h"
#include "core/system/Screen.h"
#include "renderer/texture/RrFontTexture.h"
#include <cstring>
#include <cctype>

//	Constructor (new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrTextBuilder2D::rrTextBuilder2D ( RrFontTexture* font, const uint16_t estimatedVertexCount )
	: rrMeshBuilder2D(estimatedVertexCount), m_font_texture(font)
{
}
//	Constructor (existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrTextBuilder2D::rrTextBuilder2D ( RrFontTexture* font, arModelData* preallocatedModelData )
	: rrMeshBuilder2D(preallocatedModelData), m_font_texture(font)
{
}
//	Constructor (cubic, new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
rrTextBuilder2D::rrTextBuilder2D ( RrFontTexture* font, const core::math::Cubic& screenMapping, const uint16_t estimatedVertexCount )
	: rrMeshBuilder2D(screenMapping, estimatedVertexCount), m_font_texture(font)
{
}
//	Constructor (cubic, existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
rrTextBuilder2D::rrTextBuilder2D ( RrFontTexture* font, const core::math::Cubic& screenMapping, arModelData* preallocatedModelData )
	: rrMeshBuilder2D(screenMapping, preallocatedModelData), m_font_texture(font)
{
}

static Vector2d _transformVertexPosition ( const Vector2d& position, const Vector2d& multiplier, const Vector2d& offset)
{
	return position.mulComponents(multiplier) + offset;
}

//	addText ( position, color, str ) : Adds text to draw.
void rrTextBuilder2D::addText ( const Vector2d& position, const Color& color, const char* str )
{
	// Estimate needed amount of vertices for the text:

	Real m_max_width = 10000;

	const int maxLength = (int)strlen(str);

	expand(m_vertexCount + maxLength * 4);
	expandIndices(m_indexCount + maxLength * 5);

	// Get the font info:

	auto fontInfo		= m_font_texture->GetFontInfo();
	auto fontStartGlyph	= m_font_texture->GetFontGlyphStart();
	auto fontGlyphCount	= m_font_texture->GetFontGlyphCount();
	Real baseScale		= (Real)m_font_texture->GetWidth();

	// Set up information for the text passes:

	Vector2d pen (0,0);
	uint16_t vert_index = m_vertexCount;
	uint16_t inde_index = m_indexCount;
	int c_lookup;
	// Always use 'M' as the base case font size, because it's huge
	Vector2i font_max_size = fontInfo->glyphSize['M' - fontStartGlyph];	

	// Pass 1: Build the mesh:

	for ( int c = 0; c < maxLength; ++c )
	{
		// Get the current character offset into the existing character set
		c_lookup = str[c] - fontStartGlyph;

		// Check that character is in set
		if ( c_lookup < 0 || c_lookup >= (int)fontGlyphCount )
		{
			continue; // Skip quad
		}
		// Don't make quads for whitespace
		else if ( std::isspace(str[c]) )
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
					cf_lookup = str[cf] - fontStartGlyph;
					// Check that character is in set
					if ( cf_lookup < 0 || cf_lookup >= (int)fontGlyphCount )
					{
						continue; // Skip check
					}
					// Not a space, draw a virtual letter
					else if ( !std::isspace(str[cf]) && cf != maxLength - 1 )
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
		Rect uv ( fontInfo->glyphTexelPosition[c_lookup] / baseScale, fontInfo.fontSizes[c_lookup] / baseScale );

		
		// Create the quad for the character:

		// Set up final drawing position
		Vector2d drawPos = pen - (Vector2f)fontInfo->glyphOrigin[c_lookup];
		drawPos.x = (Real)math::round( drawPos.x );
		drawPos.y = (Real)math::round( drawPos.y );

		//memset(m_model->vertices + vertex_index, 0, sizeof(arModelVertex) * 4);

		m_model->position[vert_index + 0] = _transformVertexPosition(position + drawPos, m_multiplier, m_offset);
		m_model->position[vert_index + 1] = _transformVertexPosition(position + Vector2f(drawPos.x, drawPos.y + uv.size.y * baseScale), m_multiplier, m_offset);
		m_model->position[vert_index + 2] = _transformVertexPosition(position + Vector2f(drawPos.x + uv.size.x * baseScale, drawPos.y + uv.size.y * baseScale), m_multiplier, m_offset);
		m_model->position[vert_index + 3] = _transformVertexPosition(position + Vector2f(drawPos.x + uv.size.x * baseScale, drawPos.y), m_multiplier, m_offset);

		m_model->texcoord0[vert_index + 0] = uv.pos;
		m_model->texcoord0[vert_index + 1] = Vector2f(uv.pos.x, uv.pos.y + uv.size.y);
		m_model->texcoord0[vert_index + 2] = Vector2f(uv.pos.x + uv.size.x, uv.pos.y + uv.size.y);
		m_model->texcoord0[vert_index + 3] = Vector2f(uv.pos.x + uv.size.x, uv.pos.y);

		m_model->normal[vert_index + 0] = Vector3f(-1, -1, uv.size.y / uv.size.x);
		m_model->normal[vert_index + 1] = Vector3f(-1, +1, uv.size.y / uv.size.x);
		m_model->normal[vert_index + 2] = Vector3f(+1, +1, uv.size.y / uv.size.x);
		m_model->normal[vert_index + 3] = Vector3f(+1, -1, uv.size.y / uv.size.x);

		m_model->color[vert_index + 0] = Vector4f(&color.x);
		m_model->color[vert_index + 1] = Vector4f(&color.x);
		m_model->color[vert_index + 2] = Vector4f(&color.x);
		m_model->color[vert_index + 3] = Vector4f(&color.x);

		m_model->position[vert_index + 0].z = 0.5F;
		m_model->position[vert_index + 1].z = 0.5F;
		m_model->position[vert_index + 2].z = 0.5F;
		m_model->position[vert_index + 3].z = 0.5F;

		/*m_model->triangles[triangle_index + 0].vert[0] = vertex_index + 0;
		m_model->triangles[triangle_index + 0].vert[1] = vertex_index + 1;
		m_model->triangles[triangle_index + 0].vert[2] = vertex_index + 2;

		m_model->triangles[triangle_index + 1].vert[0] = vertex_index + 0;
		m_model->triangles[triangle_index + 1].vert[1] = vertex_index + 2;
		m_model->triangles[triangle_index + 1].vert[2] = vertex_index + 3;*/
		m_model->indices[inde_index + 0] = vert_index + 0;
		m_model->indices[inde_index + 1] = vert_index + 1;
		m_model->indices[inde_index + 2] = vert_index + 2;
		m_model->indices[inde_index + 3] = vert_index + 3;
		m_model->indices[inde_index + 4] = 0xFFFF;

		// Move the array indices along
		vert_index += 4;
		inde_index += 5;

		// Move the pen along
		pen.x += fontInfo->glyphAdvance[c_lookup].x;
		pen.y += fontInfo->glyphAdvance[c_lookup].y;
	}

	// Update vertex and triangle count
	m_vertexCount = vert_index;
	m_indexCount  = inde_index;
}

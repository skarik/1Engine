#include "rrTextBuilder2D.h"

#include "core/math/Math.h"
#include "core/system/Screen.h"
#include "renderer/texture/CBitmapFont.h"
#include <cstring>
#include <cctype>

//	Constructor (new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrTextBuilder2D::rrTextBuilder2D ( CBitmapFont* font, const uint16_t estimatedVertexCount )
	: IrrMeshBuilder(estimatedVertexCount), m_font_texture(font)
{
	setScreenMapping(
		core::math::Cubic::FromPosition(
			Vector3d(0, 0, -45.0F),
			Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) )
	);
}
//	Constructor (existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrTextBuilder2D::rrTextBuilder2D ( CBitmapFont* font, arModelData* preallocatedModelData )
	: IrrMeshBuilder(preallocatedModelData), m_font_texture(font)
{
	setScreenMapping(
		core::math::Cubic::FromPosition(
			Vector3d(0, 0, -45.0F),
			Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) )
	);
}
//	Constructor (cubic, new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
rrTextBuilder2D::rrTextBuilder2D ( CBitmapFont* font, const core::math::Cubic& screenMapping, const uint16_t estimatedVertexCount )
	: IrrMeshBuilder(estimatedVertexCount), m_font_texture(font)
{
	setScreenMapping(screenMapping);
}
//	Constructor (cubic, existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
rrTextBuilder2D::rrTextBuilder2D ( CBitmapFont* font, const core::math::Cubic& screenMapping, arModelData* preallocatedModelData )
	: IrrMeshBuilder(preallocatedModelData), m_font_texture(font)
{
	setScreenMapping(screenMapping);
}

//	setScreenMapping (cubic) : Sets the mapping of the Rect coordinates to the screen.
void rrTextBuilder2D::setScreenMapping ( const core::math::Cubic& screenMapping )
{
	m_multiplier.x = 2.0F / screenMapping.size.x;
	m_multiplier.y = 2.0F / screenMapping.size.y;

	m_offset.x = -screenMapping.position.x * m_multiplier.x - 1.0F;
	m_offset.y = -screenMapping.position.y * m_multiplier.y - 1.0F;

	// Then flip Y for OpenGL coordinates.
	m_multiplier.y	= -m_multiplier.y;
	m_offset.y		= -m_offset.y;
}


static Vector2d _transformVertexPosition ( const Vector2d& position, const Vector2d& multiplier, const Vector2d& offset)
{
	return position.mulComponents(multiplier) + offset;
}

void rrTextBuilder2D::addText ( const Vector2d& position, const Color& color, const char* str )
{
	// Estimate needed amount of vertices for the text:

	Real m_max_width = 10000;

	const int maxLength = (int)strlen(str);

	expand(m_vertexCount + maxLength * 4);
	expandTri(m_triangleCount + maxLength * 2);

	// Get the font info:

	auto fontInfo	= m_font_texture->GetFontInfo();
	Real baseScale	= (Real)m_font_texture->GetWidth();

	// Set up information for the text passes:

	Vector2d pen (0,0);
	uint16_t vertex_index = m_vertexCount;
	uint16_t triangle_index = m_triangleCount;
	int c_lookup;
	// Always use 'M' as the base case font size, because it's huge
	Vector2d font_max_size = fontInfo.fontSizes['M' - fontInfo.startCharacter];	

	// Pass 1: Build the mesh:

	for ( int c = 0; c < maxLength; ++c )
	{
		// Get the current character offset into the existing character set
		c_lookup = str[c] - fontInfo.startCharacter;

		// Check that character is in set
		if ( c_lookup < 0 || c_lookup >= (int)fontInfo.setLength )
		{
			continue; // Skip quad
		}
		// Don't make quads for whitespace
		else if ( std::isspace(str[c]) )
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
					cf_lookup = str[cf] - fontInfo.startCharacter;
					// Check that character is in set
					if ( cf_lookup < 0 || cf_lookup >= (int)fontInfo.setLength )
					{
						continue; // Skip check
					}
					// Not a space, draw a virtual letter
					else if ( !std::isspace(str[cf]) && cf != maxLength - 1 )
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

		memset(m_model->vertices + vertex_index, 0, sizeof(arModelVertex) * 4);

		m_model->vertices[vertex_index + 0].position = _transformVertexPosition(position + drawPos, m_multiplier, m_offset);
		m_model->vertices[vertex_index + 1].position = _transformVertexPosition(position + Vector2f(drawPos.x, drawPos.y + uv.size.y * baseScale), m_multiplier, m_offset);
		m_model->vertices[vertex_index + 2].position = _transformVertexPosition(position + Vector2f(drawPos.x + uv.size.x * baseScale, drawPos.y + uv.size.y * baseScale), m_multiplier, m_offset);
		m_model->vertices[vertex_index + 3].position = _transformVertexPosition(position + Vector2f(drawPos.x + uv.size.x * baseScale, drawPos.y), m_multiplier, m_offset);

		m_model->vertices[vertex_index + 0].texcoord0 = uv.pos;
		m_model->vertices[vertex_index + 1].texcoord0 = Vector2f(uv.pos.x, uv.pos.y + uv.size.y);
		m_model->vertices[vertex_index + 2].texcoord0 = Vector2f(uv.pos.x + uv.size.x, uv.pos.y + uv.size.y);
		m_model->vertices[vertex_index + 3].texcoord0 = Vector2f(uv.pos.x + uv.size.x, uv.pos.y);

		m_model->vertices[vertex_index + 0].normal = Vector3f(-1, -1, uv.size.y / uv.size.x);
		m_model->vertices[vertex_index + 1].normal = Vector3f(-1, +1, uv.size.y / uv.size.x);
		m_model->vertices[vertex_index + 2].normal = Vector3f(+1, +1, uv.size.y / uv.size.x);
		m_model->vertices[vertex_index + 3].normal = Vector3f(+1, -1, uv.size.y / uv.size.x);

		m_model->vertices[vertex_index + 0].color = Vector4f(&color.x);
		m_model->vertices[vertex_index + 1].color = Vector4f(&color.x);
		m_model->vertices[vertex_index + 2].color = Vector4f(&color.x);
		m_model->vertices[vertex_index + 3].color = Vector4f(&color.x);

		m_model->vertices[vertex_index + 0].position.z = 0.5F;
		m_model->vertices[vertex_index + 1].position.z = 0.5F;
		m_model->vertices[vertex_index + 2].position.z = 0.5F;
		m_model->vertices[vertex_index + 3].position.z = 0.5F;

		m_model->triangles[triangle_index + 0].vert[0] = vertex_index + 0;
		m_model->triangles[triangle_index + 0].vert[1] = vertex_index + 1;
		m_model->triangles[triangle_index + 0].vert[2] = vertex_index + 2;

		m_model->triangles[triangle_index + 1].vert[0] = vertex_index + 0;
		m_model->triangles[triangle_index + 1].vert[1] = vertex_index + 2;
		m_model->triangles[triangle_index + 1].vert[2] = vertex_index + 3;

		// Move the array indices along
		vertex_index += 4;
		triangle_index += 2;

		// Move the pen along
		pen += fontInfo.fontOffsets[c_lookup];
	}

	// Update vertex and triangle count
	m_vertexCount = vertex_index;
	m_triangleCount = triangle_index;
}

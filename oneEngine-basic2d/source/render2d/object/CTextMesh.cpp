
#include "render2d/object/CTextMesh.h"

#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"


CTextMesh::CTextMesh ( void )
	: CRenderable2D(), m_font_texture(NULL)
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

	m_material->setTexture( 0, m_font_texture );
}

//		UpdateText ( ) 
// Updates the mesh with the text in m_text
void CTextMesh::UpdateText ( void )
{
	// Estimate needed amount of vertices for the text
	if (m_modeldata.triangleNum < m_text.length() * 2)
	{
		delete [] m_modeldata.triangles;
		delete [] m_modeldata.vertices;

		m_modeldata.triangleNum = m_text.length() * 2;
		m_modeldata.vertexNum = m_text.length() * 4;

		m_modeldata.triangles = new CModelTriangle [m_modeldata.triangleNum];
		m_modeldata.vertices = new CModelVertex [m_modeldata.vertexNum];
	}

	// Reset mesh data
	memset(m_modeldata.vertices, 0, sizeof(CModelVertex) * m_modeldata.vertexNum);
	for ( uint i = 0; i < m_modeldata.vertexNum; ++i )
	{
		m_modeldata.vertices[i].r = 1.0F;
		m_modeldata.vertices[i].g = 1.0F;
		m_modeldata.vertices[i].b = 1.0F;
		m_modeldata.vertices[i].a = 1.0F;
		m_modeldata.vertices[i].z = -10.0F;
	}

	// Get the font info
	tBitmapFontInfo fontInfo = m_font_texture->GetFontInfo();
	Real baseScale			 = (Real)m_font_texture->GetWidth();

	// Build the mesh
	Vector2d pen (0,0);
	const int maxLength = m_text.length();
	uint32_t vertex_index = 0;
	uint32_t triangle_index = 0;

	for ( int c = 0; c < maxLength; ++c )
	{
		int curChar = m_text[c] - fontInfo.startCharacter;

		// Check that character is in set
		if ( curChar < 0 || curChar >= (int)fontInfo.setLength )
		{
			continue;
		}

		Vector2d uvPos = (fontInfo.charPositions[curChar]) / baseScale;
		Vector2d uvSize = (fontInfo.fontSizes[curChar]) / baseScale;

		Vector2d drawPos = pen - fontInfo.fontOrigins[curChar];

		// 0
		m_modeldata.vertices[vertex_index + 0].u = uvPos.x;
		m_modeldata.vertices[vertex_index + 0].v = uvPos.y;
		m_modeldata.vertices[vertex_index + 0].nx = -1;
		m_modeldata.vertices[vertex_index + 0].ny = -1;
		m_modeldata.vertices[vertex_index + 0].nz = uvSize.y/uvSize.x;
		m_modeldata.vertices[vertex_index + 0].x = drawPos.x;
		m_modeldata.vertices[vertex_index + 0].y = drawPos.y;
		// 1
		m_modeldata.vertices[vertex_index + 1].u = uvPos.x;
		m_modeldata.vertices[vertex_index + 1].v = uvPos.y + uvSize.y;
		m_modeldata.vertices[vertex_index + 1].nx = -1;
		m_modeldata.vertices[vertex_index + 1].ny = 1;
		m_modeldata.vertices[vertex_index + 1].nz = uvSize.y/uvSize.x;
		m_modeldata.vertices[vertex_index + 1].x = drawPos.x;
		m_modeldata.vertices[vertex_index + 1].y = drawPos.y + uvSize.y*baseScale;
		// 2
		m_modeldata.vertices[vertex_index + 2].u = uvPos.x + uvSize.x;
		m_modeldata.vertices[vertex_index + 2].v = uvPos.y + uvSize.y;
		m_modeldata.vertices[vertex_index + 2].nx = 1;
		m_modeldata.vertices[vertex_index + 2].ny = 1;
		m_modeldata.vertices[vertex_index + 2].nz = uvSize.y/uvSize.x;
		m_modeldata.vertices[vertex_index + 2].x = drawPos.x + uvSize.x*baseScale;
		m_modeldata.vertices[vertex_index + 2].y = drawPos.y + uvSize.y*baseScale;
		// 3
		m_modeldata.vertices[vertex_index + 3].u = uvPos.x + uvSize.x;
		m_modeldata.vertices[vertex_index + 3].v = uvPos.y;
		m_modeldata.vertices[vertex_index + 3].nx = 1;
		m_modeldata.vertices[vertex_index + 3].ny = -1;
		m_modeldata.vertices[vertex_index + 3].nz = uvSize.y/uvSize.x;
		m_modeldata.vertices[vertex_index + 3].x = drawPos.x + uvSize.x*baseScale;
		m_modeldata.vertices[vertex_index + 3].y = drawPos.y;

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
		pen += fontInfo.fontOffsets[curChar];
	}


	// Now with the mesh built, push it to the modeldata :)
	PushModeldata();
}
#include "rrTextBuilder2D.h"

#include "core/system/Screen.h"

//	Constructor (new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrTextBuilder2D::rrTextBuilder2D ( CBitmapFont* font, const uint16_t estimatedVertexCount )
	: IrrMeshBuilder(estimatedVertexCount)
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
	: IrrMeshBuilder(preallocatedModelData)
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
	: IrrMeshBuilder(estimatedVertexCount)
{
	setScreenMapping(screenMapping);
}
//	Constructor (cubic, existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
rrTextBuilder2D::rrTextBuilder2D ( CBitmapFont* font, const core::math::Cubic& screenMapping, arModelData* preallocatedModelData )
	: IrrMeshBuilder(preallocatedModelData)
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

void rrTextBuilder2D::addText ( const Vector2d& position, const Color& color, const char* str )
{
	// Estimate needed amount of vertices for the text:

	/*if (m_text_triangle_count < strlen(str) * 2)
	{
		delete [] m_modeldata.triangles;
		delete [] m_modeldata.vertices;

		m_text_triangle_count = m_text.length() * 2;
		m_modeldata.vertexNum = (uint16_t)(m_text.length() * 4);

		m_modeldata.triangles = new arModelTriangle [m_text_triangle_count];
		m_modeldata.vertices = new arModelVertex [m_modeldata.vertexNum];
	}*/
}

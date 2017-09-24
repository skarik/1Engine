#include "rrMeshBuilder2D.h"

#include "core/system/Screen.h"

//	Constructor (new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrMeshBuilder2D::rrMeshBuilder2D ( const uint16_t estimatedVertexCount )
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
rrMeshBuilder2D::rrMeshBuilder2D ( arModelData* preallocatedModelData )
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
rrMeshBuilder2D::rrMeshBuilder2D ( const core::math::Cubic& screenMapping, const uint16_t estimatedVertexCount )
	: IrrMeshBuilder(estimatedVertexCount)
{
	setScreenMapping(screenMapping);
}
//	Constructor (cubic, existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
rrMeshBuilder2D::rrMeshBuilder2D ( const core::math::Cubic& screenMapping, arModelData* preallocatedModelData )
	: IrrMeshBuilder(preallocatedModelData)
{
	setScreenMapping(screenMapping);
}

//	setScreenMapping (cubic) : Sets the mapping of the Rect coordinates to the screen.
void rrMeshBuilder2D::setScreenMapping ( const core::math::Cubic& screenMapping )
{
	m_multiplier.x = 2.0F / screenMapping.size.x;
	m_multiplier.y = 2.0F / screenMapping.size.y;

	m_offset.x = -screenMapping.position.x * m_multiplier.x - 1.0F;
	m_offset.y = -screenMapping.position.y * m_multiplier.y - 1.0F;

	// Then flip Y for OpenGL coordinates.
	m_multiplier.y	= -m_multiplier.y;
	m_offset.y		= -m_offset.y;
}

//	addRect (rect, color, outline) : Adds a rectangle to draw.
// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel.
void rrMeshBuilder2D::addRect ( const Rect& rect, const Color& color, bool outline )
{
	const Vector2d pos_min = rect.pos.mulComponents(m_multiplier) + m_offset;
	const Vector2d pos_max = (rect.pos + rect.size).mulComponents(m_multiplier) + m_offset;

	if (!outline)
	{
		expand(m_vertexCount + 6);
		const uint16_t index = m_vertexCount;

		memset(m_model->vertices + index, 0, sizeof(arModelVertex) * 6);

		// Add the quad:

		m_model->vertices[index + 0].position = pos_min;
		m_model->vertices[index + 1].position = Vector2f(pos_min.x, pos_max.y);
		m_model->vertices[index + 2].position = Vector2f(pos_max.x, pos_min.y);
		m_model->vertices[index + 3].position = Vector2f(pos_max.x, pos_min.y);
		m_model->vertices[index + 4].position = Vector2f(pos_min.x, pos_max.y);
		m_model->vertices[index + 5].position = pos_max;

		m_model->vertices[index + 0].texcoord0 = Vector2f(0,0);
		m_model->vertices[index + 1].texcoord0 = Vector2f(0,1);
		m_model->vertices[index + 2].texcoord0 = Vector2f(1,0);
		m_model->vertices[index + 3].texcoord0 = Vector2f(1,0);
		m_model->vertices[index + 4].texcoord0 = Vector2f(0,1);
		m_model->vertices[index + 5].texcoord0 = Vector2f(1,1);

		m_model->vertices[index + 0].color = Vector4f(&color.x);
		m_model->vertices[index + 1].color = Vector4f(&color.x);
		m_model->vertices[index + 2].color = Vector4f(&color.x);
		m_model->vertices[index + 3].color = Vector4f(&color.x);
		m_model->vertices[index + 4].color = Vector4f(&color.x);
		m_model->vertices[index + 5].color = Vector4f(&color.x);

		m_model->vertices[index + 0].position.z = 0.5F;
		m_model->vertices[index + 1].position.z = 0.5F;
		m_model->vertices[index + 2].position.z = 0.5F;
		m_model->vertices[index + 3].position.z = 0.5F;
		m_model->vertices[index + 4].position.z = 0.5F;
		m_model->vertices[index + 5].position.z = 0.5F;

		m_vertexCount += 6;
	}
	else
	{
		const Vector2d pixelSize ( 1.0F/(Real)Screen::Info.width, 1.0F/(Real)Screen::Info.height );

		// Perform 4 addRect's for now
		addRect(rect, color, false);
		
		// UVs are separate so actually cannot do 4 addrects :)
	}
}
#include "rrMeshBuilder.h"

#include <cmath>
#include "core/math/Math.h"

//	Constructor (new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrMeshBuilder::rrMeshBuilder ( const uint16_t estimatedVertexCount )
	: IrrMeshBuilder(estimatedVertexCount)
{
	enableAttribute(renderer::shader::kVBufferSlotPosition);
	enableAttribute(renderer::shader::kVBufferSlotUV0);
	enableAttribute(renderer::shader::kVBufferSlotColor);
}
//	Constructor (existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
rrMeshBuilder::rrMeshBuilder ( arModelData* preallocatedModelData, uint16_t initialVertexCount, uint16_t initialIndexCount )
	: IrrMeshBuilder(preallocatedModelData, initialVertexCount, initialIndexCount)
{
	enableAttribute(renderer::shader::kVBufferSlotPosition);
	enableAttribute(renderer::shader::kVBufferSlotUV0);
	enableAttribute(renderer::shader::kVBufferSlotColor);
}

//	addQuad (points[4], normal, color) : Adds a quad to draw.
// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel.
void rrMeshBuilder::addQuad ( const Vector3f points [4], const Vector3f normal, const Color& color )
{
	expand(m_vertexCount + 4);
	expandIndices(m_indexCount + 5);

	const uint16_t vert_index = m_vertexCount;
	const uint16_t inde_index = m_indexCount;

	m_model->position[vert_index + 0] = points[0];
	m_model->position[vert_index + 1] = points[1];
	m_model->position[vert_index + 2] = points[2];
	m_model->position[vert_index + 3] = points[3];

	m_model->normal[vert_index + 0] = normal;
	m_model->normal[vert_index + 1] = normal;
	m_model->normal[vert_index + 2] = normal;
	m_model->normal[vert_index + 3] = normal;

	m_model->color[vert_index + 0] = color.raw;
	m_model->color[vert_index + 1] = color.raw;
	m_model->color[vert_index + 2] = color.raw;
	m_model->color[vert_index + 3] = color.raw;

	m_model->texcoord0[vert_index + 0] = Vector3f(0, 0, 0);
	m_model->texcoord0[vert_index + 1] = Vector3f(1, 0, 0);
	m_model->texcoord0[vert_index + 2] = Vector3f(0, 1, 0);
	m_model->texcoord0[vert_index + 3] = Vector3f(1, 1, 0);

	m_model->indices[inde_index + 0] = vert_index + 0;
	m_model->indices[inde_index + 1] = vert_index + 1;
	m_model->indices[inde_index + 2] = vert_index + 2;
	m_model->indices[inde_index + 3] = vert_index + 3;
	m_model->indices[inde_index + 4] = 0xFFFF;

	m_vertexCount += 4;
	m_indexCount += 5;
}

//	addCube (cubic, rotator, color) : Adds a cube to draw.
// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel.
void rrMeshBuilder::addCube ( const core::math::Cubic& cubic, const Rotator& rotator, const Color& color )
{
	Vector3f cubicCenter = cubic.center();
	Vector3f cubicExtent = cubic.size * 0.5F;

	{	// Bottom (Z-)
		Vector3f face[] = {
			cubicCenter + rotator * Vector3f(-cubicExtent.x, -cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x, -cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f(-cubicExtent.x,  cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x,  cubicExtent.y, -cubicExtent.z)
		};
		addQuad(face, rotator * Vector3f(0, 0, -1), color);
	}

	{	// Top (Z+)
		Vector3f face[] = {
			cubicCenter + rotator * Vector3f(-cubicExtent.x, -cubicExtent.y, cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x, -cubicExtent.y, cubicExtent.z),
			cubicCenter + rotator * Vector3f(-cubicExtent.x,  cubicExtent.y, cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x,  cubicExtent.y, cubicExtent.z)
		};
		addQuad(face, rotator * Vector3f(0, 0, 1), color);
	}

	{	// Left (X-)
		Vector3f face[] = {
			cubicCenter + rotator * Vector3f(-cubicExtent.x, -cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f(-cubicExtent.x,  cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f(-cubicExtent.x, -cubicExtent.y,  cubicExtent.z),
			cubicCenter + rotator * Vector3f(-cubicExtent.x,  cubicExtent.y,  cubicExtent.z)
		};
		addQuad(face, rotator * Vector3f(-1, 0, 0), color);
	}

	{	// Right (X+)
		Vector3f face[] = {
			cubicCenter + rotator * Vector3f( cubicExtent.x, -cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x,  cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x, -cubicExtent.y,  cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x,  cubicExtent.y,  cubicExtent.z)
		};
		addQuad(face, rotator * Vector3f(+1, 0, 0), color);
	}

	{	// Back (Y-)
		Vector3f face[] = {
			cubicCenter + rotator * Vector3f(-cubicExtent.x, -cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x, -cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f(-cubicExtent.x, -cubicExtent.y,  cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x, -cubicExtent.y,  cubicExtent.z)
		};
		addQuad(face, rotator * Vector3f(0, -1, 0), color);
	}

	{	// Front (Y+)
		Vector3f face[] = {
			cubicCenter + rotator * Vector3f(-cubicExtent.x,  cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x,  cubicExtent.y, -cubicExtent.z),
			cubicCenter + rotator * Vector3f(-cubicExtent.x,  cubicExtent.y,  cubicExtent.z),
			cubicCenter + rotator * Vector3f( cubicExtent.x,  cubicExtent.y,  cubicExtent.z)
		};
		addQuad(face, rotator * Vector3f(0, 1, 0), color);
	}
}
#include "rrMeshBuilder2D.h"

#include <cmath>
#include "core/math/Math.h"
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
	return addRectTex(rect, Rect(0.0F, 0.0F, 1.0F, 1.0F), color, outline);
}

//	addRectTex (rect, tex, color, outline) : Adds a rectangle to draw, but with the specified texcoord0.
// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel.
void rrMeshBuilder2D::addRectTex ( const Rect& rect, const Rect& tex, const Color& color, bool outline )
{
	Rect fixedRect = rect;
	fixedRect.Fix();

	const Vector2f pos_min = fixedRect.pos.mulComponents(m_multiplier) + m_offset;
	const Vector2f pos_max = (fixedRect.pos + fixedRect.size).mulComponents(m_multiplier) + m_offset;

	const Vector2f tex_min = tex.pos;
	const Vector2f tex_max = tex.pos + tex.size;

	if (!outline)
	{
		expand(m_vertexCount + 4);
		expandTri(m_triangleCount + 2);
		const uint16_t index = m_vertexCount;
		const uint16_t tri_index = m_triangleCount;

		memset(m_model->vertices + index, 0, sizeof(arModelVertex) * 4);

		// Add the quad:

		m_model->vertices[index + 0].position = pos_min;
		m_model->vertices[index + 1].position = Vector2f(pos_min.x, pos_max.y);
		m_model->vertices[index + 2].position = Vector2f(pos_max.x, pos_min.y);
		m_model->vertices[index + 3].position = pos_max;

		m_model->vertices[index + 0].texcoord0 = tex_min;
		m_model->vertices[index + 1].texcoord0 = Vector2f(tex_min.x, tex_max.y);
		m_model->vertices[index + 2].texcoord0 = Vector2f(tex_max.x, tex_min.y);
		m_model->vertices[index + 3].texcoord0 = tex_max;

		m_model->vertices[index + 0].color = Vector4f(color.raw);
		m_model->vertices[index + 1].color = Vector4f(color.raw);
		m_model->vertices[index + 2].color = Vector4f(color.raw);
		m_model->vertices[index + 3].color = Vector4f(color.raw);

		m_model->vertices[index + 0].position.z = 0.5F;
		m_model->vertices[index + 1].position.z = 0.5F;
		m_model->vertices[index + 2].position.z = 0.5F;
		m_model->vertices[index + 3].position.z = 0.5F;

		m_model->triangles[tri_index + 0].vert[0] = index + 0;
		m_model->triangles[tri_index + 0].vert[1] = index + 1;
		m_model->triangles[tri_index + 0].vert[2] = index + 2;

		m_model->triangles[tri_index + 1].vert[0] = index + 2;
		m_model->triangles[tri_index + 1].vert[1] = index + 1;
		m_model->triangles[tri_index + 1].vert[2] = index + 3;

		m_vertexCount += 4;
		m_triangleCount += 2;
	}
	else
	{
		//const Vector2d pixelSize ( 2.0F/(Real)Screen::Info.width * math::sgn(m_multiplier.x), 2.0F/(Real)Screen::Info.height * math::sgn(m_multiplier.y) );
		const Vector2d pixelSize ( 1.4F * m_multiplier.x, 1.4F * m_multiplier.y );
		const Vector2d pos_min_inside = (fixedRect.pos).mulComponents(m_multiplier) + m_offset + pixelSize;
		const Vector2d pos_max_inside = (fixedRect.pos + fixedRect.size).mulComponents(m_multiplier) + m_offset - pixelSize;

		expand(m_vertexCount + 8);
		expandTri(m_triangleCount + 8);
		const uint16_t index = m_vertexCount;
		const uint16_t tri_index = m_triangleCount;

		memset(m_model->vertices + index, 0, sizeof(arModelVertex) * 8);

		// Add the outline quad:

		m_model->vertices[index + 0].position = pos_min;
		m_model->vertices[index + 1].position = pos_min_inside;
		m_model->vertices[index + 2].position = Vector2f(pos_min.x, pos_max.y);
		m_model->vertices[index + 3].position = Vector2f(pos_min_inside.x, pos_max_inside.y);
		m_model->vertices[index + 4].position = pos_max;
		m_model->vertices[index + 5].position = pos_max_inside;
		m_model->vertices[index + 6].position = Vector2f(pos_max.x, pos_min.y);
		m_model->vertices[index + 7].position = Vector2f(pos_max_inside.x, pos_min_inside.y);

		m_model->vertices[index + 0].texcoord0 = tex_min;
		m_model->vertices[index + 1].texcoord0 = tex_min;
		m_model->vertices[index + 2].texcoord0 = Vector2f(tex_min.x,tex_max.y);
		m_model->vertices[index + 3].texcoord0 = Vector2f(tex_min.x,tex_max.y);
		m_model->vertices[index + 4].texcoord0 = tex_max;
		m_model->vertices[index + 5].texcoord0 = tex_max;
		m_model->vertices[index + 6].texcoord0 = Vector2f(tex_max.x,tex_min.y);
		m_model->vertices[index + 7].texcoord0 = Vector2f(tex_max.x,tex_min.y);

		m_model->vertices[index + 0].color = Vector4f(color.raw);
		m_model->vertices[index + 1].color = Vector4f(color.raw);
		m_model->vertices[index + 2].color = Vector4f(color.raw);
		m_model->vertices[index + 3].color = Vector4f(color.raw);
		m_model->vertices[index + 4].color = Vector4f(color.raw);
		m_model->vertices[index + 5].color = Vector4f(color.raw);
		m_model->vertices[index + 6].color = Vector4f(color.raw);
		m_model->vertices[index + 7].color = Vector4f(color.raw);

		m_model->vertices[index + 0].position.z = 0.5F;
		m_model->vertices[index + 1].position.z = 0.5F;
		m_model->vertices[index + 2].position.z = 0.5F;
		m_model->vertices[index + 3].position.z = 0.5F;
		m_model->vertices[index + 4].position.z = 0.5F;
		m_model->vertices[index + 5].position.z = 0.5F;
		m_model->vertices[index + 6].position.z = 0.5F;
		m_model->vertices[index + 7].position.z = 0.5F;

		for (uint i = 0; i < 4; ++i)
		{
			m_model->triangles[tri_index + 0 + i * 2].vert[0] = index + (0 + i * 2) % 8;
			m_model->triangles[tri_index + 0 + i * 2].vert[1] = index + (1 + i * 2) % 8;
			m_model->triangles[tri_index + 0 + i * 2].vert[2] = index + (2 + i * 2) % 8;

			m_model->triangles[tri_index + 1 + i * 2].vert[0] = index + (2 + i * 2) % 8;
			m_model->triangles[tri_index + 1 + i * 2].vert[1] = index + (1 + i * 2) % 8;
			m_model->triangles[tri_index + 1 + i * 2].vert[2] = index + (3 + i * 2) % 8;
		}

		m_vertexCount += 8;
		m_triangleCount += 8;
	}
}

//	addLine (point1, point2, color) : Adds a line to draw.
// The "line" is actually a very thin quad, with a width of what is calculated to be one pixel.
void rrMeshBuilder2D::addLine ( const Vector2f& point1, const Vector2f& point2, const Color& color )
{
	const Vector2d dir = (point2 - point1).normal();
	const Vector2d pos1 = (point1 - dir*0.47F).mulComponents(m_multiplier) + m_offset;
	const Vector2d pos2 = (point2 + dir*0.47F).mulComponents(m_multiplier) + m_offset;

	const Vector2d pixel_offset = Vector2d(
		//-dir.y * 1.0F / (Real)Screen::Info.width,
		-dir.y * 0.51F * m_multiplier.x,
		//+dir.x * 1.0F / (Real)Screen::Info.height);
		+dir.x * 0.51F * m_multiplier.y);

	expand(m_vertexCount + 4);
	expandTri(m_triangleCount + 2);

	{
		const uint16_t index = m_vertexCount;
		const uint16_t tri_index = m_triangleCount;

		memset(m_model->vertices + index, 0, sizeof(arModelVertex) * 4);

		// Layout: 0 2
		//         1 3

		m_model->vertices[index + 0].position = pos1 - pixel_offset;
		m_model->vertices[index + 1].position = pos2 - pixel_offset;
		m_model->vertices[index + 2].position = pos1 + pixel_offset;
		m_model->vertices[index + 3].position = pos2 + pixel_offset;

		m_model->vertices[index + 0].texcoord0 = Vector2f(0.5F, 0.5F);
		m_model->vertices[index + 1].texcoord0 = Vector2f(0.5F, 0.5F);
		m_model->vertices[index + 2].texcoord0 = Vector2f(0.5F, 0.5F);
		m_model->vertices[index + 3].texcoord0 = Vector2f(0.5F, 0.5F);

		m_model->vertices[index + 0].color = Vector4f(color.raw);
		m_model->vertices[index + 1].color = Vector4f(color.raw);
		m_model->vertices[index + 2].color = Vector4f(color.raw);
		m_model->vertices[index + 3].color = Vector4f(color.raw);

		m_model->vertices[index + 0].position.z = 0.5F;
		m_model->vertices[index + 1].position.z = 0.5F;
		m_model->vertices[index + 2].position.z = 0.5F;
		m_model->vertices[index + 3].position.z = 0.5F;

		m_model->triangles[tri_index + 0].vert[0] = index + 0;
		m_model->triangles[tri_index + 0].vert[1] = index + 1;
		m_model->triangles[tri_index + 0].vert[2] = index + 2;

		m_model->triangles[tri_index + 1].vert[0] = index + 2;
		m_model->triangles[tri_index + 1].vert[1] = index + 1;
		m_model->triangles[tri_index + 1].vert[2] = index + 3;
	}

	m_vertexCount += 4;
	m_triangleCount += 2;
}

//	addCircle (center, radius, color, divs, outline) : Adds a circle to draw.
// "Wireframe" is done via as many thin quads as divs, inset by what is calculated to be one pixel.
// The argument "divs" refers to how many unique angles will be used to build the circle.
// A 20-div circle adds 20 triangles at 18 degree intervals (360 degrees divided by 20).
void rrMeshBuilder2D::addCircle ( const Vector2f& center, const Real radius, const Color& color, int divs, bool outline )
{
	const Vector2f pos_center = center.mulComponents(m_multiplier) + m_offset;
	const Vector2f size = Vector2f(radius, radius).mulComponents(m_multiplier);
	const Real kAngleDiv = (Real)(2 * M_PI / divs);

	if (!outline)
	{
		expand(m_vertexCount + divs + 1);
		expandTri(m_triangleCount + divs);
		const uint16_t index = m_vertexCount;
		const uint16_t tri_index = m_triangleCount;

		memset(m_model->vertices + index, 0, sizeof(arModelVertex) * (divs + 1));

		m_model->vertices[index + 0].position = pos_center;
		m_model->vertices[index + 0].position.z = 0.5F;
		m_model->vertices[index + 0].texcoord0 = Vector2f(0.5F, 0.5F);
		m_model->vertices[index + 0].color = Vector4f(color.raw);

		for (int i = 0; i < divs; ++i)
		{
			m_model->vertices[index + 1 + i].texcoord0 = Vector2f(cosf(kAngleDiv * i), sinf(kAngleDiv * i));

			m_model->vertices[index + 1 + i].position = pos_center + m_model->vertices[index + 1 + i].texcoord0.mulComponents(size);
			m_model->vertices[index + 1 + i].position.z = 0.5F;

			m_model->vertices[index + 1 + i].color = Vector4f(color.raw);

			m_model->triangles[tri_index + i].vert[0] = index;
			m_model->triangles[tri_index + i].vert[1] = index + i % divs + 1;
			m_model->triangles[tri_index + i].vert[2] = index + (i + 1) % divs + 1;
		}

		m_vertexCount += divs + 1;
		m_triangleCount += divs;
	}
	else
	{
		for (int i = 0; i < divs; ++i)
		{
			addLine(
				center + Vector2f(cosf(kAngleDiv * (i+0)), sinf(kAngleDiv * (i+0))) * radius,
				center + Vector2f(cosf(kAngleDiv * (i+1)), sinf(kAngleDiv * (i+1))) * radius,
				color);
		}
	}
}

//	addQuad (vertices [4]) : Adds a quad to draw.
// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel. It may not be correct.
void rrMeshBuilder2D::addQuad ( const arModelVertex* vertices, bool outline )
{
	if (!outline)
	{
		expand(m_vertexCount + 4);
		expandTri(m_triangleCount + 2);
		const uint16_t index = m_vertexCount;
		const uint16_t tri_index = m_triangleCount;

		memcpy(m_model->vertices + index, vertices, sizeof(arModelVertex) * 4);

		for (uint i = 0; i < 4; ++i)
		{	// Update positions:
			m_model->vertices[index + i].position = m_model->vertices[index + i].position.mulComponents(m_multiplier) + m_offset;
			m_model->vertices[index + i].position.z = 0.5F;
		}

		m_model->triangles[tri_index + 0].vert[0] = index + 0;
		m_model->triangles[tri_index + 0].vert[1] = index + 1;
		m_model->triangles[tri_index + 0].vert[2] = index + 2;

		m_model->triangles[tri_index + 1].vert[0] = index + 2;
		m_model->triangles[tri_index + 1].vert[1] = index + 1;
		m_model->triangles[tri_index + 1].vert[2] = index + 3;

		m_vertexCount += 4;
		m_triangleCount += 2;
	}
	else
	{
	}
}
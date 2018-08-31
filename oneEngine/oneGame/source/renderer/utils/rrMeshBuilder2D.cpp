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
		expandIndices(m_indexCount + 5);
		const uint16_t vert_index = m_vertexCount;
		const uint16_t inde_index = m_indexCount;

		//memset(m_model->vertices + index, 0, sizeof(arModelVertex) * 4);

		// Add the quad:

		m_model->position[vert_index + 0] = pos_min;
		m_model->position[vert_index + 1] = Vector2f(pos_min.x, pos_max.y);
		m_model->position[vert_index + 2] = Vector2f(pos_max.x, pos_min.y);
		m_model->position[vert_index + 3] = pos_max;

		m_model->texcoord0[vert_index + 0] = tex_min;
		m_model->texcoord0[vert_index + 1] = Vector2f(tex_min.x, tex_max.y);
		m_model->texcoord0[vert_index + 2] = Vector2f(tex_max.x, tex_min.y);
		m_model->texcoord0[vert_index + 3] = tex_max;

		m_model->color[vert_index + 0] = Vector4f(color.raw);
		m_model->color[vert_index + 1] = Vector4f(color.raw);
		m_model->color[vert_index + 2] = Vector4f(color.raw);
		m_model->color[vert_index + 3] = Vector4f(color.raw);

		m_model->position[vert_index + 0].z = 0.5F;
		m_model->position[vert_index + 1].z = 0.5F;
		m_model->position[vert_index + 2].z = 0.5F;
		m_model->position[vert_index + 3].z = 0.5F;

		/*m_model->indices[inde_index + 0] = index + 0;
		m_model->indices[inde_index + 1] = index + 1;
		m_model->indices[inde_index + 2] = index + 2;

		m_model->indices[inde_index + 3] = index + 2;
		m_model->indices[inde_index + 4] = index + 1;
		m_model->indices[inde_index + 5] = index + 3;*/

		m_model->indices[inde_index + 0] = vert_index + 0;
		m_model->indices[inde_index + 1] = vert_index + 1;
		m_model->indices[inde_index + 2] = vert_index + 2;
		m_model->indices[inde_index + 3] = vert_index + 3;
		m_model->indices[inde_index + 4] = 0xFFFF;

		m_vertexCount += 4;
		m_indexCount += 5;
	}
	else
	{
		//const Vector2d pixelSize ( 2.0F/(Real)Screen::Info.width * math::sgn(m_multiplier.x), 2.0F/(Real)Screen::Info.height * math::sgn(m_multiplier.y) );
		const Vector2d pixelSize ( 1.4F * m_multiplier.x, 1.4F * m_multiplier.y );
		const Vector2d pos_min_inside = (fixedRect.pos).mulComponents(m_multiplier) + m_offset + pixelSize;
		const Vector2d pos_max_inside = (fixedRect.pos + fixedRect.size).mulComponents(m_multiplier) + m_offset - pixelSize;

		expand(m_vertexCount + 8);
		//expandTri(m_triangleCount + 8);
		expandIndices(m_indexCount + 11);
		const uint16_t vert_index = m_vertexCount;
		const uint16_t inde_index = m_indexCount;

		//memset(m_model->vertices + index, 0, sizeof(arModelVertex) * 8);

		// Add the outline quad:

		m_model->position[vert_index + 0] = pos_min;
		m_model->position[vert_index + 1] = pos_min_inside;
		m_model->position[vert_index + 2] = Vector2f(pos_min.x, pos_max.y);
		m_model->position[vert_index + 3] = Vector2f(pos_min_inside.x, pos_max_inside.y);
		m_model->position[vert_index + 4] = pos_max;
		m_model->position[vert_index + 5] = pos_max_inside;
		m_model->position[vert_index + 6] = Vector2f(pos_max.x, pos_min.y);
		m_model->position[vert_index + 7] = Vector2f(pos_max_inside.x, pos_min_inside.y);

		m_model->texcoord0[vert_index + 0] = tex_min;
		m_model->texcoord0[vert_index + 1] = tex_min;
		m_model->texcoord0[vert_index + 2] = Vector2f(tex_min.x,tex_max.y);
		m_model->texcoord0[vert_index + 3] = Vector2f(tex_min.x,tex_max.y);
		m_model->texcoord0[vert_index + 4] = tex_max;
		m_model->texcoord0[vert_index + 5] = tex_max;
		m_model->texcoord0[vert_index + 6] = Vector2f(tex_max.x,tex_min.y);
		m_model->texcoord0[vert_index + 7] = Vector2f(tex_max.x,tex_min.y);

		m_model->color[vert_index + 0] = Vector4f(color.raw);
		m_model->color[vert_index + 1] = Vector4f(color.raw);
		m_model->color[vert_index + 2] = Vector4f(color.raw);
		m_model->color[vert_index + 3] = Vector4f(color.raw);
		m_model->color[vert_index + 4] = Vector4f(color.raw);
		m_model->color[vert_index + 5] = Vector4f(color.raw);
		m_model->color[vert_index + 6] = Vector4f(color.raw);
		m_model->color[vert_index + 7] = Vector4f(color.raw);

		m_model->position[vert_index + 0].z = 0.5F;
		m_model->position[vert_index + 1].z = 0.5F;
		m_model->position[vert_index + 2].z = 0.5F;
		m_model->position[vert_index + 3].z = 0.5F;
		m_model->position[vert_index + 4].z = 0.5F;
		m_model->position[vert_index + 5].z = 0.5F;
		m_model->position[vert_index + 6].z = 0.5F;
		m_model->position[vert_index + 7].z = 0.5F;

		/*for (uint i = 0; i < 4; ++i)
		{
			m_model->triangles[tri_index + 0 + i * 2].vert[0] = index + (0 + i * 2) % 8;
			m_model->triangles[tri_index + 0 + i * 2].vert[1] = index + (1 + i * 2) % 8;
			m_model->triangles[tri_index + 0 + i * 2].vert[2] = index + (2 + i * 2) % 8;

			m_model->triangles[tri_index + 1 + i * 2].vert[0] = index + (2 + i * 2) % 8;
			m_model->triangles[tri_index + 1 + i * 2].vert[1] = index + (1 + i * 2) % 8;
			m_model->triangles[tri_index + 1 + i * 2].vert[2] = index + (3 + i * 2) % 8;
		}*/

		m_model->indices[inde_index +  0] = vert_index + 0;
		m_model->indices[inde_index +  1] = vert_index + 1;
		m_model->indices[inde_index +  2] = vert_index + 2;
		m_model->indices[inde_index +  3] = vert_index + 3;
		m_model->indices[inde_index +  4] = vert_index + 4;
		m_model->indices[inde_index +  5] = vert_index + 5;
		m_model->indices[inde_index +  6] = vert_index + 6;
		m_model->indices[inde_index +  7] = vert_index + 7;
		m_model->indices[inde_index +  8] = vert_index + 0;
		m_model->indices[inde_index +  9] = vert_index + 1;
		m_model->indices[inde_index + 10] = 0xFFFF;

		m_vertexCount += 8;
		//m_triangleCount += 8;
		m_indexCount += 11;
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
	//expandTri(m_triangleCount + 2);
	expandIndices(m_indexCount + 5);

	{
		const uint16_t vert_index = m_vertexCount;
		const uint16_t inde_index = m_indexCount;

		//memset(m_model->vertices + index, 0, sizeof(arModelVertex) * 4);

		// Layout: 0 2
		//         1 3

		m_model->position[vert_index + 0] = pos1 - pixel_offset;
		m_model->position[vert_index + 1] = pos2 - pixel_offset;
		m_model->position[vert_index + 2] = pos1 + pixel_offset;
		m_model->position[vert_index + 3] = pos2 + pixel_offset;

		m_model->texcoord0[vert_index + 0] = Vector2f(0.5F, 0.5F);
		m_model->texcoord0[vert_index + 1] = Vector2f(0.5F, 0.5F);
		m_model->texcoord0[vert_index + 2] = Vector2f(0.5F, 0.5F);
		m_model->texcoord0[vert_index + 3] = Vector2f(0.5F, 0.5F);

		m_model->color[vert_index + 0] = Vector4f(color.raw);
		m_model->color[vert_index + 1] = Vector4f(color.raw);
		m_model->color[vert_index + 2] = Vector4f(color.raw);
		m_model->color[vert_index + 3] = Vector4f(color.raw);

		m_model->position[vert_index + 0].z = 0.5F;
		m_model->position[vert_index + 1].z = 0.5F;
		m_model->position[vert_index + 2].z = 0.5F;
		m_model->position[vert_index + 3].z = 0.5F;

		/*m_model->triangles[tri_index + 0].vert[0] = index + 0;
		m_model->triangles[tri_index + 0].vert[1] = index + 1;
		m_model->triangles[tri_index + 0].vert[2] = index + 2;

		m_model->triangles[tri_index + 1].vert[0] = index + 2;
		m_model->triangles[tri_index + 1].vert[1] = index + 1;
		m_model->triangles[tri_index + 1].vert[2] = index + 3;*/

		m_model->indices[inde_index + 0] = vert_index + 0;
		m_model->indices[inde_index + 1] = vert_index + 1;
		m_model->indices[inde_index + 2] = vert_index + 2;
		m_model->indices[inde_index + 3] = vert_index + 3;
		m_model->indices[inde_index + 4] = 0xFFFF;
	}

	m_vertexCount += 4;
	//m_triangleCount += 2;
	m_indexCount += 5;
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
		expandIndices(m_indexCount + (divs + 1) * 2 + 1);
		const uint16_t vert_index = m_vertexCount;
		const uint16_t inde_index = m_indexCount;

		//memset(m_model->vertices + index, 0, sizeof(arModelVertex) * (divs + 1));

		m_model->position[vert_index + 0] = pos_center;
		m_model->position[vert_index + 0].z = 0.5F;
		m_model->texcoord0[vert_index + 0] = Vector2f(0.5F, 0.5F);
		m_model->color[vert_index + 0] = Vector4f(color.raw);

		for (int i = 0; i < divs; ++i)
		{
			m_model->texcoord0[vert_index + 1 + i] = Vector2f(cosf(kAngleDiv * i), sinf(kAngleDiv * i));

			m_model->position[vert_index + 1 + i] = pos_center + m_model->texcoord0[vert_index + 1 + i].mulComponents(size);
			m_model->position[vert_index + 1 + i].z = 0.5F;

			m_model->color[vert_index + 1 + i] = Vector4f(color.raw);

			m_model->indices[inde_index + i + 0] = vert_index;
			m_model->indices[inde_index + i + 1] = vert_index + i % divs + 1;
			m_model->indices[inde_index + i + 2] = vert_index + (i + 1) % divs + 1;
		}

		m_vertexCount += divs + 1;
		m_indexCount += (divs + 1) * 2 + 1;
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
		expandIndices(m_indexCount + 5);
		const uint16_t vert_index = m_vertexCount;
		const uint16_t inde_index = m_indexCount;

		//memcpy(m_model->vertices + index, vertices, sizeof(arModelVertex) * 4);

		for (uint i = 0; i < 4; ++i)
		{	// Update positions:
			m_model->position[vert_index + i] = m_model->position[vert_index + i].mulComponents(m_multiplier) + m_offset;
			m_model->position[vert_index + i].z = 0.5F;
		}

		/*m_model->indices[inde_index + 0].vert[0] = index + 0;
		m_model->indices[inde_index + 0].vert[1] = index + 1;
		m_model->indices[inde_index + 0].vert[2] = index + 2;

		m_model->indices[inde_index + 1].vert[0] = index + 2;
		m_model->indices[inde_index + 1].vert[1] = index + 1;
		m_model->indices[inde_index + 1].vert[2] = index + 3;*/
		m_model->indices[inde_index + 0] = vert_index + 0;
		m_model->indices[inde_index + 1] = vert_index + 1;
		m_model->indices[inde_index + 2] = vert_index + 2;
		m_model->indices[inde_index + 3] = vert_index + 3;
		m_model->indices[inde_index + 4] = 0xFFFF;

		m_vertexCount += 4;
		m_indexCount += 5;
	}
	else
	{
	}
}
#ifndef RENDERER_UTILS_RR_MESH_BUILDER_2D_H_
#define RENDERER_UTILS_RR_MESH_BUILDER_2D_H_

#include "IrrMeshBuilder.h"

#include "core/math/Rect.h"
#include "core/math/Color.h"
#include "core/math/Cubic.h"

class rrMeshBuilder2D : public IrrMeshBuilder
{
public:
	//	Constructor (new data)
	// Pulls a model from the the pool that has at least the estimated input size.
	// If the estimation is incorrect, the data will be resized.
	// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
	RENDER_API explicit		rrMeshBuilder2D ( const uint16_t estimatedVertexCount );
	//	Constructor (existing data)
	// Sets up model, using the input data.
	// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
	// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
	RENDER_API explicit		rrMeshBuilder2D ( arModelData* preallocatedModelData );
	//	Constructor (cubic, new data)
	// Pulls a model from the the pool that has at least the estimated input size.
	// If the estimation is incorrect, the data will be resized.
	RENDER_API explicit		rrMeshBuilder2D ( const core::math::Cubic& screenMapping, const uint16_t estimatedVertexCount );
	//	Constructor (cubic, existing data)
	// Sets up model, using the input data.
	// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
	RENDER_API explicit		rrMeshBuilder2D ( const core::math::Cubic& screenMapping, arModelData* preallocatedModelData );


	//	setScreenMapping (cubic) : Sets the mapping of the Rect coordinates to the screen.
	RENDER_API void			setScreenMapping ( const core::math::Cubic& screenMapping );

	//	addRect (rect, color, outline) : Adds a rectangle to draw.
	// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel.
	RENDER_API void			addRect ( const Rect& rect, const Color& color, bool outline );

protected:
	Vector2d	m_multiplier;
	Vector2d	m_offset;
};

#endif//RENDERER_UTILS_RR_MESH_BUILDER_2D_H_
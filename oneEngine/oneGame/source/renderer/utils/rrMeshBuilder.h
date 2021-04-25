#ifndef RENDERER_UTILS_RR_MESH_BUILDER_H_
#define RENDERER_UTILS_RR_MESH_BUILDER_H_

#include "IrrMeshBuilder.h"

#include "core/math/Rect.h"
#include "core/math/Color.h"
#include "core/math/Cubic.h"

class rrMeshBuilder : public IrrMeshBuilder
{
public:
	//	Constructor (new data)
	// Pulls a model from the the pool that has at least the estimated input size.
	// If the estimation is incorrect, the data will be resized.
	// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
	// To assist in estimation, there are 4 vertices used per quad, and 16 per outlined quad.
	RENDER_API explicit		rrMeshBuilder ( const uint16_t estimatedVertexCount );
	//	Constructor (existing data)
	// Sets up model, using the input data.
	// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
	// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
	RENDER_API explicit		rrMeshBuilder ( arModelData* preallocatedModelData, uint16_t initialVertexCount = 0, uint16_t initialIndexCount = 0 );

	//	getPrimitiveMode () : returns the primitive mode this mesh builder would like to render in.
	// Must be implemented by child classes.
	RENDER_API renderer::rrPrimitiveMode
							getPrimitiveMode ( void ) const override
		{ return renderer::kPrimitiveModeTriangleList_Indexed; }

	//	addQuad (points[4], normal, color) : Adds a quad to draw.
	RENDER_API void			addQuad ( const Vector3f points [4], const Vector3f normal, const Color& color );

	//	addCube (cubic, rotator, color) : Adds a cube to draw.
	RENDER_API void			addCube ( const core::math::Cubic& cubic, const Rotator& rotator, const Color& color );

	//	addTriangle (points[3]) : Adds a triangle to draw
	RENDER_API void			addTriangle ( const Vector3f points [3] );

	//	addTriangleStrip (points[], pointCount) : Adds a triangle strip to draw.
	RENDER_API void			addTriangleStrip ( const Vector3f* points, const uint32_t pointCount );

};

#endif//RENDERER_UTILS_RR_MESH_BUILDER_H_
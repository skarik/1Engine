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
	ARCORE_ASSERT(false);
}

//	addCube (cubic, rotator, color) : Adds a cube to draw.
// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel.
void rrMeshBuilder::addCube ( const core::math::Cubic& cubic, const Rotator& rotator, const Color& color )
{
	ARCORE_ASSERT(false);

	// four addQuad
}
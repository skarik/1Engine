#ifndef RENDERER_UTILS_RR_TEXT_BUILDER_2D_H_
#define RENDERER_UTILS_RR_TEXT_BUILDER_2D_H_

#include "IrrMeshBuilder.h"

#include "core/math/Rect.h"
#include "core/math/Color.h"
#include "core/math/Cubic.h"

class CBitmapFont;

class rrTextBuilder2D : public IrrMeshBuilder
{
public:
	//	Constructor (new data)
	// Pulls a model from the the pool that has at least the estimated input size.
	// If the estimation is incorrect, the data will be resized.
	// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
	// To assist in estimation, there are 6 vertices used per quad, and 24 per outlined quad.
	RENDER_API explicit		rrTextBuilder2D ( CBitmapFont* font, const uint16_t estimatedVertexCount );
	//	Constructor (existing data)
	// Sets up model, using the input data.
	// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
	// The screen mapping for the meshes created defaults to 1:1 pixel-mode mapping.
	RENDER_API explicit		rrTextBuilder2D ( CBitmapFont* font, arModelData* preallocatedModelData );
	//	Constructor (cubic, new data)
	// Pulls a model from the the pool that has at least the estimated input size.
	// If the estimation is incorrect, the data will be resized.
	// To assist in estimation, there are 6 vertices used per quad, and 24 per outlined quad.
	RENDER_API explicit		rrTextBuilder2D ( CBitmapFont* font, const core::math::Cubic& screenMapping, const uint16_t estimatedVertexCount );
	//	Constructor (cubic, existing data)
	// Sets up model, using the input data.
	// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
	RENDER_API explicit		rrTextBuilder2D ( CBitmapFont* font, const core::math::Cubic& screenMapping, arModelData* preallocatedModelData );

	//	getPrimitiveMode () : returns the primitive mode this mesh builder would like to render in.
	// Must be implemented by child classes.
	RENDER_API renderer::rrPrimitiveMode
		getPrimitiveMode ( void ) const override
	{ return renderer::kPrimitiveModeTriangleList_Indexed; }

	//	setScreenMapping (cubic) : Sets the mapping of the Rect coordinates to the screen.
	RENDER_API void			setScreenMapping ( const core::math::Cubic& screenMapping );

	//	addRect (rect, color, outline) : Adds a rectangle to draw.
	// "Wireframe" is done via four thin quads, inset by what is calculated to be one pixel.
	//RENDER_API void			addRect ( const Rect& rect, const Color& color, bool outline );


	RENDER_API void			addText ( const Vector2d& position, const Color& color, const char* str );

protected:
	Vector2d	m_multiplier;
	Vector2d	m_offset;

	CBitmapFont*	m_font_texture;
};

#endif//RENDERER_UTILS_RR_TEXT_BUILDER_2D_H_
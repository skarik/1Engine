// Cube primitive, create it with some width and height!

#ifndef _C_CUBE_RENDERABLE_PRIMITIVE_H_
#define _C_CUBE_RENDERABLE_PRIMITIVE_H_

// Includes
#include "CRenderablePrimitive.h"

// Class definition
class CCubeRenderablePrimitive : public CRenderablePrimitive
{
public:
	RENDER_API CCubeRenderablePrimitive ( void );
	RENDER_API CCubeRenderablePrimitive ( Real width, Real depth, Real height );

	// Render function
	bool Render ( const char pass );

	// Set size
	RENDER_API void SetSize ( Real width, Real depth, Real height );

protected:
	void GenerateVertices ( void );
protected:
	Real fWidth;
	Real fDepth;
	Real fHeight;
};

// Typedef for ease, because that's a friggin horrible name (change later)
typedef CCubeRenderablePrimitive CCubeRenderPrim;

#endif
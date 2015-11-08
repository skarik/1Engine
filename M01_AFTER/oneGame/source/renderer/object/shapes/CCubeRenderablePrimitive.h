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
	RENDER_API CCubeRenderablePrimitive ( ftype width, ftype depth, ftype height );

	// Render function
	bool Render ( const char pass );

	// Set size
	RENDER_API void SetSize ( ftype width, ftype depth, ftype height );

protected:
	void GenerateVertices ( void );
protected:
	ftype fWidth;
	ftype fDepth;
	ftype fHeight;
};

// Typedef for ease, because that's a friggin horrible name (change later)
typedef CCubeRenderablePrimitive CCubeRenderPrim;

#endif
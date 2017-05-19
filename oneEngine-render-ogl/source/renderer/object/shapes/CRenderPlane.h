
#ifndef _C_RENDER_PLANE_
#define _C_RENDER_PLANE_

// ==Includes==
#include "CRenderablePrimitive.h"

// todo: add subdividing

class CRenderPlane : public CRenderablePrimitive
{
public:
	RENDER_API CRenderPlane ( Real xsize = 1.0f, Real ysize = 1.0f );
	RENDER_API ~CRenderPlane();

	RENDER_API void SetSize ( Real xsize = 1.0f, Real ysize = 1.0f );

private:
	Real width;
	Real height;

	void GenerateShape ( Real xsize, Real ysize );

};


#endif
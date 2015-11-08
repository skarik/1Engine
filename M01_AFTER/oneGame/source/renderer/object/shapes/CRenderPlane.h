
#ifndef _C_RENDER_PLANE_
#define _C_RENDER_PLANE_

// ==Includes==
#include "CRenderablePrimitive.h"

// todo: add subdividing

class CRenderPlane : public CRenderablePrimitive
{
public:
	RENDER_API CRenderPlane ( ftype xsize = 1.0f, ftype ysize = 1.0f );
	RENDER_API ~CRenderPlane();

	RENDER_API void SetSize ( ftype xsize = 1.0f, ftype ysize = 1.0f );

private:
	ftype width;
	ftype height;

	void GenerateShape ( ftype xsize, ftype ysize );

};


#endif
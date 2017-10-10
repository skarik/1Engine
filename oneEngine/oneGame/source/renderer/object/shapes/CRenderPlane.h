#ifndef C_RENDER_PLANE_
#define C_RENDER_PLANE_

// ==Includes==
#include "CRenderablePrimitive.h"

class CRenderPlane : public CRenderablePrimitive
{
public:
	RENDER_API explicit		CRenderPlane ( Real xsize = 1.0f, Real ysize = 1.0f );
	RENDER_API				~CRenderPlane();

	RENDER_API void			setSize ( Real xsize = 1.0f, Real ysize = 1.0f );

private:
	void					buildPlane ( Real xsize, Real ysize );
};

#endif//C_RENDER_PLANE_
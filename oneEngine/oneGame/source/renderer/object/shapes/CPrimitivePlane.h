#ifndef C_PRIMITIVE_PLANE_
#define C_PRIMITIVE_PLANE_

#include "renderer/object/CRenderable3D.h"

class CPrimitivePlane : public CRenderable3D
{
public:
	RENDER_API explicit		CPrimitivePlane ( Real xsize = 1.0f, Real ysize = 1.0f );
	RENDER_API				~CPrimitivePlane();

	RENDER_API void			setSize ( Real xsize = 1.0f, Real ysize = 1.0f );

private:
	void					buildPlane ( Real xsize, Real ysize );
};

#endif//C_PRIMITIVE_PLANE_
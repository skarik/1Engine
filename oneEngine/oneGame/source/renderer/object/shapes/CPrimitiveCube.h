#ifndef C_CUBE_RENDERABLE_PRIMITIVE_H_
#define C_CUBE_RENDERABLE_PRIMITIVE_H_

// Includes
#include "renderer/object/CRenderable3D.h"

// Class definition
class CPrimitiveCube : public CRenderable3D
{
public:
	RENDER_API explicit	CPrimitiveCube ( void );
	RENDER_API explicit	CPrimitiveCube ( Real width, Real depth, Real height );

	RENDER_API void		setSize ( Real width, Real depth, Real height );

protected:
	void				buildCube ( Real x, Real y, Real z );
};

#endif//C_CUBE_RENDERABLE_PRIMITIVE_H_
#ifndef C_PRIMITIVE_ISOSPHERE_H_
#define C_PRIMITIVE_ISOSPHERE_H_

#include "CRenderablePrimitive.h"
#include <vector>

class CPrimitiveIsosphere : public CRenderablePrimitive
{
public:
	// Constructor and Destructor
	RENDER_API explicit	CPrimitiveIsosphere ( int divisions = 1 );
	RENDER_API			~CPrimitiveIsosphere ( void );

protected:
	//	buildIsosphere() : Creates and pushes the isosphere mesh
	void				buildIsosphere ( int divisions );
};

#endif//C_PRIMITIVE_ISOSPHERE_H_
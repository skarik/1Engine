#ifndef C_ISOSPHERE_H_
#define C_ISOSPHERE_H_

#include "CRenderablePrimitive.h"
#include <vector>

class CIsosphere : public CRenderablePrimitive
{
public:
	// Constructor and Destructor
	RENDER_API explicit	CIsosphere ( int divisions = 1 );
	RENDER_API			~CIsosphere ( void );

protected:
	//	buildIsosphere() : Creates and pushes the isosphere mesh
	void				buildIsosphere ( int divisions );
};

#endif//C_ISOSPHERE_H_
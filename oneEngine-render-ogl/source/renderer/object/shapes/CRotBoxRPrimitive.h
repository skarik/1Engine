// Old test version of the cube primitive. Was really entertaining.

// Usage example
// CRotBoxRPrimitive aRotBoxes [1000];

#ifndef _C_ROT_BOX_R_PRIMITIVE_
#define _C_ROT_BOX_R_PRIMITIVE_

// Includes
#include "CRenderablePrimitive.h"

// Class definition
class CRotBoxRPrimitive : public CRenderablePrimitive
{
public:
	CRotBoxRPrimitive ( void );
	CRotBoxRPrimitive ( ftype width, ftype depth, ftype height );

	// Render function
	bool Render ( const char pass );
protected:
	void GenerateVertices ( void );
protected:
	ftype fWidth;
	ftype fDepth;
	ftype fHeight;

	ftype fRot;
};

#endif
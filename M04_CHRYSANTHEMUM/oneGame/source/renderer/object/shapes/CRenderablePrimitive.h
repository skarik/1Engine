// Renderable Primitive class
// The renderable primitive is not primitive, but renders an array of vertices simply.
// To render using this class, derive from this class and set the array, or set the array after instantation

#ifndef _C_RENDERABLE_PRIMITIVE_
#define _C_RENDERABLE_PRIMITIVE_

// Includes
#include "core/types/ModelData.h"
#include "core/math/Vector3d.h"
#include "../CRenderableObject.h"
#include "renderer/material/glMaterial.h"

// Class definition
class CRenderablePrimitive : public CRenderableObject
{
public:
	// Constructor and Destructor
	RENDER_API CRenderablePrimitive ( void );
	RENDER_API ~CRenderablePrimitive ( void );

	// Render function
	bool Render ( const char pass );

	// Set material
	//void SetMaterial ( glMaterial* );

	// Set vertices
	RENDER_API void SetVertices ( CModelVertex*, unsigned int );

protected:
	CModelVertex * vertexData;	// In derived classes, this class retains ownership
	unsigned int vertexNum;

	// Material list
	//vector<glMaterial*> vMaterials;
	//void ClearMaterialList ( void );
};


#endif
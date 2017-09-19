// Renderable Primitive class
// The renderable primitive is not primitive, but renders an array of vertices simply.
// To render using this class, derive from this class and set the array, or set the array after instantation

#ifndef _C_RENDERABLE_PRIMITIVE_
#define _C_RENDERABLE_PRIMITIVE_

// Includes
#include "core/types/ModelData.h"
#include "core/math/Vector3d.h"
#include "../CRenderableObject.h"
#include "renderer/material/RrMaterial.h"

// Class definition
class CRenderablePrimitive : public CRenderableObject
{
public:
	// Constructor and Destructor
	RENDER_API CRenderablePrimitive ( void );
	RENDER_API ~CRenderablePrimitive ( void );

	bool PreRender ( void ) override;
	bool Render ( const char pass ) override;

	// Set material
	//void SetMaterial ( RrMaterial* );

	// Set vertices
	RENDER_API void SetVertices ( arModelVertex*, unsigned int );

protected:
	arModelVertex * vertexData;	// In derived classes, this class retains ownership
	unsigned int vertexNum;

	// Material list
	//vector<RrMaterial*> vMaterials;
	//void ClearMaterialList ( void );
};


#endif
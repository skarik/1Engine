

#include "CRenderablePrimitive.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

// Constructor
CRenderablePrimitive::CRenderablePrimitive ( void ) : CRenderableObject()
{
	vertexData = NULL;
	vertexNum = 0;
}

// Destructor
CRenderablePrimitive::~CRenderablePrimitive ( void )
{
	if ( vertexData != NULL )
		delete [] vertexData;
	vertexData = NULL;
	vertexNum = 0;
}

// Render object
bool CRenderablePrimitive::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	GL.Transform( &transform );

	m_material->bindPass(pass);
	m_material->setShaderConstants(this);

	// Draw the object
	unsigned int v;
	GLd.BeginPrimitive( GL_TRIANGLES );
	GLd.P_PushColor( Color(1,1,1,1) );
	for ( v = 0; v < vertexNum; v += 1 )
	{
		GLd.P_PushTexcoord( vertexData[v].u, vertexData[v].v );
		GLd.P_PushNormal( vertexData[v].nx, vertexData[v].ny, vertexData[v].nz );
		GLd.P_AddVertex( vertexData[v].x, vertexData[v].y, vertexData[v].z );
	}
	GLd.EndPrimitive();

	return true;
}
/*
// Set the material, clearing the list of materials
void CRenderablePrimitive::SetMaterial ( RrMaterial* pNewMaterial )
{
	ClearMaterialList();
	vMaterials.push_back( pNewMaterial );
}

// Clear up the material list.
void CRenderablePrimitive::ClearMaterialList ( void )
{
	for ( unsigned int i = 0; i < vMaterials.size(); i += 1 )
	{
		// If the material has no external owner, free it.
		//if ( vMaterials[i]->canFree() )
		//	delete vMaterials[i];
	}
	vMaterials.clear();
}
*/
// Set vertices
void CRenderablePrimitive::SetVertices ( CModelVertex* newData, unsigned int newSize )
{
	vertexData	= newData;
	vertexNum	= newSize;
}
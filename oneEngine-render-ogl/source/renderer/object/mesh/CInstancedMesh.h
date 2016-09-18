#ifndef _C_INSTANCED_MESH_H_
#define _C_INSTANCED_MESH_H_
/*
#include "renderer/object/mesh/CMesh.h"

class CInstancedMesh : public CMesh
{
public:
	RENDER_API explicit CInstancedMesh ( glMesh* );
	RENDER_API virtual ~CInstancedMesh ( void );

public:
	//		GetModelData ( ) 
	// Return access to model data
	RENDER_API CModelData* GetModelData ( void );

	//		StreamLockModelData ( )
	// Push the current stuff in model data to GPU.
	RENDER_API void StreamLockModelData ( void );

	//		Render
	// Called during engine render pass.
	// Overrides base class to forward the 
	RENDER_API bool Render ( const char pass ) override;

	//		EndRender
	RENDER_API bool EndRender ( void ) override;

protected:
	void PushModeldata ( void );
};
*/
#endif//_C_INSTANCED_MESH_H_
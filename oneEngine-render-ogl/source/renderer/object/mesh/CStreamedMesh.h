#ifndef _C_STREAMED_MESH_H_
#define _C_STREAMED_MESH_H_

#include "renderer/object/mesh/CMesh.h"

class CStreamedMesh : public CMesh
{

public:
	RENDER_API explicit CStreamedMesh ( void );
	RENDER_API virtual ~CStreamedMesh ( void );

public:
	//		GetModelData ( ) 
	// Return access to model data
	RENDER_API CModelData* GetModelData ( void );

	//		StreamLockModelData ( )
	// Push the current stuff in model data to GPU.
	RENDER_API void StreamLockModelData ( void );

protected:
	void PushModeldata ( void );
};

#endif//_C_STREAMED_MESH_H_
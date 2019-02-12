#ifndef _C_STREAMED_MESH_H_
#define _C_STREAMED_MESH_H_

#include "renderer/object/mesh/Mesh.h"

namespace renderer
{
	class StreamedMesh : public Mesh
	{

	public:
		RENDER_API explicit StreamedMesh ( void );
		RENDER_API virtual ~StreamedMesh ( void );

	public:
		//		GetModelData ( ) 
		// Return access to model data
		RENDER_API arModelData* GetModelData ( void );

		//		StreamLockModelData ( )
		// Push the current stuff in model data to GPU.
		RENDER_API void StreamLockModelData ( void );

	protected:
		void PushModeldata ( void );
	};
}

#endif//_C_STREAMED_MESH_H_
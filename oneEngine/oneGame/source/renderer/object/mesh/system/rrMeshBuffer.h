#ifndef RR_MESH_H_
#define RR_MESH_H_

#include "core/types/ModelData.h"
#include "core/containers/arstring.h"
#include "renderer/types/types.h"
#include "renderer/types/ModelStructures.h"
#include "renderer/types/shaders/vattribs.h"
#include "gpuw/Buffers.h"

/*enum rrMeshBufferType
{
	kMeshBuffer_Normal,
	kMeshBuffer_Skinned,
};*/

class rrMeshBuffer
{
public:
	RENDER_API				rrMeshBuffer ( void );
	RENDER_API				~rrMeshBuffer ( void );

	//
	// Setters:

	//	InitMeshBuffers( mesh ) : Creates mesh buffers based on the model data.
	// Will call FreeMeshBuffers() if there is already a mesh loaded up.
	// Will hold onto the input model data.
	RENDER_API void			InitMeshBuffers ( arModelData* const modelData );
	
	//	FreeMeshBuffers() : Frees mesh buffers
	RENDER_API void			FreeMeshBuffers ( void );
	
	//	StreamMeshBuffers( mesh ) : Updates contents of mesh buffers based the model data.
	// Performs similar actions to InitMeshBuffers, but is optimized for per-frame streaming..
	// Will hold onto the input model data, as it may not be finished when the function exits.
	RENDER_API void			StreamMeshBuffers ( arModelData* const modelData );

public:
	gpu::Buffer			m_buffer [renderer::shader::kVBufferSlotMaxCount];
	bool				m_bufferEnabled [renderer::shader::kVBufferSlotMaxCount];
	gpu::Buffer			m_indexBuffer;

	//uint16_t			m_boneCount;
	//gpu::Buffer			m_boneBuffer;

	arModelData*		m_modeldata;

	bool				m_mesh_uploaded;
};

#endif//RR_MESH_H_
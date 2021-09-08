#ifndef RENDERER_OBJECT_C_INSTANCED_MESH_H_
#define RENDERER_OBJECT_C_INSTANCED_MESH_H_

#include "renderer/object/mesh/Mesh.h"

namespace renderer
{
	// Example structure that can be used with InstacedMeshes.
	struct rrInstanceDataSimple
	{
		Vector4f position;
		Vector4f rotation;
	};

	//	class InstancedMesh
	// A InstancedMesh provides a mechanism to draw a single rrMeshBuffer with instancing.
	class InstancedMesh : public Mesh
	{
	public:
		RENDER_API explicit		InstancedMesh ( rrMeshBuffer*, bool n_enableSkinning=false );
		RENDER_API virtual		~InstancedMesh ( void );

	public:
		//RENDER_API void			SetInstancePosition ( int instance_id, const Vector3f& position );
		//RENDER_API void			SetInstanceRotation ( int instance_id, const Quaternion& rotation );
		//RENDER_API void			SetInstanceCount ( int instance_count );

		//	SetInstanceData( data, size ) : Sets new instance data.
		// Also tells the instanced mesh there's been an update, and must update the buffer again.
		RENDER_API void			SetInstanceData ( void* instanceData, const size_t dataSize );
		//	SetInstanceCount() : Sets new number of instances.
		RENDER_API void			SetInstanceCount ( const int instanceCount );

		//		Render
		// Called during engine render pass.
		// Overrides base class to forward the needed buffer forward to the material
		RENDER_API bool			Render ( const rrRenderParams* params ) override;

		//		EndRender
		// Pushes the instance data to the renderer at the end so the device has time to catch up
		//RENDER_API bool			EndRender ( void ) override;

		//		PrepRender
		// Pushes the instance data to the renderer before rendering starts.
		RENDER_API bool			PrepRender ( rrCameraPass* cameraPass ) override;

	protected:
		//glHandle	m_tbo_buffer;
		//glHandle	m_tex_buffer;
		gpu::Buffer			m_buffer;

		int					m_instanceCount;
		void*				m_data;
		size_t				m_dataSize;
		size_t				m_dataSizeAllocated;
		bool				m_newData;
		//int					data_count;
		//int					uploaded_data_count;
	};
}

#endif//RENDERER_OBJECT_C_INSTANCED_MESH_H_
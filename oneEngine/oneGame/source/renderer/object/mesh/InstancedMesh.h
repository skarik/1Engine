#ifndef RENDERER_OBJECT_C_INSTANCED_MESH_H_
#define RENDERER_OBJECT_C_INSTANCED_MESH_H_

#include "renderer/object/mesh/Mesh.h"

namespace renderer
{
	class InstancedMesh : public Mesh
	{
	public:
		RENDER_API explicit		InstancedMesh ( rrMeshBuffer*, bool n_enableSkinning=false );
		RENDER_API virtual		~InstancedMesh ( void );

	public:
		RENDER_API void			SetInstancePosition ( int instance_id, const Vector3d& position );
		RENDER_API void			SetInstanceRotation ( int instance_id, const Quaternion& rotation );

		RENDER_API void			SetInstanceCount ( int instance_count );

		//		Render
		// Called during engine render pass.
		// Overrides base class to forward the needed buffer forward to the material
		RENDER_API bool			Render ( const rrRenderParams* params ) override;

		//		EndRender
		// Pushes the instance data to the renderer at the end so the device has time to catch up
		RENDER_API bool			EndRender ( void ) override;

	protected:
		//glHandle	m_tbo_buffer;
		//glHandle	m_tex_buffer;
		gpu::Buffer			m_buffer;

		struct instanceData_t
		{
			Vector4d position;
			Vector4d rotation;
		};
		std::vector<instanceData_t>
							data;
		int					data_count;
		int					uploaded_data_count;
	};
}

#endif//RENDERER_OBJECT_C_INSTANCED_MESH_H_
#ifndef _C_INSTANCED_MESH_H_
#define _C_INSTANCED_MESH_H_

#include "renderer/object/mesh/CMesh.h"

class CInstancedMesh : public CMesh
{
public:
	RENDER_API explicit CInstancedMesh ( glMesh* );
	RENDER_API virtual ~CInstancedMesh ( void );

public:
	RENDER_API void SetInstancePosition ( int instance_id, const Vector3d& position );
	RENDER_API void SetInstanceRotation ( int instance_id, const Quaternion& rotation );

	RENDER_API void SetInstanceCount ( int instance_count );

	//		Render
	// Called during engine render pass.
	// Overrides base class to forward the needed buffer forward to the material
	RENDER_API bool Render ( const char pass ) override;

	//		EndRender
	// Pushes the instance data to the renderer at the end so the device has time to catch up
	RENDER_API bool EndRender ( void ) override;

protected:
	glHandle	m_tbo_buffer;
	glHandle	m_tex_buffer;

	struct instanceData_t
	{
		Vector4d position;
		Vector4d rotation;
	};
	std::vector<instanceData_t> data;
	int data_count;
	int uploaded_data_count;
};

#endif//_C_INSTANCED_MESH_H_
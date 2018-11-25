#include "CStreamedMesh.h"
#include "renderer/logic/model/RrCModel.h"
//#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMesh.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "renderer/camera/RrCamera.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"

renderer::StreamedMesh::StreamedMesh( void )
	: Mesh( NULL, false )
{
	bUseFrustumCulling = false; // Disable frustum culling on default for streaming meshes
}

renderer::StreamedMesh::~StreamedMesh ( void )
{
	delete_safe( m_mesh );
}

// Return access to model data
arModelData* renderer::StreamedMesh::GetModelData ( void )
{
	if ( m_mesh == NULL )
	{
		arModelData* modeldata = new arModelData;
		modeldata->triangleNum = 0;
		modeldata->triangles = NULL;
		modeldata->vertexNum = 0;
		modeldata->vertices = NULL;

		m_mesh = new rrMesh;
		m_mesh->Initialize( "Stream mesh instance", modeldata, 1, true );
	}
	// Return mesh data directly
	return m_mesh->modeldata;
}

// Push the current stuff in model data to GPU.
void renderer::StreamedMesh::StreamLockModelData ( void )
{
	PushModeldata();
}

// Push mesh
void renderer::StreamedMesh::PushModeldata ( void )
{
	if ( m_mesh )
	{
		m_mesh->StreamMeshBuffers(modeldata);
	}
}
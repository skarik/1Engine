
#include "CStreamedMesh.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/rrMesh.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"

CStreamedMesh::CStreamedMesh ( void )
	: CMesh( NULL, false )
{
	bUseFrustumCulling = false; // Disable frustum culling on default for streaming meshes
}

CStreamedMesh::~CStreamedMesh ( void )
{
	delete_safe( m_mesh );
}

// Return access to model data
arModelData* CStreamedMesh::GetModelData ( void )
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
void CStreamedMesh::StreamLockModelData ( void )
{
	PushModeldata();
}

// Push mesh
void CStreamedMesh::PushModeldata ( void )
{
	if ( m_mesh )
	{
		m_mesh->Restream();
	}
}
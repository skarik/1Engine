
#include "CStreamedMesh.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/object/mesh/system/glMesh.h"
#include "renderer/object/mesh/system/glSkinnedMesh.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"

CStreamedMesh::CStreamedMesh ( void )
	: CMesh( NULL, false )
{
	bUseFrustumCulling = false; // Disable frustum culling on default for streaming meshes
}

CStreamedMesh::~CStreamedMesh ( void )
{
	delete_safe( m_glMesh );
}

// Return access to model data
CModelData* CStreamedMesh::GetModelData ( void )
{
	if ( m_glMesh == NULL )
	{
		CModelData* modeldata = new CModelData;
		modeldata->triangleNum = 0;
		modeldata->triangles = NULL;
		modeldata->vertexNum = 0;
		modeldata->vertices = NULL;

		m_glMesh = new glMesh;
		m_glMesh->Initialize( "Stream mesh instance", modeldata, 1, true );
	}
	// Return mesh data directly
	return m_glMesh->pmData;
}

// Push the current stuff in model data to GPU.
void CStreamedMesh::StreamLockModelData ( void )
{
	PushModeldata();
}

// Push mesh
void CStreamedMesh::PushModeldata ( void )
{
	if ( m_glMesh )
	{
		m_glMesh->Restream();
	}
}
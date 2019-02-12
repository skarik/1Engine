#include "rrMeshBuffer.h"
#include "core-ext/utils/MeshAttributes.h"
//#include "renderer/material/RrMaterial.h"
//#include "renderer/system/glMainSystem.h"

rrMeshBuffer::rrMeshBuffer ( void )
	: m_bufferEnabled(), /*m_boneCount(0),*/ m_modeldata(NULL), 
	m_mesh_uploaded(false)/*, m_name()*/
{
	;
}
rrMeshBuffer::~rrMeshBuffer ( void )
{
	// Free any previous mesh
	if (m_mesh_uploaded)
	{
		FreeMeshBuffers();
	}
}

void rrMeshBuffer::InitMeshBuffers ( arModelData* const modelData )
{
	// Free any previous mesh
	if (m_mesh_uploaded)
	{
		FreeMeshBuffers();
	}

	// assign model data!
	m_modeldata = modelData;

	// upload needed buffers:

	if (m_modeldata->position) {
		m_buffer[renderer::shader::kVBufferSlotPosition].initAsVertexBuffer(NULL, gpu::kFormatR32G32B32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotPosition].uploadElements(NULL, m_modeldata->position, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotPosition] = true;
	}
	if (m_modeldata->texcoord0) {
		m_buffer[renderer::shader::kVBufferSlotUV0].initAsVertexBuffer(NULL, gpu::kFormatR32G32B32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotUV0].uploadElements(NULL, m_modeldata->texcoord0, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotUV0] = true;
	}
	if (m_modeldata->color) {
		m_buffer[renderer::shader::kVBufferSlotColor].initAsVertexBuffer(NULL, gpu::kFormatR32G32B32A32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotColor].uploadElements(NULL, m_modeldata->color, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotColor] = true;
	}
	if (m_modeldata->normal) {
		m_buffer[renderer::shader::kVBufferSlotNormal].initAsVertexBuffer(NULL, gpu::kFormatR32G32B32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotNormal].uploadElements(NULL, m_modeldata->normal, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotNormal] = true;
	}
	if (m_modeldata->tangent) {
		m_buffer[renderer::shader::kVBufferSlotTangent].initAsVertexBuffer(NULL, gpu::kFormatR32G32B32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotTangent].uploadElements(NULL, m_modeldata->tangent, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotTangent] = true;
	}
	if (m_modeldata->binormal) {
		m_buffer[renderer::shader::kVBufferSlotBinormal].initAsVertexBuffer(NULL, gpu::kFormatR32G32B32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotBinormal].uploadElements(NULL, m_modeldata->binormal, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotBinormal] = true;
	}
	if (m_modeldata->weight) {
		m_buffer[renderer::shader::kVBufferSlotBoneWeight].initAsVertexBuffer(NULL, gpu::kFormatR32G32B32A32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotBoneWeight].uploadElements(NULL, m_modeldata->weight, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotBoneWeight] = true;
	}
	if (m_modeldata->bone) {
		m_buffer[renderer::shader::kVBufferSlotBoneIndices].initAsVertexBuffer(NULL, gpu::kFormatR16G16B16A16UInteger, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotBoneIndices].uploadElements(NULL, m_modeldata->bone, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotBoneIndices] = true;
	}
	if (m_modeldata->texcoord1) {
		m_buffer[renderer::shader::kVBufferSlotUV1].initAsVertexBuffer(NULL, gpu::kFormatR32G32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotUV1].uploadElements(NULL, m_modeldata->texcoord1, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotUV1] = true;
	}
	if (m_modeldata->texcoord2) {
		m_buffer[renderer::shader::kVBufferSlotMaxPosition + 0].initAsVertexBuffer(NULL, gpu::kFormatR32G32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotMaxPosition + 0].uploadElements(NULL, m_modeldata->texcoord2, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotMaxPosition + 0] = true;
	}
	if (m_modeldata->texcoord3) {
		m_buffer[renderer::shader::kVBufferSlotMaxPosition + 1].initAsVertexBuffer(NULL, gpu::kFormatR32G32SFloat, m_modeldata->vertexNum);
		m_buffer[renderer::shader::kVBufferSlotMaxPosition + 1].upload(NULL, m_modeldata->texcoord3, m_modeldata->vertexNum, gpu::kTransferStatic);
		m_bufferEnabled[renderer::shader::kVBufferSlotMaxPosition + 1] = true;
	}

	// upload index buffer:

	if (m_modeldata->indexNum > 0)
	{
		size_t buffer_size = sizeof(uint16_t) * m_modeldata->indexNum;
		m_indexBuffer.initAsIndexBuffer(NULL, gpu::kIndexFormatUnsigned16, m_modeldata->indexNum);
		void* buffer = m_indexBuffer.map(NULL, gpu::kTransferStatic);
		if (buffer != NULL)
		{
			memcpy(buffer, m_modeldata->indices, buffer_size);
			m_indexBuffer.unmap(NULL);
		}
	}

	// we have a fuckin mesh
	m_mesh_uploaded = true;
}

void rrMeshBuffer::FreeMeshBuffers ( void )
{
	for (int i = 0; i < renderer::shader::kVBufferSlotMaxPosition; ++i)
	{
		m_buffer[i].free(NULL);
		m_bufferEnabled[i] = false;
	}
	m_indexBuffer.free(NULL);

	// we no longer have a fuckin mesh
	m_mesh_uploaded = false;
}

void rrMeshBuffer::StreamMeshBuffers ( arModelData* const modelData )
{
	InitMeshBuffers(modelData);
}

#if 0
// Constructor
rrMesh::rrMesh ( void )
	: name("unnamed")
{
	bReady	= false;
	bShaderSetup = false;

	iVBOverts	= 0;
	iVBOfaces	= 0;

	modeldata	= NULL;

	userdata		= NULL;
	userdata_type	= kUserdataTypeNone;
}

// Destructor
rrMesh::~rrMesh ( void )
{
	// We gotta free the model
	FreeVBOData();
	FreeRAMData();

	// Based on the ibDataType, also free that
	if ( userdata_type == kUserdataTypeString ) 
	{	// String data
		delete[] ((char*)userdata);
		userdata = NULL;
		userdata_type = kUserdataTypeNone;
	}
}

const char* rrMesh::GetName ( void ) const
{
	return name.c_str();
}


// Create a new VBO associated with this rrMesh
// Removes any old VBO data.
// This object gains ownership of the model data. Well, it should, or you'll get memory errors and shit. And that's bad.
void rrMesh::Initialize ( const char* nNewName, arModelData* const pNewModelData, unsigned int frames, bool willStream )
{
	// Ensure only single tpye of frame created
	if (frames != 0 && frames != 1)
	{
		throw core::NotYetImplementedException();
	}

	name = nNewName;

	// Free the old data, duh
	FreeVBOData ( );

	modeldata = pNewModelData;

	glBindVertexArray( 0 );

	// Create new buffers
	glGenBuffers( 1, &iVBOverts );
	glGenBuffers( 1, &iVBOfaces );
	bShaderSetup = false; // With making new buffers, shader is now not ready

	// Bind to some buffer objects
	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes

	// Copy data to the buffer
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(arModelVertex) * (modeldata->vertexNum),
		modeldata->vertices,
		willStream ? GL_STREAM_DRAW : GL_STATIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(arModelTriangle) * (modeldata->triangleNum),
		modeldata->triangles,
		willStream ? GL_STREAM_DRAW : GL_STATIC_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
// Sends new VBO data to the mesh
void rrMesh::Restream ( void )
{
	// Bind to some buffer objects
	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes

	// Tell data to be discarded
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(arModelVertex) * (modeldata->vertexNum),
		NULL,
		GL_STREAM_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(arModelTriangle) * (modeldata->triangleNum),
		NULL,
		GL_STREAM_DRAW );

	// Copy data to the buffer
	void* memoryData;
	size_t bufferSize;

	bufferSize = sizeof(arModelVertex) * (modeldata->vertexNum);
	memoryData = glMapBufferRange( GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( memoryData ) {
		memcpy( memoryData, modeldata->vertices, bufferSize );
	}
	bufferSize = sizeof(arModelTriangle) * (modeldata->triangleNum);
	memoryData = glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
	if ( memoryData ) {
		memcpy( memoryData, modeldata->triangles, bufferSize );
	}
	
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );


	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

// Reputs the data into the VBO
void rrMesh::Reinitialize ( void )
{
	Initialize( name, modeldata, 1 );
}
// Recalculates normals for all meshes
void rrMesh::RecalculateNormals ( void )
{
	core::mesh::RecalculateNormals( modeldata );
	Reinitialize();
}
// Recalculates the tangents and binormals
void rrMesh::RecalculateTangents ( void )
{
	core::mesh::RecalculateTangents( modeldata );
	Reinitialize();
}

// == Private Routines ==
void rrMesh::FreeVBOData ( void )
{
	// If the mesh is ready to render, then we gotta free the model
	if ( bReady )
	{
		bReady = false;
		bShaderSetup = false;

		if ( iVBOverts )
			glDeleteBuffers( 1, &iVBOverts );
		iVBOverts = 0;
		if ( iVBOfaces )
			glDeleteBuffers( 1, &iVBOfaces );
		iVBOfaces = 0;
	}
	// Otherwise, do nothing really
}

void rrMesh::FreeRAMData ( void )
{
	if ( modeldata != NULL )
	{
		// Delete the data
		delete[] modeldata->triangles;
		delete[] modeldata->vertices;

		// Delete the mesh containers
		delete modeldata;

		// Set final result
		modeldata = NULL;
	}
}
#endif
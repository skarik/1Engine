#include "IrrMeshBuilder.h"

#include <stdlib.h>
#include <algorithm>
#include <atomic>

struct rrModelDataEntry
{
	arModelData data;
	std::atomic_flag in_use;
};
static rrModelDataEntry l_modelDatas [4] = {};

//	Constructor (new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
IrrMeshBuilder::IrrMeshBuilder ( const uint16_t estimatedVertexCount )
	: m_vertexCount(0), m_indexCount(0), m_model(NULL), m_model_isFromPool(true),
	m_enabledAttribs()
{
	// Loop through the pool to find an unused model.

	for (uint i = 0; i < 4; ++i)
	{
		if (!l_modelDatas[i].in_use.test_and_set())
		{
			m_model = &l_modelDatas[i].data;
			break;
		}
	}
	ARCORE_ASSERT(m_model != NULL);
	m_model_isFromPool = true;

	expand(estimatedVertexCount);
	expandIndices((estimatedVertexCount * 3) / 2);

	enableAttribute(renderer::shader::kVBufferSlotPosition);
}
//	Constructor (existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
IrrMeshBuilder::IrrMeshBuilder ( arModelData* preallocatedModelData, uint16_t initialVertexCount, uint16_t initialIndexCount )
	: m_vertexCount(initialVertexCount), m_indexCount(initialIndexCount), m_model(preallocatedModelData), m_model_isFromPool(false),
	m_enabledAttribs()
{
	; // Nothing needed to be done here. :)
	enableAttribute(renderer::shader::kVBufferSlotPosition);
}

//	Destructor ()
// Frees internal storage for reuse.
IrrMeshBuilder::~IrrMeshBuilder ( void )
{
	if (m_model_isFromPool)
	{
		// Free the pool model.
		for (uint i = 0; i < 4; ++i)
		{
			if (m_model == &l_modelDatas[i].data)
			{
				l_modelDatas[i].in_use.clear();
				break;
			}
		}
	}
}

//	getModelData () : Returns the current model data.
// The sizes in the model data are filled
arModelData IrrMeshBuilder::getModelData ( void ) const
{
	arModelData newData = *m_model;
	newData.vertexNum = m_vertexCount;
	newData.indexNum  = m_indexCount;
	return newData;
}

//	getModelDataVertexCount () : Returns the current vertex count.
// This is the value that would fill in for the vertex count in getModelData().
uint16_t IrrMeshBuilder::getModelDataVertexCount ( void ) const
{
	return m_vertexCount;
}

//	getModelDataIndexCount () : Returns the current vertex count.
// This is the value that would fill in for the index count in getModelData().
uint16_t IrrMeshBuilder::getModelDataIndexCount ( void ) const
{
	return m_indexCount;
}

//	enableAttribute ( attrib ) : Enables storage for the given attribute
void IrrMeshBuilder::enableAttribute( renderer::shader::VBufferSlot attrib )
{
	m_enabledAttribs[attrib] = true;
}

//	expand ( new vertex count ) : Ensure storage is large enough to hold the count
void IrrMeshBuilder::expand ( const uint16_t vertexCount )
{
	bool nulled_attribute = false;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotPosition])
		if (m_model->position == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV0])
		if (m_model->texcoord0 == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotColor])
		if (m_model->color == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotNormal])
		if (m_model->normal == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBinormal])
		if (m_model->binormal == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV1])
		if (m_model->texcoord1 == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBoneWeight])
		if (m_model->weight == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBoneIndices])
		if (m_model->bone == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV2])
		if (m_model->texcoord2 == NULL) nulled_attribute = true;
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV3])
		if (m_model->texcoord3 == NULL) nulled_attribute = true;

	if (nulled_attribute || m_model->vertexNum < vertexCount)
	{
		if (m_model_isFromPool) {
			reallocateGreedy(vertexCount);
		}
		else {
			reallocateConservative(vertexCount);
		}
	}
}

template <typename T>
FORCE_INLINE static T* reallocate ( T* old_data, const size_t old_count, const size_t new_count )
{
	T* new_data = new T [new_count];
	if (old_data != NULL)
	{
		memcpy(new_data, old_data, old_count * sizeof(T));
		delete[] old_data;
	}
	return new_data;
}

//	reallocateGreedy ( new size ) : Expands vertex storage in a greedy way
void IrrMeshBuilder::reallocateGreedy ( uint16_t targetSize )
{
	uint16_t old_count = m_model->vertexNum;

	// Increase the size by 150%.
	while (m_model->vertexNum < targetSize)
	{
		m_model->vertexNum = (uint16_t)std::min<Real>(m_model->vertexNum * 1.5F + 1.0F, 65535.0F);
	}

	if (m_enabledAttribs[renderer::shader::kVBufferSlotPosition])
		m_model->position = reallocate(m_model->position, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV0])
		m_model->texcoord0 = reallocate(m_model->texcoord0, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotColor])
		m_model->color = reallocate(m_model->color, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotNormal])
		m_model->normal = reallocate(m_model->normal, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotTangent])
		m_model->tangent = reallocate(m_model->tangent, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBinormal])
		m_model->binormal = reallocate(m_model->binormal, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV1])
		m_model->texcoord1 = reallocate(m_model->texcoord1, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBoneWeight])
		m_model->weight = reallocate(m_model->weight, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBoneIndices])
		m_model->bone = reallocate(m_model->bone, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV2])
		m_model->texcoord2 = reallocate(m_model->texcoord2, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV3])
		m_model->texcoord3 = reallocate(m_model->texcoord3, old_count, m_model->vertexNum);
}

//	reallocateWild ( new size ) : Expands vertex storage in a conservative way
void IrrMeshBuilder::reallocateConservative ( uint16_t targetSize )
{
	uint16_t old_count = m_model->vertexNum;
	
	// Increase the size to just above the target.
	while (m_model->vertexNum < targetSize)
	{
		m_model->vertexNum = targetSize + m_model->vertexNum / 1024 + 1;
	}
	
	if (m_enabledAttribs[renderer::shader::kVBufferSlotPosition])
		m_model->position = reallocate(m_model->position, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV0])
		m_model->texcoord0 = reallocate(m_model->texcoord0, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotColor])
		m_model->color = reallocate(m_model->color, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotNormal])
		m_model->normal = reallocate(m_model->normal, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotTangent])
		m_model->tangent = reallocate(m_model->tangent, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBinormal])
		m_model->binormal = reallocate(m_model->binormal, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV1])
		m_model->texcoord1 = reallocate(m_model->texcoord1, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBoneWeight])
		m_model->weight = reallocate(m_model->weight, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotBoneIndices])
		m_model->bone = reallocate(m_model->bone, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV2])
		m_model->texcoord2 = reallocate(m_model->texcoord2, old_count, m_model->vertexNum);
	if (m_enabledAttribs[renderer::shader::kVBufferSlotUV3])
		m_model->texcoord3 = reallocate(m_model->texcoord3, old_count, m_model->vertexNum);

}


//	expandTri ( new vertex count ) : Ensure storage is large enough to hold the count of triangles
void IrrMeshBuilder::expandIndices ( const uint16_t indiciesCount )
{
	if (m_model->indexNum < indiciesCount)
	{
		if (m_model_isFromPool) {
			reallocateIndicesGreedy(indiciesCount);
		}
		else {
			reallocateIndicesConservative(indiciesCount);
		}
	}
}

//	reallocateTriGreedy ( new size ) : Expands triangle storage in a greedy way
void IrrMeshBuilder::reallocateIndicesGreedy ( uint16_t targetSize )
{
	uint16_t old_count = m_model->indexNum;
	uint16_t* old_indices = m_model->indices;

	// Increase the size by 150%.
	while (m_model->indexNum < targetSize)
	{
		m_model->indexNum = (uint16_t)std::min<Real>(m_model->indexNum * 1.5F + 1.0F, 65535.0F);
	}

	m_model->indices = new uint16_t[m_model->indexNum];
	memcpy(m_model->indices, old_indices, sizeof(uint16_t) * old_count);
	delete[] old_indices;
}

//	reallocateTriWild ( new size ) : Expands triangle storage in a conservative way
void IrrMeshBuilder::reallocateIndicesConservative ( uint16_t targetSize )
{
	uint16_t old_count = m_model->indexNum;
	uint16_t* old_indices = m_model->indices;

	// Increase the size to just above the target.
	while (m_model->indexNum < targetSize)
	{
		m_model->indexNum = targetSize + m_model->indexNum / 1024 + 1;
	}

	m_model->indices = new uint16_t[m_model->indexNum];
	memcpy(m_model->indices, old_indices, sizeof(uint16_t) * old_count);
	delete[] old_indices;
}
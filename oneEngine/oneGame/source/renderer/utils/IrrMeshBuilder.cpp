#include "IrrMeshBuilder.h"

#include <stdlib.h>
#include <algorithm>

struct rrModelDataEntry
{
	arModelData data;
	bool in_use;
};
static rrModelDataEntry l_modelDatas [4] = {};

//	Constructor (new data)
// Pulls a model from the the pool that has at least the estimated input size.
// If the estimation is incorrect, the data will be resized.
IrrMeshBuilder::IrrMeshBuilder ( const uint16_t estimatedVertexCount )
	: m_vertexCount(0), m_indexCount(0), m_model(NULL), m_model_isFromPool(true)
{
	// Loop through the pool to find an unused model.

	for (uint i = 0; i < 4; ++i)
	{
		if (l_modelDatas[i].in_use == false)
		{
			m_model = &l_modelDatas[i].data;
			l_modelDatas[i].in_use = true;
			break;
		}
	}
	m_model_isFromPool = true;

	expand(estimatedVertexCount);
	expandIndices((estimatedVertexCount * 3) / 2);
}
//	Constructor (existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
IrrMeshBuilder::IrrMeshBuilder ( arModelData* preallocatedModelData )
	: m_vertexCount(0), m_indexCount(0), m_model(preallocatedModelData), m_model_isFromPool(false)
{
	; // Nothing needed to be done here. :)
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
				l_modelDatas[i].in_use = false;
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

//	expand ( new vertex count ) : Ensure storage is large enough to hold the count
void IrrMeshBuilder::expand ( const uint16_t vertexCount )
{
	if (m_model->vertexNum < vertexCount)
	{
		if (m_model_isFromPool) {
			reallocateGreedy(vertexCount);
		}
		else {
			reallocateConservative(vertexCount);
		}
	}
}

//	reallocateGreedy ( new size ) : Expands vertex storage in a greedy way
void IrrMeshBuilder::reallocateGreedy ( uint16_t targetSize )
{
	uint16_t old_count = m_model->vertexNum;
	arModelVertex* old_vertices = m_model->vertices;

	// Increase the size by 150%.
	while (m_model->vertexNum < targetSize)
	{
		m_model->vertexNum = (uint16_t)std::min<Real>(m_model->vertexNum * 1.5F + 1.0F, 65535.0F);
	}

	m_model->vertices = new arModelVertex[m_model->vertexNum];
	memcpy(m_model->vertices, old_vertices, old_count * sizeof(arModelVertex));
	delete[] old_vertices;
}

//	reallocateWild ( new size ) : Expands vertex storage in a conservative way
void IrrMeshBuilder::reallocateConservative ( uint16_t targetSize )
{
	uint16_t old_count = m_model->vertexNum;
	arModelVertex* old_vertices = m_model->vertices;
	
	// Increase the size to just above the target.
	while (m_model->vertexNum < targetSize)
	{
		m_model->vertexNum = targetSize + m_model->vertexNum / 1024 + 1;
	}
	
	m_model->vertices = new arModelVertex[m_model->vertexNum];
	memcpy(m_model->vertices, old_vertices, old_count * sizeof(arModelVertex));
	delete[] old_vertices;
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
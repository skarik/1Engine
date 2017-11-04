#include "IrrMeshBuilder.h"

#include <stdlib.h>
#include <algorithm>

//static arModelData l_modelDataTemp = {};
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
	: m_vertexCount(0), m_triangleCount(0), m_model(NULL), m_model_isFromPool(true)
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
	expandTri(estimatedVertexCount / 2);
}
//	Constructor (existing data)
// Sets up model, using the input data.
// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
IrrMeshBuilder::IrrMeshBuilder ( arModelData* preallocatedModelData )
	: m_vertexCount(0), m_triangleCount(0), m_model(preallocatedModelData), m_model_isFromPool(false)
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
	newData.triangleNum = m_triangleCount;
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
void IrrMeshBuilder::expandTri ( const uint16_t triangleCount )
{
	if (m_model->triangleNum < triangleCount)
	{
		if (m_model_isFromPool) {
			reallocateTriGreedy(triangleCount);
		}
		else {
			reallocateTriConservative(triangleCount);
		}
	}
}

//	reallocateTriGreedy ( new size ) : Expands triangle storage in a greedy way
void IrrMeshBuilder::reallocateTriGreedy ( uint16_t targetSize )
{
	uint16_t old_count = m_model->triangleNum;
	arModelTriangle* old_triangles = m_model->triangles;

	// Increase the size by 150%.
	while (m_model->triangleNum < targetSize)
	{
		m_model->triangleNum = (uint16_t)std::min<Real>(m_model->triangleNum * 1.5F + 1.0F, 65535.0F);
	}

	m_model->triangles = new arModelTriangle[m_model->triangleNum];
	memcpy(m_model->triangles, old_triangles, old_count * sizeof(arModelTriangle));
	delete[] old_triangles;
}

//	reallocateTriWild ( new size ) : Expands triangle storage in a conservative way
void IrrMeshBuilder::reallocateTriConservative ( uint16_t targetSize )
{
	uint16_t old_count = m_model->triangleNum;
	arModelTriangle* old_triangles = m_model->triangles;

	// Increase the size to just above the target.
	while (m_model->triangleNum < targetSize)
	{
		m_model->triangleNum = targetSize + m_model->triangleNum / 1024 + 1;
	}

	m_model->triangles = new arModelTriangle[m_model->triangleNum];
	memcpy(m_model->triangles, old_triangles, old_count * sizeof(arModelTriangle));
	delete[] old_triangles;
}
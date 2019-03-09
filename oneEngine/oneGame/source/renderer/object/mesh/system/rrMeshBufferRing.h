#ifndef RR_MESH_RING_H_
#define RR_MESH_RING_H_

#include "core/containers/arring.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

//	rrMeshBufferRing : Class for N-buffered mesh buffers.
// The buffer that will render trails behind the buffer being edited, which maximizes the time the buffer hangs around after being used in a render call.
template <unsigned short Ln>
class rrMeshBufferRing
{
public:
	static_assert(Ln >= 2,
				"rrMeshBufferRing has a minimum length of two.");

	explicit				rrMeshBufferRing ( void )
		: m_ring(false), m_meshToRender(NULL), m_meshToEdit(NULL)
	{
		m_meshToRender = &m_ring.get();
		m_ring.increment();
		m_meshToEdit = &m_ring.get();
	}

	//	freeMeshBuffers() : Frees all mesh buffers
	void					freeMeshBuffers ( void )
	{
		for (unsigned short i = 0; i < Ln; ++i)
		{
			m_ring.at(i).FreeMeshBuffers();
		}
	}

	//	incrementAfterEdit() : steps through the ring
	void					incrementAfterEdit ( void )
	{
		m_ring.increment();
		m_meshToRender = m_meshToEdit; // Render the mesh that was under edit
		m_meshToEdit = &m_ring.get();
	}

	//	getToRender() : returns the buffer under render
	rrMeshBuffer*			getToRender ( void )
	{
		return m_meshToRender;
	}

	//	getToEdit() : returns the buffer under edit
	rrMeshBuffer*			getToEdit ( void )
	{
		return m_meshToEdit;
	}

	//	size() : returns the length of the ring
	size_t					size ( void )
	{
		return Ln;
	}

public:
	arring<rrMeshBuffer, Ln>
						m_ring;
	rrMeshBuffer*		m_meshToRender;
	rrMeshBuffer*		m_meshToEdit;
};

//	rrRing : Class for N-buffered generalist buffers.
// The buffer that will render trails behind the buffer being edited, which maximizes the time the buffer hangs around after being used in a render call.
template <typename OBJECT, unsigned short Ln>
class rrRing
{
public:
	static_assert(Ln >= 2,
				"rrRing has a minimum length of two.");

	explicit				rrRing ( void )
		: m_ring(false), m_objectToRender(NULL), m_objectToEdit(NULL)
	{
		m_objectToRender = &m_ring.get();
		m_ring.increment();
		m_objectToEdit = &m_ring.get();
	}

	//	freeMeshBuffers() : Frees all mesh buffers
	void					free ( void )
	{
		for (unsigned short i = 0; i < Ln; ++i)
		{
			m_ring.at(i).free(NULL);
		}
	}

	//	incrementAfterEdit() : steps through the ring
	void					incrementAfterEdit ( void )
	{
		m_ring.increment();
		m_objectToRender = m_objectToEdit; // Render the mesh that was under edit
		m_objectToEdit = &m_ring.get();
	}

	//	getToRender() : returns the buffer under render
	rrMeshBuffer*			getToRender ( void )
	{
		return m_objectToRender;
	}

	//	getToEdit() : returns the buffer under edit
	rrMeshBuffer*			getToEdit ( void )
	{
		return m_objectToEdit;
	}

	//	size() : returns the length of the ring
	size_t					size ( void )
	{
		return Ln;
	}

public:
	arring<OBJECT, Ln>	m_ring;
	OBJECT*				m_objectToRender;
	OBJECT*				m_objectToEdit;
};


#endif//RR_MESH_RING_H_
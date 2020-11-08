#ifndef RENDERER_UTILS_I_RR_MESH_BUILDER_H_
#define RENDERER_UTILS_I_RR_MESH_BUILDER_H_

#include "core/types/types.h"
#include "core/types/ModelData.h"
#include "renderer/types/renderModes.h"
#include "renderer/types/shaders/vattribs.h"

class IrrMeshBuilder
{
public:
	//	Constructor (new data)
	// Pulls a model from the the pool that has at least the estimated input size.
	// If the estimation is incorrect, the data will be resized.
	RENDER_API explicit		IrrMeshBuilder ( const uint16_t estimatedVertexCount );
	//	Constructor (existing data)
	// Sets up model, using the input data.
	// As above, will re-allocate if the data is small, but will do so extremely conservatively (slowly).
	RENDER_API explicit		IrrMeshBuilder ( arModelData* preallocatedModelData, uint16_t initialVertexCount = 0, uint16_t initialIndexCount = 0 );

	//	Destructor ()
	// Frees internal storage for reuse.
	RENDER_API virtual		~IrrMeshBuilder ( void );

	//	getModelData () : Returns the current model data.
	// The sizes in the model data are filled with the correct sizes.
	// The model data itself still belongs to the mesh builder, so should not be freed by the user.
	RENDER_API arModelData	getModelData ( void ) const;
	//	getModelDataVertexCount () : Returns the current vertex count.
	// This is the value that would fill in for the vertex count in getModelData().
	// The vertex count can be useful for patching in data that is not written by the builder.
	RENDER_API uint16_t		getModelDataVertexCount ( void ) const;
	//	getModelDataIndexCount () : Returns the current vertex count.
	// This is the value that would fill in for the index count in getModelData().
	// The index count can be useful for patching in data that is not written by the builder.
	RENDER_API uint16_t		getModelDataIndexCount ( void ) const;

	//	getPrimitiveMode () : returns the primitive mode this mesh builder would like to render in.
	// Must be implemented by child classes.
	RENDER_API virtual renderer::rrPrimitiveMode
							getPrimitiveMode ( void ) const = 0;

	//	enableAttribute ( attrib ) : Enables storage for the given attribute
	RENDER_API void			enableAttribute( renderer::shader::VBufferSlot attrib );

protected:
	//	expand ( new vertex count ) : Ensure storage is large enough to hold the count
	RENDER_API void			expand ( const uint16_t vertexCount );
	//	reallocateGreedy ( new size ) : Expands vertex storage in a greedy way
	RENDER_API void			reallocateGreedy ( uint16_t targetSize );
	//	reallocateWild ( new size ) : Expands vertex storage in a conservative way
	RENDER_API void			reallocateConservative ( uint16_t targetSize );

	//	expandIndices ( new triangle count ) : Ensure storage is large enough to hold the count
	RENDER_API void			expandIndices ( const uint16_t indiciesCount );
	//	reallocateIndicesGreedy ( new size ) : Expands triangle storage in a greedy way
	RENDER_API void			reallocateIndicesGreedy ( uint16_t targetSize );
	//	reallocateIndicesConservative ( new size ) : Expands triangle storage in a conservative way
	RENDER_API void			reallocateIndicesConservative ( uint16_t targetSize );

protected:
	// Current vertex count of the model.
	uint16_t		m_vertexCount;
	// Current index count of the model.
	uint16_t		m_indexCount;
	// Pointer to the model structure and the array sizes.
	arModelData*	m_model;
	// Did the model come from outside or from the internal model pool?
	bool			m_model_isFromPool;

	// Enabled attributes:
	bool			m_enabledAttribs [renderer::shader::kVBufferSlotMaxCount];
};

#endif//RENDERER_UTILS_I_RR_MESH_BUILDER_H_
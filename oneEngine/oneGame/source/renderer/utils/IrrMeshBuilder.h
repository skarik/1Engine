#ifndef RENDERER_UTILS_I_RR_MESH_BUILDER_H_
#define RENDERER_UTILS_I_RR_MESH_BUILDER_H_

#include "core/types/types.h"
#include "core/types/ModelData.h"
#include "renderer/types/renderModes.h"

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
	RENDER_API explicit		IrrMeshBuilder ( arModelData* preallocatedModelData );

	//	Destructor ()
	// Frees internal storage for reuse.
	RENDER_API virtual		~IrrMeshBuilder ( void );

	//	getModelData () : Returns the current model data.
	// The sizes in the model data are filled
	RENDER_API arModelData	getModelData ( void ) const;

	//	getPrimitiveMode () : returns the primitive mode this mesh builder would like to render in.
	// Must be implemented by child classes.
	RENDER_API virtual renderer::rrPrimitiveMode
							getPrimitiveMode ( void ) const = 0;

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
};

#endif//RENDERER_UTILS_I_RR_MESH_BUILDER_H_
#ifndef RENDERER_MESHMODEL_LOAD_REQUEST_H_
#define RENDERER_MESHMODEL_LOAD_REQUEST_H_

#include "core/types/types.h"

class RrAnimatedMeshGroup;

//	rrModelLoadParams : Parameter struct for loading models.
// Used for when only parts of the model data is needed.
struct rrModelLoadParams
{
	const char*			resource_name = nullptr;
	bool				geometry = true;
	bool				morphs = false;
	bool				animation = false;
	bool				skeleton = false;
	bool				collision = false;
};

namespace renderer
{
	// Load in a mesh group.
	RENDER_API RrAnimatedMeshGroup*
							LoadMeshGroup ( const rrModelLoadParams& load_params );
};

#endif//RENDERER_MESHMODEL_LOAD_REQUEST_H_
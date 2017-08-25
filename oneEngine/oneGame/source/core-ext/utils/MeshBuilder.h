#ifndef _CORE_EXTENSION_UTILS_MESH_BUILDER_H_
#define _CORE_EXTENSION_UTILS_MESH_BUILDER_H_

#include "core/types/types.h"
#include "core/types/ModelData.h"
#include "core/math/Rect.h"

namespace core
{
	namespace meshbuilder
	{
		CORE_API void Quad ( arModelData* md, const Rect& position, const Rect& uvs );
	}
}

#endif//_CORE_EXTENSION_UTILS_MESH_BUILDER_H_
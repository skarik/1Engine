#ifndef _CORE_EXTENSION_UTILS_MESH_BUILDER_H_
#define _CORE_EXTENSION_UTILS_MESH_BUILDER_H_

#include "core/types/types.h"
#include "core/types/ModelData.h"
#include "core/math/Rect.h"
#include "core/math/Cubic.h"
#include "core/math/Color.h"

namespace core
{
	namespace meshbuilder
	{
		//	Quad() : Adds a 2D quad to the given modeldata
		// Vertices are in counter-clockwise order around the quad edge.
		CORE_API void Quad ( arModelData* md, const Rect& position, const Rect& uvs );

		//	Quad() : Adds a 3D quad to the given modeldata
		// Vertices are in counter-clockwise order around the quad edge.
		CORE_API void Quad ( arModelData* md, const Vector3f* positions, const Color& color, const Rect& uvs );
	}
}

#endif//_CORE_EXTENSION_UTILS_MESH_BUILDER_H_
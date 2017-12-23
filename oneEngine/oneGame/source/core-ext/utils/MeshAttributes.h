//===============================================================================================//
//
//		core/utils/MeshAttributes.h
//
// Tool for helping build commonly made meshes.
//
//===============================================================================================//
#ifndef CORE_EXTENSION_UTILS_MESH_ATTRIBUTES_H_
#define CORE_EXTENSION_UTILS_MESH_ATTRIBUTES_H_

#include "core/types/types.h"
#include "core/types/ModelData.h"
#include "core/math/Rect.h"
#include "core/math/Cubic.h"
#include "core/math/Color.h"

namespace core
{
	namespace mesh
	{
		//	RecalculateNormals(model) : Recalculates the normals per-triangle.
		// Internally, uses a lot of useless information.
		CORE_API void RecalculateNormals ( arModelData* md );
		//	RecalculateTangents(model) : Recalculates the tangent and binormals.
		// Trashes existing tangent and binormal data as they are used for temporary data.
		CORE_API void RecalculateTangents ( arModelData* md );
	}
}

#endif//CORE_EXTENSION_UTILS_MESH_ATTRIBUTES_H_
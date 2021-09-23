#ifndef RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_
#define RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_

#include "core/types/arBaseObject.h"
#include "core/containers/arstring.h"
#include "core/math/Cubic.h"
#include "core-ext/resources/IArResource.h"
#include "core-ext/animation/Skeleton.h"
#include "physical/material/ArMaterial.h"

#include <vector>

class CMorpherSet;
class rrMeshBuffer;

class RrAnimatedMeshGroup : public arBaseObject, public IArResource
{
public:
	//	virtual ResourceType() : What type of resource is this?
	// Identifies the type of resource this is.
	RENDER_API core::arResourceType 
							ResourceType ( void ) override
		{ return core::kResourceTypeRrMeshGroup; }
	//	virtual ResourceName() : Returns the resource name.
	// This is used to search for the resource. The smaller, the better.
	RENDER_API const char* const
							ResourceName ( void ) override
		{ return m_name.c_str(); }

public:
	arstring64			m_name;

	std::vector<rrMeshBuffer*>
						m_meshes;
	std::vector<core::math::Cubic>
						m_meshBounds;
	std::vector<arstring64>
						m_meshNames;

	// TODO: Associated materials are also part of meshgroups.
	std::vector<ArMaterialContainer*>
						m_materials;

	std::vector<rrMeshBuffer*>
						m_morphs;
	animation::SkeletonBinding*
						m_skeleton = nullptr;

	struct LoadState
	{
		uint			meshes : 1;
		uint			morphs : 1;
		uint			skeleton : 1;

		LoadState ( void )
			: meshes(false)
			, morphs(false)
			, skeleton(false)
			{}
	};
	LoadState			m_loadState;
};

#endif//RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_
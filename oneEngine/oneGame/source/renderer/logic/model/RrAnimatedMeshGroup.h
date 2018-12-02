#ifndef RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_
#define RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_

#include "core/types/arBaseObject.h"
#include "core/containers/arstring.h"
#include "core-ext/resources/IArResource.h"
#include "core-ext/animation/Skeleton.h"

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
	size_t				m_meshCount;
	rrMeshBuffer**		m_meshes;
	size_t				m_morphCount;
	CMorpherSet**		m_morphs;
	animation::SkeletonBinding*
						m_skeleton;

	arstring64			m_name;

};

#endif//RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_
#ifndef RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_
#define RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_

#include "core/containers/arstring.h"
#include "core-ext/resources/IArResource.h"
#include "core-ext/animation/Skeleton.h"

class RrMesh;
class CMorpherSet;

class CAnimatedMeshGroup : public IArResource
{
public:
	//	virtual ResourceType() : What type of resource is this?
	// Identifies the type of resource this is.
	CORE_API virtual core::arResourceType
							ResourceType ( void )
		{ return core::kResourceTypeRrMeshGroup; }
	//	virtual ResourceName() : Returns the resource name.
	// This is used to search for the resource. The smaller, the better.
	CORE_API virtual const char* const
							ResourceName ( void )
		{ return m_name.c_str(); }

public:
	RrMesh**				m_meshes;
	CMorpherSet**			m_meshMorphs;
	animation::SkeletonBinding*
							m_skeleton;

	arstring64				m_name;

};

#endif//RENDERER_OBJECT_MESH_ANIMATED_MESH_GROUP_H_
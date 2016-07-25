#include "Skeleton.h"
#include "core-ext/transform/TransformUtility.h"

void Animation::Skeleton::GenerationAnimationTransforms ( Skeleton & n_skeleton )
{
	// Ensure math targets are the correct size
	if ( n_skeleton.current_transform.size() != n_skeleton.animation_xpose.size() )
	{
		n_skeleton.current_transform.resize(n_skeleton.animation_xpose.size());
	}
	for ( size_t i = 0; i < n_skeleton.animation_xpose.size(); ++i )
	{
		// TODO: Perform all this math in local space. Since inv_bind_pose is constant, this is doable.

		// Need to convert the local animation delta into a world-space delta
		n_skeleton.current_transform[i].local.position	= n_skeleton.animation_xpose[i].position;
		n_skeleton.current_transform[i].local.scale		= n_skeleton.animation_xpose[i].scale;
		n_skeleton.current_transform[i].local.rotation	= n_skeleton.animation_xpose[i].rotation;
		if ( n_skeleton.parent[i] >= 0 )
			n_skeleton.current_transform[i].UpdateWorldFromLocal( &n_skeleton.current_transform[n_skeleton.parent[i]] );
		else
			n_skeleton.current_transform[i].UpdateWorldFromLocal( NULL );
	}
}

void Animation::Skeleton::GenerateShaderMatrices ( Skeleton& n_skeleton )
{
	// Ensure math targets are the correct size
	if ( n_skeleton.current_pose.size() != n_skeleton.current_transform.size() )
	{
		n_skeleton.current_pose.resize(n_skeleton.current_transform.size());
	}
	for ( size_t i = 0; i < n_skeleton.animation_xpose.size(); ++i )
	{
		// TODO: Perform all this math in local space. Since inv_bind_pose is constant, this is doable.
		// Create the value that will be applied to the skinning math
		n_skeleton.current_pose[i] = n_skeleton.current_transform[i].WorldMatrix() * n_skeleton.inv_bind_pose[i];
	}
}

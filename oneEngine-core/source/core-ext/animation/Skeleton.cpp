#include "Skeleton.h"
#include "core-ext/transform/TransformUtility.h"
#include "core/utils/StringUtils.h"

void animation::Skeleton::GenerationAnimationTransforms ( Skeleton & n_skeleton )
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

void animation::Skeleton::GenerateShaderMatrices ( Skeleton& n_skeleton )
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

//	FindInSkeleton ( const char* name ) : finds best match for input name
// Loops through the information in names[], selects the best match, and returns its index. -1 if no match.
int32_t animation::Skeleton::FindInSkeleton ( const Skeleton& n_skeleton, const char* n_name )
{
	int32_t match = -1;

	// Start with no match, and a minimum length of the name passed in
	string comparison = n_name;
	size_t largest_match = comparison.length();

	// Loop through all names
	for ( size_t i = 0; i < n_skeleton.names.size(); ++i )
	{
		size_t current_match = StringUtils::LargestCommonSubstringLength( comparison, n_skeleton.names[i].c_str() );
		if ( current_match > largest_match )
		{
			current_match = largest_match;
			match = (int32_t)i;
		}
	}
	return match;
}
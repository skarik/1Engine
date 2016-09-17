#ifndef _CORE_ANIMATION_SKELETON_H_
#define _CORE_ANIMATION_SKELETON_H_

#include "core/containers/arstring.h"
#include "core-ext/transform/CTransform.h"
#include "core-ext/transform/TransformUtility.h"

#include <vector>

// WORKFLOW:
// Load: Bind pose given to the skeleton.
// Load: BoneMapper object created, mapping bones from animation to mesh skeleton.
// Load: Animation state is given a skeleton + mapper to sample to. It can handle multiple.
// Animation: Animation sampled.
// Animation: Animation retargeted, copied via BoneMapper index to temporary pose based off skeleton.
// Animation: Animation IK calculated to temporary pose.
// Animation: Pose copied to skeleton.
// Animation: Additional effects occur over skeleton. Skeleton stores previous states.
// Animation update: Skeleton transforms updated.
// Animation update: Matrices for OpenGL created.
// glSkinnedMesh::GetPoseMatrices pulls state directly from skeleton.

namespace Animation
{
	//	class Skeleton
	// Container for skeleton information used for simulation, animation, and display.
	// If any list is empty, then that list is not used. This minimizes memory impact.
	class Skeleton
	{
	public:
		// Matrix4x4: Matrix representation
		// XTransform: Human readable and Havok-compatible representation

		// Bone structure information:

		std::vector<arstring128>	names;
		std::vector<int32_t>		parent;

		// Pose information for basic animation:

		// Bind pose
		std::vector<Matrix4x4>		inv_bind_pose;
		//std::vector<Matrix4x4>		bind_pose;
		std::vector<XTransform>		bind_xpose;

		// Animation source (in bone local-space)
		std::vector<XTransform>		reference_xpose;

		// Animation result (in bone-bind-space)
		//std::vector<Matrix4x4>		animation_pose;
		// Animation result (in bone local-space)
		std::vector<XTransform>		animation_xpose;

		// Current skeleton transform storage
		std::vector<Core::TransformLite>	current_transform;
		// Current skeleton pose for OpenGL (in bone-bind-space)
		std::vector<Matrix4x4>		current_pose;

		// Extra pose information for effects:

		// Previous iteration's pose
		std::vector<Matrix4x4>		ext_previous_pose;
		// Velocity used for jiggle/cloth
		std::vector<Matrix4x4>		ext_pose_velocity;
		// Temporary storage used for jiggle/cloth
		std::vector<Matrix4x4>		ext_pose_information;
		// Strength of ragdoll/physics simulation
		std::vector<Real>			ext_physics_strength;

	public:
		//	GenerationAnimationTransforms ( void ) : generates matrices in current_transform
		// Using animation_xpose and simulation via TransformLite, data is generated into current_pose.
		CORE_API static void		GenerationAnimationTransforms ( Skeleton& n_skeleton );

		//	GenerateShaderMatrices ( void ) : generates matrices in current_pose to pass into a shader
		// Using current_transform, inv_bind_pose, and simulation via TransformLite, data is generated into current_pose.
		CORE_API static void		GenerateShaderMatrices ( Skeleton& n_skeleton );

		//	FindInSkeleton ( const char* name ) : finds best match for input name
		// Loops through the information in names[], selects the best match, and returns its index. -1 if no match.
		CORE_API static int32_t		FindInSkeleton ( const Skeleton& n_skeleton, const char* n_name );
	};
}

#endif//_CORE_ANIMATION_SKELETON_H_
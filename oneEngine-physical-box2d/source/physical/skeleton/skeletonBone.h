// definition for the opengl bones

#ifndef _PHYSICAL_SKELETON_BONE_H_
#define _PHYSICAL_SKELETON_BONE_H_

#include "core/containers/arstring.h"
#include "core-ext/transform/CTransform.h"

#include <vector>

class skeletonBone_t
{
public:
	explicit skeletonBone_t ( skeletonBone_t* parent )
		: ragdollStrength(0)
	{
		//transform.SetTransform( startTransform );
		if ( parent != NULL )
		{
			transform.SetParent( &(parent->transform) );
		}
		transform.active = false;
		transform.owner = (void*)(this);
		transform.ownerType = Transform::TYPE_RENDERER_GLBONE;
		transform.name = this->name;

		tempMatx = Matrix4x4();
		veloMatx = Matrix4x4();
		effect = 0;
	}
	~skeletonBone_t ( void ) {

	}
	// Copy
	skeletonBone_t& operator= ( skeletonBone_t const& right )
	{
		transform.Get( right.transform );
		bindPose = right.bindPose;
		invBindPose = right.invBindPose;
		currentPose = right.currentPose;
		name = right.name;
		index = right.index;
		transform.name = right.transform.name;
		effect = right.effect;

		xBindPose = right.xBindPose;
		xBindPoseModel = right.xBindPoseModel;
		xRagdollPoseModel = right.xRagdollPoseModel;

		effect_v[0] = right.effect_v[0];
		effect_v[1] = right.effect_v[1];
		effect_v[2] = right.effect_v[2];
		effect_v[3] = right.effect_v[3];
		effect_v[4] = right.effect_v[4];
		effect_v[5] = right.effect_v[5];
		effect_v[6] = right.effect_v[6];
		effect_v[7] = right.effect_v[7];
		effect_v[8] = right.effect_v[8];
		return (*this);
	}

	void SetBindPose ( void )
	{
		bindPose = transform.WorldMatrix();
		invBindPose = bindPose.inverse();
	};

	Matrix4x4 GetPoseMatrix ( void )
	{
		return currentPose;
	};
	Matrix4x4 GetInvBindMatrix ( void )
	{
		return invBindPose;
	};

	void UpdatePose ( void )
	{
		currentPose = (transform.WorldMatrix() * invBindPose);
	}

	void SendTransformation ( void )
	{
		transform.SetLocalTransform( animTransform );
	}

	// Main Bone Transform
	Transform transform;

	// Bind Pose
	Matrix4x4 bindPose;
	Matrix4x4 invBindPose;
	// xBindPose is not used the in the built-in animation, but is used to store the Havok bind-pose
	// Since the engine's transform system is still wonky/wrong, this is absolutely necessary.
	XTransform xBindPose;
	XTransform xBindPoseModel;
	XTransform xRagdollPoseModel;

	// Animation Pose
	Matrix4x4 currentPose;
	Matrix4x4 tempMatx;
	Matrix4x4 veloMatx;
	XTransform animTransform;
	// Bone name
	string	name;
	// Bone index
	unsigned char index;
	// Bone LOD value
	unsigned char lod;
	// Bone effect info
	unsigned char effect;
	// Effect values
	Real	effect_v[9];

	// Bone animation strength
	Real	ragdollStrength;
};

typedef skeletonBone_t skeletonBone_t;
typedef skeletonBone_t glBone;


#endif//_PHYSICAL_SKELETON_BONE_H_
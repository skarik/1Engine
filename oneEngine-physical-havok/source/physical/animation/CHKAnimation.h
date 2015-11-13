
#ifndef _C_HK_ANIMATION_H_
#define _C_HK_ANIMATION_H_

#include "physical/physics/CPhysicsCommon.h"
#include "core-ext/animation/CAnimationCommon.h"
#include "core-ext/animation/CAnimation.h"

class glhkSkeleton;
class skeletonBone_t;

#pragma warning( disable : 4351 ) // Disable default C++11 array initialization warning
class CHKAnimation : public CAnimation
{
public:
	// Constructors
	//explicit CHKAnimation ( glhkSkeleton * );
	PHYS_API explicit CHKAnimation ( string const& sInFilename, CAnimation* pFoundReference )
		: CAnimation( sInFilename, pFoundReference ),
		bHKready(false), mMirrorSkelly(NULL), /*mMirrorAnim(NULL),*/ mAnimBinding(NULL),
		mSpineBlends(), mPropBlends()
	{
		;
	}
	PHYS_API explicit CHKAnimation ( string const& sInFilename, CAnimationSet* pInSet )
		: CAnimation( sInFilename, pInSet ),
		bHKready(false), mMirrorSkelly(NULL), /*mMirrorAnim(NULL),*/ mAnimBinding(NULL),
		mSpineBlends(), mPropBlends()
	{
		;
	}

	// Destructor (free pose info, possibly)
	PHYS_API virtual ~CHKAnimation ( void );

	// Update (manually sample animation to the skeleton)
	PHYS_API void Update ( const Real deltaTime ) override;

	// Set skeleton
	//void SetHKSkeleton ( glhkSkeleton* );	// This has to be called before the animation is ready
	PHYS_API void SetSkeleton ( skeletonBone_t*, std::vector<skeletonBone_t*>& );	// This needs to be called before the animation is ready, after the skeleton is assembled.

	PHYS_API virtual void GetRagdollPose ( skeletonBone_t*, std::vector<skeletonBone_t*>& );

	static Vector3d defaultJointVector;
private:
	//hkaPose*	mPose;	// Stores the actual Havok transforms (HOWEVER, REQUIRES A SKELETON)

	//glhkSkeleton*	mSourceSkeleton;	// Target skeleton to work on
	hkaSkeleton*			mSkelly;
	hkaAnimatedSkeleton*	mAnimSkelly;

	void	SetupMirrorMode ( void );
	hkaMirroredSkeleton*	mMirrorSkelly;
	//hkaMirroredAnimation*	mMirrorAnim;
	std::vector<hkaAnimation*>	mMirrorAnims;

	hkaAnimationBinding*	mAnimBinding;

	bool		bHKready;	// If the CHKAnimation is ready to actually perfrom

private:

private:
	std::vector<hkReferencedObject*>	mFootstepIKs;
	Real		mSpineBlends [6];
	Real		mPropBlends [4];
};

#endif
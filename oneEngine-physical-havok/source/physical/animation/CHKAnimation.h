
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
	PHYS_API explicit CHKAnimation ( const Animation::Skeleton& n_skeleton )
		: CAnimation( n_skeleton ),
		mSkelly(NULL), mAnimSkelly(NULL), mMirrorSkelly(NULL), mMirrorAnim(NULL),
		mSpineBlends(), mPropBlends()
	{
		SkeletonToHkaSkeleton();
	}
	PHYS_API explicit CHKAnimation ( CAnimation* pFoundReference )
		: CAnimation( pFoundReference ),
		mSkelly(NULL), mAnimSkelly(NULL), mMirrorSkelly(NULL), mMirrorAnim(NULL),
		mSpineBlends(), mPropBlends()
	{
		SkeletonToHkaSkeleton();
	}
	/*PHYS_API explicit CHKAnimation ( string const& sInFilename, CAnimationSet* pInSet )
		: CAnimation( sInFilename, pInSet ),
		bHKready(false), mMirrorSkelly(NULL), mAnimBinding(NULL),
		mSpineBlends(), mPropBlends()
	{
		;
	}*/

	// Destructor (free pose info, possibly)
	PHYS_API virtual ~CHKAnimation ( void );

	// Update (manually sample animation to the skeleton)
	PHYS_API void Update ( const Real deltaTime ) override;

	// Set skeleton
	//void SetHKSkeleton ( glhkSkeleton* );	// This has to be called before the animation is ready
	//PHYS_API void SetSkeleton ( skeletonBone_t*, std::vector<skeletonBone_t*>& );	// This needs to be called before the animation is ready, after the skeleton is assembled.

	PHYS_API virtual void GetRagdollPose ( skeletonBone_t*, std::vector<skeletonBone_t*>& );

	static Vector3d defaultJointVector;
private:
	//hkaPose*	mPose;	// Stores the actual Havok transforms (HOWEVER, REQUIRES A SKELETON)

	//glhkSkeleton*	mSourceSkeleton;	// Target skeleton to work on
	hkaSkeleton*			mSkelly;
	hkaAnimatedSkeleton*	mAnimSkelly;

	//void	SetupMirrorMode ( void );
	hkaMirroredSkeleton*	mMirrorSkelly;
	hkaMirroredAnimation*	mMirrorAnim;
	std::vector<hkaAnimation*>	mMirrorAnims;

	// Need an animation binding for each animation set
	//std::vector<hkaAnimationBinding*>	mAnimBindings;
	//hkaAnimationBinding*	mAnimBinding;

private:
	//	SkeletonToHkaSkeleton ( ) : converts the skeleton currently contained into a Havok-formatted skeleton.
	void		SkeletonToHkaSkeleton ( void );

	//	CopyHkTransformsToXTransform ( transform list, target list ) : copies transforms to XTransform listing
	// Returns true if successful, returns false or throws exception on failure.
	static bool		CopyHkTransformsToXTransform ( const hkArray<hkQsTransform>& transforms, std::vector<XTransform>& targets );  
	//	CopyXTransformsToHkTransform ( xtransform list, target list ) : copies XTransforms to hkQsTransform listing
	// Returns true if successful, returns false or throws exception on failure
	static bool		CopyXTransformsToHkTransform ( const std::vector<XTransform>& transforms, hkArray<hkQsTransform>& targets );  

private:
	std::vector<hkReferencedObject*>	mFootstepIKs;
	Real		mSpineBlends [6];
	Real		mPropBlends [4];
};

#endif
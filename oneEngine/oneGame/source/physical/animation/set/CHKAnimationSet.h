
#ifndef _C_HK_ANIMATION_SET_H_
#define _C_HK_ANIMATION_SET_H_

#include "physical/physics/CPhysicsCommon.h"
#include "core-ext/animation/CAnimationCommon.h"
#include "core-ext/animation/set/AnimationSet.h"

class CHKAnimationSet : public AnimationSet
{

public:
	PHYS_API explicit CHKAnimationSet ( unsigned int inBoneCount, unsigned int inFrameCount );
	PHYS_API ~CHKAnimationSet ( void );

	// Used for generating the animation set
	PHYS_API void AddValue ( const XTransform& );
	PHYS_API void OrderizeValues ( void );
	PHYS_API void Initialize ( void );	// Actually sends the values to Havok

	// Grab animation object
	/*hkaAnimation*	GetHKAnimation ( void ) {
		return mAnimation;
	}*/
	PHYS_API std::vector<hkaAnimation*>* GetHKAnimation ( void );

	// Export Havok animated skelly values to void pointer of XTransforms
	PHYS_API void Export ( std::vector<void*> & ) override;

	// Set action values (for creating the hkaAnimations)
	PHYS_API void SetActionCount ( const int nSize );
	PHYS_API void SetAction ( const int nIndex, const int nFrameStart, const int nFrameEnd );

private:
	// Used for reordering the values stored in memory
	unsigned int iBoneNum;
	unsigned int iFrameCount;
	// Used for keeping track of the current add potion
	unsigned int iCurrentBone;
	unsigned int iCurrentFrame;
	// Actual frame values
	XTransform* mAnimTransform; // Only used until Havok values set (eventually will move to all Havok)
	// Havok animation object
	//hkaInterleavedUncompressedAnimation* mAnimation;
	std::vector<hkaAnimation*> mAnimations;
	struct actionData_t
	{
		int start;
		int end;
	};
	actionData_t* mActionData;
};

#endif
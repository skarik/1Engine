
#include "CHKAnimationSet.h"

CHKAnimationSet::CHKAnimationSet ( unsigned int inBoneCount, unsigned int inFrameCount )
{
	iBoneNum	= inBoneCount;
	iFrameCount	= inFrameCount;
	mAnimTransform = new XTransform [ iBoneNum*iFrameCount ];
	iCurrentBone = 0;
	iCurrentFrame= 0;

	//mAnimation = NULL;
	mActionData = NULL;
}

CHKAnimationSet::~CHKAnimationSet ( void )
{
	if ( mAnimTransform )
		delete [] mAnimTransform;
	mAnimTransform = NULL;

	/*if ( mAnimation )
		delete mAnimation;
	mAnimation = NULL;*/
	if ( mActionData )
		delete [] mActionData;
	mActionData = NULL;

	for ( unsigned int i = 0; i < mAnimations.size(); ++i )
	{
		delete mAnimations[i];
		mAnimations[i] = NULL;
	}
}

void CHKAnimationSet::AddValue ( const XTransform& inTransform )
{
	fnl_assert( (iCurrentBone+(iCurrentFrame*iBoneNum) < iBoneNum*iFrameCount) );
	mAnimTransform[ iCurrentBone+(iCurrentFrame*iBoneNum) ] = inTransform;
	iCurrentFrame += 1;
	if ( iCurrentFrame >= iFrameCount )
	{
		iCurrentFrame = 0;
		iCurrentBone += 1;
	}
}

void CHKAnimationSet::OrderizeValues ( void )
{
	// Actually unneeded since the values are thrown in correctly.
	iCurrentFrame = 0;
	iCurrentBone = 0;
}

void CHKAnimationSet::Initialize ( void )
{
	// Take the array, (should be aligned at 10 bytes), and send it to Havok
	/*mAnimation = new hkaInterleavedUncompressedAnimation();

	mAnimation->m_transforms.setSize( iBoneNum*iFrameCount );
	for ( unsigned int i = 0; i < iBoneNum*iFrameCount; ++i )
	{
		mAnimation->m_transforms[i].setTranslation( hkVector4( mAnimTransform[i].position.x,mAnimTransform[i].position.y,mAnimTransform[i].position.z ) );
		mAnimation->m_transforms[i].setScale( hkVector4( mAnimTransform[i].scale.x,mAnimTransform[i].scale.y,mAnimTransform[i].scale.z ) );
		mAnimation->m_transforms[i].setRotation( hkQuaternion( mAnimTransform[i].rotation.X(),mAnimTransform[i].rotation.Y(),mAnimTransform[i].rotation.Z(),mAnimTransform[i].rotation.W() ) );
	}
	mAnimation->m_duration = iFrameCount / 30.0f;

	mAnimation->m_numberOfTransformTracks = iBoneNum;
	*/

	// If no animations, add default one
	if ( mAnimations.size() == 0 )
	{
		mAnimations.push_back(NULL);
	}

	// Loop through all actions and build up the sets
	for ( unsigned int i = 0; i < mAnimations.size(); ++i )
	{
		hkaInterleavedUncompressedAnimation* anim = new hkaInterleavedUncompressedAnimation();

		// Set anim length
		int frameCount = 2;
		int frameStartOffset = 0;
		if ( mActionData != NULL ) {
			frameCount = std::max( mActionData[i].end - mActionData[i].start, 2 );
			frameStartOffset = mActionData[i].start;
		}
		anim->m_transforms.setSize( iBoneNum * frameCount );
		// Set the transforms
		for ( int frame = 0; frame < frameCount; ++frame )
		{
			for ( unsigned int bone = 0; bone < iBoneNum; ++bone )
			{	// iCurrentBone+(iCurrentFrame*iBoneNum)
				unsigned int frameOffset = frame + frameStartOffset;
				unsigned int target = bone+(frame*iBoneNum);
				unsigned int source = bone+(frameOffset*iBoneNum);
				fnl_assert( (target < iBoneNum*iFrameCount) );
				fnl_assert( (source < iBoneNum*iFrameCount) );
				anim->m_transforms[target].setTranslation( hkVector4(
					mAnimTransform[source].position.x,
					mAnimTransform[source].position.y,
					mAnimTransform[source].position.z
					) );
				anim->m_transforms[target].setScale( hkVector4(
					mAnimTransform[source].scale.x,
					mAnimTransform[source].scale.y,
					mAnimTransform[source].scale.z
					) );
				anim->m_transforms[target].setRotation( hkQuaternion(
					mAnimTransform[source].rotation.X(),
					mAnimTransform[source].rotation.Y(),
					mAnimTransform[source].rotation.Z(),
					mAnimTransform[source].rotation.W()
					) );
			}
		}
		// Set animation length
		anim->m_duration = frameCount / 30.0f;
		anim->m_numberOfTransformTracks = iBoneNum;

		// Set anim to final list
		mAnimations[i] = anim;
	}

	// And now we can delete the old data since it's now stored in Havok.
	if ( mAnimTransform )
		delete [] mAnimTransform;
	mAnimTransform = NULL;

	// Old data is now sitting pretty in Havok
}

void CHKAnimationSet::Export ( std::vector<void*> & )
{
	//std::cout << "INVALID SET TO EXPORT." << std::endl;
	//throw Core::InvalidCallException();
}
std::vector<hkaAnimation*>* CHKAnimationSet::GetHKAnimation ( void )
{
	return &mAnimations;
}

// Set action
void CHKAnimationSet::SetActionCount ( const int nSize )
{
	if ( nSize != 0 ) {
		mAnimations.resize( nSize, NULL );
		mActionData = new actionData_t [nSize];
	}
}
void CHKAnimationSet::SetAction ( const int nIndex, const int nFrameStart, const int nFrameEnd )
{
	mActionData[nIndex].start	= nFrameStart;
	mActionData[nIndex].end		= nFrameEnd;
}


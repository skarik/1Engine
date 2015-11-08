// Includes
//#include "core/time/time.h"
#include "CAnimation.h"
//#include "CModelMaster.h"
#include "core/utils/StringUtils.h"

using std::cout;
using std::endl;

// Class Constants
const unsigned char CAnimation::maxLayers = 6;
const bool			CAnimation::useHavok = true;

CAnimAction			CAnimation::deadAction ( "" );

// Constructors
CAnimation::CAnimation ( string const& sInFilename, CAnimation* pFoundReference )
{
	pAnimationSet = NULL;
	sFilename = sInFilename;
	//pAnimationSet = ModelMaster.GetAnimationReference( sFilename );
	// Grab reference
	//CAnimation* pFoundReference = ModelMaster.GetAnimationReference( sFilename ); // this should go elsewhere!
	if ( pFoundReference != NULL )
	{
		// Copy the needed data
		(*this) = pFoundReference;
		// Is valid
		bIsValid = true;

		//ModelMaster.AddReference( sFilename, this );
	}
	else
	{
		//cout << "Warning: invalid animation reference...prepare for a clusterfuck." << endl;
		bIsValid = false;

		//throw Core::InvalidArgumentException();
	}

	bEventsRead = false;
}
CAnimation::CAnimation ( string const& sInFilename, CAnimationSet* pInSet )
{
	pAnimationSet = pInSet;
	sFilename = sInFilename;
	//ModelMaster.AddReference( sFilename, pAnimationSet );
	//ModelMaster.AddReference( sFilename, this );
}

// Destructor
CAnimation::~CAnimation ( void )
{
	//if ( bIsValid )
	//	ModelMaster.RemoveAnimSetReference( sFilename );
}

// Add action
void CAnimation::AddAction ( CAnimAction newAction )
{
	mAnimations[newAction.GetName()] = newAction;
	//cout << "Anim: " << newAction.actionName << " S: " << newAction.start << " E: " << newAction.end << " L: " << newAction.loop << endl;
}
void CAnimation::AddIKInfo ( const ikinfo_t& ik )
{
	ikList.push_back( ik );
}

// Copy from existing reference (pointer)
CAnimation& CAnimation::operator= ( CAnimation* pRight )
{
	return ((*this) = (*pRight));
}
// Copy from existing refernece
CAnimation&	CAnimation::operator= ( CAnimation const& sourceAnimRef )
{
	// Copy the animation set
	pAnimationSet = sourceAnimRef.pAnimationSet;
	// Copy the action map
	mAnimations = sourceAnimRef.mAnimations;
	// Set all of the animation actions' owners to this, and reset them all
	for ( std::map<string,CAnimAction>::iterator it = mAnimations.begin(); it != mAnimations.end(); it++ )
	{
		it->second.Reset();
		it->second.owner = this;
	}
	// Set the first action to active
	if ( !mAnimations.empty() ) {
		mAnimations.begin()->second.Play();
	}
	// Copy the IK list
	ikList = sourceAnimRef.ikList;

	// Return this
	return (*this);
}


void CAnimation::Play ( string const& animName )
{
	// Find animation with name
	std::map<string,CAnimAction>::iterator it;

	it = mAnimations.find( StringUtils::ToLower( animName ) );
	if ( it == mAnimations.end() )
	{
		cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
	}
	else
	{
		it->second.Play( 1.0f );
	}
}
void CAnimation::PlaySmoothed ( string const& animName, ftype const smoothTime )
{
	// Find animation with name
	std::map<string,CAnimAction>::iterator it;

	it = mAnimations.find( StringUtils::ToLower( animName ) );
	if ( it == mAnimations.end() )
	{
		cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
	}
	else
	{
		it->second.Play( 1.0f, smoothTime );
	}
}
void CAnimation::Stop ( string const& animName )
{
	// Find animation with name
	std::map<string,CAnimAction>::iterator it;

	it = mAnimations.find( StringUtils::ToLower( animName ) );
	if ( it == mAnimations.end() )
	{
		cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
	}
	else
	{
		it->second.Stop();
	}
}
void CAnimation::StopSmoothed ( string const& animName, ftype const smoothTime )
{
	// Find animation with name
	std::map<string,CAnimAction>::iterator anim;

	anim = mAnimations.find( StringUtils::ToLower( animName ) );
	if ( anim == mAnimations.end() )
	{
		cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
	}
	else
	{
		// find guy in fadeOutList
		for ( auto it = fadeOutList.begin(); it != fadeOutList.end(); ++it )
		{
			if ( it->first == &(anim->second) ) {
				it->second = smoothTime;
				return;
			}
		}
		// here, entry not exists
		std::pair<CAnimAction*,ftype> newFade ( &(anim->second), smoothTime );
		fadeOutList.push_back( newFade );
	}
}

void CAnimation::Normalize ( const uchar layer )
{
	ftype weightCount = 0;
	for ( auto anim = mAnimations.begin(); anim != mAnimations.end(); ++anim )
	{
		if ( anim->second.layer == layer )
		{
			weightCount += anim->second.weight;
		}
	}
	if ( weightCount > 1.0f )
	{
		for ( auto anim = mAnimations.begin(); anim != mAnimations.end(); ++anim )
		{
			if ( anim->second.layer == layer )
			{
				anim->second.weight /= weightCount;
			}
		}
	}
}


CAnimAction& CAnimation::operator [] ( string const& animName )
{
	// Find animation with name
	std::map<string,CAnimAction>::iterator it;

	it = mAnimations.find( StringUtils::ToLower( animName ) );
	if ( it == mAnimations.end() ) {
#ifdef _ENGINE_DEBUG
		//cout << "WARNING: Could not find animation named \"" << animName << "\" in list!" << endl;
		//throw std::out_of_range( "Bad animation" );
#endif
		deadAction.isPlaying = false;
		deadAction.weight = 0;
		deadAction.frame = 0;
		deadAction.owner = this;
		return deadAction;
	}

	// return reference to it
	return it->second;
}
CAnimAction& CAnimation::operator [] ( const int & animIndex )
{
	std::map<string,CAnimAction>::iterator it;
	it = mAnimations.begin();
	/*for ( int i = 0; i < animIndex; ++i )
		++it;*/
	while ( it != mAnimations.end() && it->second.index != animIndex ) {
		++it;
	}

	if ( it == mAnimations.end() ) {
#ifdef _ENGINE_DEBUG
		cout << "WARNING: Could not find index numbered \"" << animIndex << "\" in list!";
		//throw std::out_of_range( "Bad animation" );
#endif
		deadAction.isPlaying = false;
		deadAction.weight = 0;
		deadAction.frame = 0;
		deadAction.owner = this;
		return deadAction;
	}
	return it->second;
}
CAnimAction* CAnimation::FindAction ( string const& animName )
{
	// Find animation with name
	std::map<string,CAnimAction>::iterator it;

	it = mAnimations.find( StringUtils::ToLower( animName ) );
	if ( it == mAnimations.end() ) {
		//cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
		return NULL;
	}
	else {
		return &(it->second);
	}
}

ikinfo_t& CAnimation::GetIKInfo ( const string& chainName )
{
	for ( uint i = 0; i < ikList.size(); ++i )
	{
		if ( chainName == ikList[i].name ) {
			return ikList[i];
		}
	}
	throw std::out_of_range( "Cannot find chain" );
	return ikList[0];
}


// Copy reference list
void CAnimation::AssignReferenceList ( std::vector<void*> const& newRef )
{
	animRefs = newRef;
}

// Update
void CAnimation::Update ( const Real deltaTime )
{
	// Before sampling actions, need to check events
	if ( bEventsRead ) {
		vEvents.clear();
		bEventsRead = false;
	}
	// Reset model motion
	vModelMotion = Vector3d(0,0,0);
	
	// Need to update smooth faders
	for ( auto it = fadeOutList.begin(); it != fadeOutList.end(); )
	{
		it->first->weight -= deltaTime / it->second;
		if ( it->first->weight <= 0 ) {
			it->first->isPlaying = false;
			it->first->weight = 0;
			it = fadeOutList.erase(it);
		}
		else {
			++it;
		}
	}

	for ( unsigned char layer = 0; layer < maxLayers; layer++ )
	{
		std::map<string,CAnimAction>::iterator it = mAnimations.begin();
		do
		{
			CAnimAction& currentAction = it->second;
			// First, check if the animation is 
			// First, check the animation layer
			// Check for a layer limit
			if ( currentAction.layer >= maxLayers )
				currentAction.layer = maxLayers-1;
			// And only sample+update the animation if it's on the current layer
			if ( currentAction.layer == layer )
			{
				// Update the animation
				currentAction.Update(deltaTime,0.0f);

				// Now, check the weight
				if ( currentAction.weight > 1 )
					currentAction.weight = 1;
				// And only sample if the weight is larger than zero
				if ( currentAction.weight > 0.001f )
				{
					currentAction.Sample( pAnimationSet, animRefs );
				}
			}

			it++;
		}
		while ( it != mAnimations.end() );
	}

	// Now export all the animations
	pAnimationSet->Export( animRefs );

}


void CAnimation::PushFrameEvent ( const Animation::ActionEvent & a_event )
{
	vEvents.push_back( a_event );
}

void CAnimation::GetEvents ( std::vector<Animation::ActionEvent>& events, unsigned int & event_count )
{
	events.clear();
	events = vEvents;
	//vEvents.clear();
	bEventsRead = true;
	event_count = events.size();
}


const Vector3d&	CAnimation::GetExtrapolatedMotion ( void )
{
	return vModelMotion;
}
void CAnimation::ResetExtrapolatedMotion ( void )
{
	vModelMotion = Vector3d(0,0,0);
}
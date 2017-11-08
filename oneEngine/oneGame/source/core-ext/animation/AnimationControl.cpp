// Includes
#include "AnimationControl.h"
#include "core/utils/StringUtils.h"

using std::cout;
using std::endl;

// Class Constants
const unsigned char AnimationControl::maxLayers = 6;
const bool			AnimationControl::useHavok = true;

AnimationAction			AnimationControl::deadAction ( "" );

// Class static listing
std::vector<AnimationControl*>	AnimationControl::_AnimationInstances;
// Exported instance list
const std::vector<AnimationControl*>& AnimationControl::Instances ( void )
{
	return _AnimationInstances;
}

// Constructors
AnimationControl::AnimationControl ( const animation::Skeleton& n_skeleton )
	: bIsValid(false), bEventsRead(false)
{
	skeleton = n_skeleton;
	bIsValid = true;

	_AnimationInstances.push_back(this);
}
AnimationControl::AnimationControl ( AnimationControl* pFoundReference )
	: bIsValid(false), bEventsRead(false)
{
	if ( pFoundReference != NULL )
	{
		// Copy the needed data
		(*this) = pFoundReference;
		// Is valid
		bIsValid = true;
	}
	else
	{
		throw core::InvalidArgumentException();
	}

	_AnimationInstances.push_back(this);
}

// Destructor
AnimationControl::~AnimationControl ( void )
{
	_AnimationInstances.erase( std::find(_AnimationInstances.begin(), _AnimationInstances.end(), this ) );
}

void AnimationControl::AddAction ( AnimationAction& newAction )
{
	mAnimations[newAction.GetName()] = newAction;
	//cout << "Anim: " << newAction.actionName << " S: " << newAction.start << " E: " << newAction.end << " L: " << newAction.loop << endl;
}
void AnimationControl::AddIKInfo ( const animation::arIKInfo& ik )
{
	ikList.push_back( ik );
}

// Copy from existing reference (pointer)
AnimationControl& AnimationControl::operator= ( AnimationControl* pRight )
{
	return ((*this) = (*pRight));
}
// Copy from existing refernece
AnimationControl&	AnimationControl::operator= ( AnimationControl const& sourceAnimRef )
{
	// Copy the skeleton
	skeleton = sourceAnimRef.skeleton;
	// Copy the animation set
	sampleSource = sourceAnimRef.sampleSource;
	sampleMappingTrack = sourceAnimRef.sampleMappingTrack;
	// Copy the action map
	mAnimations = sourceAnimRef.mAnimations;
	// Set all of the animation actions' owners to this, and reset them all
	for ( auto it = mAnimations.begin(); it != mAnimations.end(); it++ )
	{
		it->second.Reset();
		it->second.owner = this;
	}
	// Set the first action to active
	if ( !mAnimations.empty() ) {
		mAnimations.begin()->second.Play(0.01F);
	}
	// Copy the IK list
	ikList = sourceAnimRef.ikList;

	// Return this
	return (*this);
}


void AnimationControl::Play ( const char* animName )
{
	// Find animation with name
	auto it = mAnimations.find( arstring128(animName) );
	if ( it == mAnimations.end() )
	{
		cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
	}
	else
	{
		it->second.Play( 1.0f );
	}
}
void AnimationControl::PlaySmoothed ( const char* animName, Real const smoothTime )
{
	// Find animation with name
	auto it = mAnimations.find( arstring128(animName) );
	if ( it == mAnimations.end() )
	{
		cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
	}
	else
	{
		it->second.Play( 1.0f, smoothTime );
	}
}
void AnimationControl::Stop ( const char* animName )
{
	// Find animation with name
	auto it = mAnimations.find( arstring128(animName) );
	if ( it == mAnimations.end() )
	{
		cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
	}
	else
	{
		it->second.Stop();
	}
}
void AnimationControl::StopSmoothed ( const char* animName, Real const smoothTime )
{
	// Find animation with name
	auto anim = mAnimations.find( arstring128(animName) );
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
		std::pair<AnimationAction*,Real> newFade ( &(anim->second), smoothTime );
		fadeOutList.push_back( newFade );
	}
}

void AnimationControl::Normalize ( const uchar layer )
{
	Real weightCount = 0;
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


AnimationAction& AnimationControl::operator [] ( const char* animName )
{
	// Find animation with name
	auto it = mAnimations.find( arstring128( animName ) );
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
AnimationAction& AnimationControl::operator [] ( const int & animIndex )
{
	auto it = mAnimations.begin();
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
AnimationAction* AnimationControl::FindAction ( const char* animName )
{
	// Find animation with name
	auto it = mAnimations.find( arstring128( animName ) );
	if ( it == mAnimations.end() ) {
		//cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
		return NULL;
	}
	else {
		return &(it->second);
	}
}

animation::arIKInfo& AnimationControl::GetIKInfo ( const string& chainName )
{
	for ( uint i = 0; i < ikList.size(); ++i )
	{
		if ( ikList[i].name.compare(chainName.c_str()) )
		{
			return ikList[i];
		}
	}
	throw std::out_of_range( "Cannot find chain" );
	return ikList[0];
}


// Copy reference list
bool AnimationControl::AddOutput ( animation::Skeleton* output )
{
	animation::BoneMapper mapping;
	animation::BoneMapper::CreateFromNameMatching( skeleton, *output, mapping, false );
	return AddOutput( output, mapping );
}
bool AnimationControl::AddOutput ( animation::Skeleton* output, animation::BoneMapper& manual_mapping )
{
	// Search for existing entry
	for ( auto output_itr = vOutputs.begin(); output_itr != vOutputs.end(); ++output_itr )
	{
		if ( output == output_itr->target )
		{
			throw core::InvalidCallException();
			return false;
		}
	}
	// Add entry otherwise
	output_t entry;
	entry.mapping = manual_mapping;
	entry.target = output;
	vOutputs.push_back( entry );
	// Success
	return true;
}

// Update
void AnimationControl::Update ( const Real deltaTime )
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
		auto it = mAnimations.begin();
		do
		{
			AnimationAction& currentAction = it->second;
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
					//currentAction.Sample( pAnimationSet, skeleton.animation_xpose );
					throw core::NotYetImplementedException();
				}
			}

			it++;
		}
		while ( it != mAnimations.end() );
	}

	// Now export all the animations
	//pAnimationSet->Export( animRefs ); // Fuck this, TODO later
	// TODO: Make animation system more general
}


void AnimationControl::PushFrameEvent ( const animation::ActionEvent & a_event )
{
	vEvents.push_back( a_event );
}

void AnimationControl::GetEvents ( std::vector<animation::ActionEvent>& events, unsigned int & event_count )
{
	events.clear();
	events = vEvents;
	//vEvents.clear();
	bEventsRead = true;
	event_count = (unsigned int)events.size();
}


const Vector3d&	AnimationControl::GetExtrapolatedMotion ( void )
{
	return vModelMotion;
}
void AnimationControl::ResetExtrapolatedMotion ( void )
{
	vModelMotion = Vector3d(0,0,0);
}
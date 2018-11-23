#ifndef C_ANIMATION_CONTROL_H_
#define C_ANIMATION_CONTROL_H_

// == Includes ==
// Animation common
#include "core/types/types.h"
#include "core-ext/animation/Common.h"
#include "core-ext/animation/AnimationEvents.h"
#include "core-ext/animation/set/AnimationSet.h"

// Include actions
#include "core-ext/animation/AnimationAction.h"
// Include IK
#include "core-ext/animation/AnimIK.h"
// Include skeleton and mapping
#include "core-ext/animation/Skeleton.h"
#include "core-ext/animation/BoneMapper.h"

// Using string and map to reference animation
#include "core/containers/arstring.h"
#include <map>
#include <utility>

class RrCModel;
class CSkinnedModel;

// Class Definition
class AnimationControl
{
public:
	CORE_API explicit		AnimationControl ( const animation::Skeleton& n_skeleton );
	CORE_API explicit		AnimationControl ( AnimationControl* );		// Construct by reference
	//CORE_API explicit		AnimationControl ( string const&, AnimationSet* );	// Construct by set

	CORE_API AnimationControl&	operator= ( AnimationControl const& );
	CORE_API AnimationControl&	operator= ( AnimationControl* );

	CORE_API virtual		~AnimationControl ( void );

	CORE_API virtual void	Update ( const Real deltaTime );

	//	UpdateTransform ( world_transform ) : Set transform
	// Update the position used by the animation system for world effects.
	// Generally set the model is passed in as the animation action.
	void UpdateTransform ( const XTransform& n_transform )
	{
		mModelTransform = n_transform;
	}

			 void Play ( const string& animName ) { return this->Play(animName.c_str()); }
	CORE_API void Play ( const char* animName );
	         void PlaySmoothed ( const string& animName, Real const smoothTime ) { return this->PlaySmoothed(animName.c_str(),smoothTime); }
	CORE_API void PlaySmoothed ( const char* animName, Real const smoothTime );
	         void Stop ( const string& animName ) { return this->Stop(animName.c_str()); }
	CORE_API void Stop ( const char* animName );
	         void StopSmoothed ( const string& animName, Real const smoothTime ) { return this->StopSmoothed(animName.c_str(),smoothTime); }
	CORE_API void StopSmoothed ( const char* animName, Real const smoothTime );

	CORE_API void Normalize ( const uchar layer );

	CORE_API AnimationAction&	operator[] ( const int & animIndex );

	         AnimationAction&	operator[] ( const string& animName ) { return this->operator[](animName.c_str()); }
	CORE_API AnimationAction&	operator[] ( const char* animName );
	         AnimationAction*	FindAction ( const string& animName ) { return this->FindAction(animName.c_str()); }
	CORE_API AnimationAction*	FindAction ( const char* animName );

	CORE_API void			LoadActions ( const char* );
	CORE_API void			AddAction ( AnimationAction& );
	const std::map<arstring128,AnimationAction>& GetActionMap ( void ) const {
		return mAnimations;
	}

	//CORE_API void			AssignReferenceList ( std::vector<void*> const& );

	//	AddOutput ( output, [optional] mapping ) : Add animation output target
	// Returns true on successful mapping.
	CORE_API bool			AddOutput ( animation::Skeleton* output );
	CORE_API bool			AddOutput ( animation::Skeleton* output, animation::BoneMapper& manual_mapping );

	/*AnimationSet*	GetAnimationSet ( void ) {
		return pAnimationSet;
	}*/
	std::vector<AnimationSet*>& GetAnimationSource ( void ) {
		return sampleSource;
	}
	bool	IsValid ( void ) {
		return bIsValid;
	}
	
	/*void SetOwner ( RrCModel* model ) {
		pOwner = model;
	}
	RrCModel* GetOwner ( void ) {
		return pOwner;
	}*/

	//	GetSkeleton () : Return skeleton used as the internal animation's sampling result
	const animation::Skeleton& GetSkeleton ( void ) {
		return skeleton;
	}

	CORE_API void			GetEvents ( std::vector<animation::ActionEvent>& events, unsigned int & event_count );

	CORE_API const Vector3d&	GetExtrapolatedMotion ( void );
	CORE_API void			ResetExtrapolatedMotion ( void );

	CORE_API void			AddIKInfo ( const animation::arIKInfo& );
	CORE_API animation::arIKInfo&	GetIKInfo ( const string& );

	CORE_API static const unsigned char	maxLayers; // 6 Max Layers
	CORE_API static const bool			useHavok;

	CORE_API static AnimationAction	deadAction;
protected:
	friend AnimationAction;
	friend RrCModel;
	friend CSkinnedModel;

	// Is the animation valid and loaded?
	bool					bIsValid;

	// Old sampling and instantiation method
	//RrCModel*					pOwner; 

	// List of actions
	std::map<arstring128,AnimationAction>	mAnimations;
	// Filename the animations are sourced from - invalid for multiple animation sets
	string					sFilename;
	// Skeleton used to save animations in the interim
	animation::Skeleton		skeleton;

	// Sampling source
	std::vector<AnimationSet*>			sampleSource;
	// Mapping tracks used by animation sets for sampling to the internal skeleton
	std::vector<animation::BoneMapper>	sampleMappingTrack;

	// IK info
	std::vector<animation::arIKInfo>	ikList;

	// Animations to fade out next
	std::vector<std::pair<AnimationAction*,Real>>	fadeOutList;

	// Event sampling information
	CORE_API void			PushFrameEvent ( const animation::ActionEvent & );
	std::vector<animation::ActionEvent>		vEvents;
	bool					bEventsRead;

	// Extrapolated motion
	Vector3d				vModelMotion;
	// TODO: ???
	XTransform				mModelTransform;

	// Output storage
	struct output_t
	{
		animation::BoneMapper	mapping;
		animation::Skeleton*	target;
	};
	std::vector<output_t>	vOutputs;

private:
	// Static list of all animation instances for use in updating during physics step
	static std::vector<AnimationControl*>	_AnimationInstances;
public:
	// Updating is handled in the oneGame module. Thus, the instance list is exported.
	CORE_API static const std::vector<AnimationControl*>& Instances ( void );
};

#endif//C_ANIMATION_CONTROL_H_
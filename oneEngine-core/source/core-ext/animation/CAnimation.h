
#ifndef _C_ANIMATION_H_
#define _C_ANIMATION_H_

// == Includes ==
// Animation common
#include "core/types/types.h"
#include "core-ext/animation/CAnimationCommon.h"
#include "core-ext/animation/CAnimationEvents.h"
#include "core-ext/animation/set/CAnimationSet.h"

// Include actions
#include "core-ext/animation/CAnimAction.h"
// Include IK
#include "core-ext/animation/CAnimIK.h"
// Include skeleton and mapping
#include "core-ext/animation/Skeleton.h"
#include "core-ext/animation/BoneMapper.h"

// Using string and map to reference animation
#include "core/containers/arstring.h"
#include <map>
#include <utility>

class CModel;
class CSkinnedModel;

// Class Definition
class CAnimation
{
public:
	CORE_API explicit		CAnimation ( const Animation::Skeleton& n_skeleton );
	CORE_API explicit		CAnimation ( CAnimation* );		// Construct by reference
	//CORE_API explicit		CAnimation ( string const&, CAnimationSet* );	// Construct by set

	CORE_API CAnimation&	operator= ( CAnimation const& );
	CORE_API CAnimation&	operator= ( CAnimation* );

	CORE_API virtual		~CAnimation ( void );

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

	CORE_API CAnimAction&	operator[] ( const int & animIndex );

	         CAnimAction&	operator[] ( const string& animName ) { return this->operator[](animName.c_str()); }
	CORE_API CAnimAction&	operator[] ( const char* animName );
	         CAnimAction*	FindAction ( const string& animName ) { return this->FindAction(animName.c_str()); }
	CORE_API CAnimAction*	FindAction ( const char* animName );

	CORE_API void			LoadActions ( const char* );
	CORE_API void			AddAction ( CAnimAction& );
	const std::map<arstring128,CAnimAction>& GetActionMap ( void ) const {
		return mAnimations;
	}

	//CORE_API void			AssignReferenceList ( std::vector<void*> const& );

	//	AddOutput ( output, [optional] mapping ) : Add animation output target
	// Returns true on successful mapping.
	CORE_API bool			AddOutput ( Animation::Skeleton* output );
	CORE_API bool			AddOutput ( Animation::Skeleton* output, Animation::BoneMapper& manual_mapping );

	/*CAnimationSet*	GetAnimationSet ( void ) {
		return pAnimationSet;
	}*/
	std::vector<CAnimationSet*>& GetAnimationSource ( void ) {
		return sampleSource;
	}
	bool	IsValid ( void ) {
		return bIsValid;
	}
	
	/*void SetOwner ( CModel* model ) {
		pOwner = model;
	}
	CModel* GetOwner ( void ) {
		return pOwner;
	}*/

	//	GetSkeleton () : Return skeleton used as the internal animation's sampling result
	const Animation::Skeleton& GetSkeleton ( void ) {
		return skeleton;
	}

	CORE_API void			GetEvents ( std::vector<Animation::ActionEvent>& events, unsigned int & event_count );

	CORE_API const Vector3d&	GetExtrapolatedMotion ( void );
	CORE_API void			ResetExtrapolatedMotion ( void );

	CORE_API void			AddIKInfo ( const ikinfo_t& );
	CORE_API ikinfo_t&		GetIKInfo ( const string& );

	CORE_API static const unsigned char	maxLayers; // 6 Max Layers
	CORE_API static const bool			useHavok;

	CORE_API static CAnimAction	deadAction;
protected:
	friend CAnimAction;
	friend CModel;
	friend CSkinnedModel;

	// Is the animation valid and loaded?
	bool					bIsValid;

	// Old sampling and instantiation method
	//CModel*					pOwner; 

	// List of actions
	std::map<arstring128,CAnimAction>	mAnimations;
	// Filename the animations are sourced from - invalid for multiple animation sets
	string					sFilename;
	// Skeleton used to save animations in the interim
	Animation::Skeleton		skeleton;

	// Sampling source
	std::vector<CAnimationSet*>			sampleSource;
	// Mapping tracks used by animation sets for sampling to the internal skeleton
	std::vector<Animation::BoneMapper>	sampleMappingTrack;

	// IK info
	std::vector<ikinfo_t>	ikList;

	// Animations to fade out next
	std::vector<std::pair<CAnimAction*,Real>>	fadeOutList;

	// Event sampling information
	CORE_API void			PushFrameEvent ( const Animation::ActionEvent & );
	std::vector<Animation::ActionEvent>		vEvents;
	bool					bEventsRead;

	// Extrapolated motion
	Vector3d				vModelMotion;
	// TODO: ???
	XTransform				mModelTransform;

	// Output storage
	struct output_t
	{
		Animation::BoneMapper	mapping;
		Animation::Skeleton*	target;
	};
	std::vector<output_t>	vOutputs;

private:
	// Static list of all animation instances for use in updating during physics step
	static std::vector<CAnimation*>	_AnimationInstances;
public:
	// Updating is handled in the oneGame module. Thus, the instance list is exported.
	CORE_API static const std::vector<CAnimation*>& Instances ( void );
};

#endif
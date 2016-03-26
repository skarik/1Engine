
#ifndef _C_ANIMATION_H_
#define _C_ANIMATION_H_

// == Includes ==
// Animation common
#include "CAnimationCommon.h"
#include "set/CAnimationSet.h"
#include "CAnimationEvents.h"

// Using string and map to reference animation
#include "core/containers/arstring.h"
#include <map>
#include <utility>

// Include actions
#include "CAnimAction.h"
// Include IK
#include "CAnimIK.h"

class CModel;
class CSkinnedModel;

// Class Definition
class CAnimation
{
public:
	CORE_API explicit		CAnimation ( string const&, CAnimation* );		// Construct by reference
	CORE_API explicit		CAnimation ( string const&, CAnimationSet* );	// Construct by set

	//explicit CAnimation ( CAnimation* );
	CORE_API CAnimation&	operator= ( CAnimation const& );
	CORE_API CAnimation&	operator= ( CAnimation* );

	CORE_API virtual		~CAnimation ( void );

	CORE_API virtual void	Update ( const Real deltaTime );

	// Update the position used by the animation system
	void UpdateTransform ( const XTransform& n_transform )
	{
		mModelTransform = n_transform;
	}

			 void Play ( const string& animName ) { return this->Play(animName.c_str()); }
	CORE_API void Play ( const char* animName );
	         void PlaySmoothed ( const string& animName, ftype const smoothTime ) { return this->PlaySmoothed(animName.c_str(),smoothTime); }
	CORE_API void PlaySmoothed ( const char* animName, ftype const smoothTime );
	         void Stop ( const string& animName ) { return this->Stop(animName.c_str()); }
	CORE_API void Stop ( const char* animName );
	         void StopSmoothed ( const string& animName, ftype const smoothTime ) { return this->StopSmoothed(animName.c_str(),smoothTime); }
	CORE_API void StopSmoothed ( const char* animName, ftype const smoothTime );

	CORE_API void Normalize ( const uchar layer );

	CORE_API CAnimAction&	operator[] ( const int & animIndex );

	         CAnimAction&	operator[] ( const string& animName ) { return this->operator[](animName.c_str()); }
	CORE_API CAnimAction&	operator[] ( const char* animName );
	         CAnimAction*	FindAction ( const string& animName ) { return this->FindAction(animName.c_str()); }
	CORE_API CAnimAction*	FindAction ( const char* animName );

	CORE_API void			LoadActions ( const char* );
	CORE_API void			AddAction ( CAnimAction );
	const std::map<arstring128,CAnimAction>& GetActionMap ( void ) const {
		return mAnimations;
	}

	CORE_API void			AssignReferenceList ( std::vector<void*> const& );

	CAnimationSet*	GetAnimationSet ( void ) {
		return pAnimationSet;
	}
	bool	IsValid ( void ) {
		return bIsValid;
	}
	
	void SetOwner ( CModel* model ) {
		pOwner = model;
	}
	CModel* GetOwner ( void ) {
		return pOwner;
	}

	CORE_API void			GetEvents ( std::vector<Animation::ActionEvent>& events, unsigned int & event_count );

	CORE_API const Vector3d&	GetExtrapolatedMotion ( void );
	CORE_API void			ResetExtrapolatedMotion ( void );

	CORE_API void			AddIKInfo ( const ikinfo_t& );
	CORE_API ikinfo_t&		GetIKInfo ( const string& );

	//const unsigned char maxLayers = 6;
	CORE_API static const unsigned char	maxLayers;
	CORE_API static const bool			useHavok;

	CORE_API static CAnimAction	deadAction;
protected:
	friend CAnimAction;
	friend CModel;
	friend CSkinnedModel;

	std::map<arstring128,CAnimAction>	mAnimations;
	CAnimationSet*			pAnimationSet;
	string					sFilename;
	std::vector<void*>		animRefs;

	std::vector<ikinfo_t>	ikList;
	std::vector<std::pair<CAnimAction*,ftype>>	fadeOutList;

	bool					bIsValid;

	CModel*					pOwner;

	CORE_API void			PushFrameEvent ( const Animation::ActionEvent & );
	std::vector<Animation::ActionEvent>		vEvents;
	bool					bEventsRead;

	Vector3d				vModelMotion;

	XTransform				mModelTransform;
};

#endif
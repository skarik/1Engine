
#ifndef _C_CHARACTER_MODEL_H_
#define _C_CHARACTER_MODEL_H_

#include "core-ext/transform/TransformUtility.h"
#include "core-ext/animation/AnimationEvents.h"
#include "engine/behavior/CGameObject.h"
//#include "after/entities/item/CWeaponItem.h"
#include "engine-common/types/ItemProperties.h"

#include <string>
using std::string;

class CCharacter;
class CAnimAction;
class CModel;
class CSkinnedModel;
class CActor;
class CAnimation;
class CHKAnimation;
class CRagdollCollision;
class CMorpher;
namespace Animation
{
	class Sequence;
}

class CCharacterModel : public CGameObject
{
	ClassName( "CharacterModel" );
public:
	explicit		CCharacterModel ( CCharacter* owner );
					~CCharacterModel( void );

	// = Loading and Model Set Operations =

	// LoadBase( character_name )
	// returns:
	//		true when model loading is successful, false if error 
	// description:
	//		Loads the base character model with the given name.
	//		If the character given cannot be found, then the model is not loaded, and the operation of the rest of the model is undefined. 
	bool			LoadBase ( const string& character_name );
	void			ConfigureBase ( CSkinnedModel* target_model );
	// LoadModel ( model_filename )
	// returns:
	//		true when model loading is successful, false if error
	// description:
	//		Loads the base model with the given name.
	//		The basename is set to an empty string. If the model can not be found, then the game crashes. Usually. Or undefined behavior.
	virtual bool	LoadModel ( const string& model_filename );

	// Update
	void			Update	   ( void ) override;
	void			LateUpdate ( void ) override;
	void			PostUpdate ( void ) override;
	void			PostFixedUpdate ( void ) override;

	// = Toggle visiblity of the model =

	bool			GetVisibility ( void ) { return bShowModel; };
	void			SetVisibility ( const bool show=true ) { bShowModel = show; };
	void			SetModelAlpha ( const ftype alpha ) { fModelAlpha = alpha; };

	// = Interface to grab the positions of attachments =

	void			GetProp01Transform ( XTransform & );
	void			GetProp02Transform ( XTransform & );

	void			GetLFootTransform ( const int index, XTransform & );
	void			GetRFootTransform ( const int index, XTransform & );

	void			GetEyecamTransform ( XTransform & );
	void			GetHeadTransform   ( XTransform & );

	void			GetBoneTransform ( const int index, XTransform & );

	// = Interface to set the rotations and transforms of certain bones =

	void			SetLookAtPosition ( const Vector3d & );
	void			SetFaceAtPosition ( Vector3d const& );
	void			SetFaceAtRotation ( const Rotator & );
	void			SetLookAtSpeed ( const float headSpeed, const float eyeSpeed );

	void			FixAimingAnglesHack ( const Rotator & aimRotator );
	//void			SetLookAtRotations ( const Rotator & head, const Rotator & torso );
	void			SetLookAtRotations ( const Vector3d & headEuler, const Rotator & torso );

	void			SetGlanceAtPosition ( const Vector3d & );
	void			SetEyeRotation ( const Rotator & eyes );

	void			SetSplitFacing ( const bool );

	// = General offset (Not used anymore. Motion extrapolation now used.) =

	void			SetVerticalOffset ( const ftype );

	// = Interface to control the animation =

	// Play a scripted animation
	bool			PlayScriptedAnimation ( const string &sAnimName, const float fFramesPerSecond= 30.0f );
	// Play an animation - general use. Will interrupt layer 0 sequences (except from when called from sequence manager).
	bool			SetNextBlendingValue ( const float nNextBlendSpeed );
	bool			PlayAnimation ( const string &sAnimType, const float fArg= 0.0f, const float fAnimSpeed=0, const float fAttackSkip=0, const int nArm=0 );
	bool			PlayAnimationIdle ( const string &sAnimType, const float fArg= 0.0f, const float fAnimSpeed=0, const float fAttackSkip=0, const int nArm=0 );
	// Sets the ACTION to play every step automatically.
	bool			SetMoveAction ( const string & );
	bool			SetMoveAnimation ( const string & );
	bool			SetIdleAnimation ( const string & );
	// Sets the motion used to influence animation played.
	bool			SetAnimationMotion ( const Vector3d & );

	// Stops an animation
	void			StopAnimation ( const string& );
	// Stop an animation's playing
	void			StopAnimationPlaying ( const string& );
	// Stops movement animation
	void			StopMoveAnimation ( const string & );
	// Scales movement animation
	void			SetMoveAnimationSpeed ( const ftype );

	// Interface to get animation information. Useful stuff for combo systems and other animation cancelling items
	// Returns -1 if no applicable animation is playing. Returns 100- for preswing, returns 100+ for backswing
	int				GetMeleeAttackFrame ( const short hand );
	// Returns the length of the target animation
	int				GetMeleeAttackLength ( const string& sAnimLength );

	// Interface to control facial morphs
	void			PlayMorphTrack ( const string & );
	virtual CMorpher*		GetFaceMorpher ( void ) { return NULL; }

	// Interface to edit sequences
	void			AddSequence ( const char* n_filename, const char* n_sequencename, const uchar n_layer, const bool n_loopoverride=false );

	// Interface to control holding IK
	void			SetAimerIK ( const uchar nArmIndex, const Item::HoldType& nHoldType );

	// Interface to control ragdolling
	void			BlendToRagdoll ( ftype time );
	void			BlendToAnimation ( ftype time );

	// = Get Model State =

	const CSkinnedModel*	GetModelLowLevel ( void );
	CAnimation*		GetAnimationState ( void );
	const Core::TransformLite*		GetSkeletonRoot ( void );
	const Rotator&	GetModelRotation ( void );

	// = Edit Model State =

	// Create the hitbox collision. This is not needed to be called for Referenced models (MCC or RF).
	void			CreateHitboxCollision ( void );

	// = Edit events =

	void			RemoveAnimationEvent ( const Animation::eAnimSystemEvent& nTypeToDelete );

protected:
	// Returns the "basename" or character name of the current model set.
	// This is used in derived classes to see what model subsets to load.
	const string&	GetBasename ( void ) { return basename; }

	// virtual Load ( )
	// This is called when the charModel value is guaranteed to not be NULL.
	// Thus, this can be used to load parts dependant on the basename.
	virtual void	Load ( void ) {;};
	// virtual SetBaseModel ( string )
	// This is called before Load, and is used to modify the base model that is loaded.
	virtual void	SetBaseModel ( string& str ) {;};

	// Character and model that derived classes have access to
	CCharacter*			actor;
	CSkinnedModel*		charModel;
	// Target model, the model used to grabbing transforms, is also visible to derived classes.
	CSkinnedModel*		charTargetModel;
	// Ragdoll/Hitbox collision. Used for both hitboxes and ragdoll animation
	CRagdollCollision*	charRagdoll;
	// Animator for the actor
	CAnimation*			animator;
	CHKAnimation*		hkanimator;
	
	// Also the hiding aspect needs to be accessed
	bool			bShowModel;
	ftype			fModelAlpha;

	// Resets all the transform trackers back to NULL. They are looked up again when queried.
	void	ResetTransformTrackers ( void );

	// These are identifiers keeping track of the transforms.
	Core::TransformLite* pEyeL;
	Core::TransformLite* pEyeR;
	Core::TransformLite* pHead;
	Core::TransformLite* pNeck;
	Core::TransformLite* pSpine0;
	Core::TransformLite* pSpine1;
	Core::TransformLite* pSpine2;
	Core::TransformLite* pSpine3;
	Core::TransformLite* pUpperArmL;
	Core::TransformLite* pUpperArmR;
	Core::TransformLite* pProp1;
	Core::TransformLite* pProp2;
	Core::TransformLite* pProp3;
	Core::TransformLite* pProp4;

	Core::TransformLite* pFootL0;
	Core::TransformLite* pFootL1;
	Core::TransformLite* pFootR0;
	Core::TransformLite* pFootR1;

	Core::TransformLite* getTransformLite ( const char* n_bone_name );

	// This is the list of sequences that the character will look at
	std::vector<Animation::Sequence*>	sequences;
	// This is the currently active sequence
	Animation::Sequence*		currentSequence;

	// Get the current ragdoll strength. This is to edit IK controlled by children, used to make sure they don't interfere.
	Real			GetRagdollStrength ( void ) { return fRagdollStrength; }

private: // Derived classes do NOT muck with the animations, however
	
	// Configures the animation actions that are common to the current model set.
	void			ConfigureAnimations ( void );
	// Configures the animation actions that are common to most models.
	// This function does not throw std::out_of_range on non-standard sets like ConfigureAnimations.
	void			ConfigureCommonAnimations ( void );

	// Does work on animations that need to be updated every frame
	void			UpdateAnimations ( void );

	// Settings
	bool	useDirectionalMovementAnimation;

	// Variables
	string		sCurrentMoveAnim;	// Current animation to play (set during FixedStep so needs to be saved for RenderStep)
	Vector3d	vMotionVelocity;	// Current velocity to add to the animation (might be set during FixedStep so needs to be saved but needed to be saved anyways)
	Vector3d	vPlacementOffset;	// World offset to add to the character model position
	int			iFallType;		// Current fall animation to use
	bool		bSplitFacing;	// If the facing IK should be split between hips and torso. Default 0
	ftype		fAnimCooldown;
	float		fNextBlendSpeed;

	// Blendout list
	struct blendoutAnimation_t
	{
		// Action to blend out
		CAnimAction*	action;
		// Frames to blend in over
		Real			in;
		// Frames to blend out over
		Real			out;
		
		blendoutAnimation_t ( void ) :
			action(NULL), in(0), out(0)
		{
			;
		}
	};
	std::vector<blendoutAnimation_t>	blendinoutList;
	std::vector<blendoutAnimation_t> blendstoppedList;
	// Add animation to the blendin/blendout list
	void AddAnimationBlendout ( const char* n_anim, const Real n_fadeIn, const Real n_fadeOut );
	void AddAnimationBlendoutPct ( const char* n_anim, const Real n_fadeInPct, const Real n_fadeOutPct ); // Relies on AddAnimationBlendout
	// Add animation to the blendstopped list
	void AddAnimationBlendstopped ( const char* n_anim, const Real n_fadeOutTime );

	// Aimer lock
	enum eAimerStateLocks {
		LockNone,
		LockMajor,
		LockAllButNeck,
		LockAllButHead,
		LockAll
	};
	eAimerStateLocks iIdleType;		// How the player is idling.
	bool		bIsIdling;

	string		basename;		// Model set basename. Is empty for non-standard sets.

	// IK and facing
	Rotator		faceatRotation;
	Rotator		faceatRotationFrom;
	bool		bTurnToFace;
	float		fTurnToTimer;

	Rotator		lookatRotation;

	Vector3d	lookatHeadTarget;
	Vector3d	lookatHeadPosition;
	Vector3d	lookatHeadPositionS;
	float		lookatHeadTimer;
	Vector3d	lookatEyePosition;
	Vector3d	lookatEyePositionS;
	float		lookatEyeTimer;

	Rotator		faceatPosRotation;
	Rotator		faceatPosRotationTarget;
	float		faceatPosTimer;

	float		lookatHeadSpeed;
	float		lookatEyeSpeed;

	bool		m_IKFootEnable;

	float		fRagdollBlendSpeed;
	float		fRagdollTargetStrength;
	float		fRagdollStrength;

	bool		bPerformSequenceInterrupts;

	// Events
	std::vector<Animation::ActionEvent> animEventList;

};

#endif
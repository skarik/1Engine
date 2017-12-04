#ifndef M04_INTERFACES_CHARACTER_CONTROLLER_PLATFORMER_H_
#define M04_INTERFACES_CHARACTER_CONTROLLER_PLATFORMER_H_

#include "core/math/Math3d.h"
#include "physical/physics/cast/PrCast.h"

class PrShape;
class CRigidbody;
class CInputControl;

namespace M04
{
	enum grPlatformerMotionState
	{
		kPMotionStateDefault,
		kPMotionStateWallStick,
		kPMotionStateAutoVault,
	};
	
	struct grPlatformControllerOptions
	{
		float gravity;

		float runSpeed;
		float jumpSpeed;

		float runAcceleration;
		float runDecceleration;

		float airAcceleration;
		float airDecceleration;
	};

	// This macro controls how the motion in the platform controller is performed.
	// Kinematic is very stable, but the interaction with physics objects can be seen, in a way, as undesirable.
#	define M04_KINEMATIC_PLATFORM_CONTROLLER

	class CharacterControllerPlatformer
	{
	public:
		explicit	CharacterControllerPlatformer ( void* owner );
					~CharacterControllerPlatformer ( void );

		//	InitCollision() : Set up collision state w/ given default hull size.
		// Call after setting m_tracked_position and m_tracked_velocity.
		void		InitCollision ( Vector2f hullSize );

		void		Step ( void );
		void		PhysicsStep ( void );

	public:

		bool		CheckIsOnGround ( void )
			{ return m_onGround; }

	public:
		void*			m_owner;
		CInputControl*	m_input;

		Vector3f*	m_tracked_position;
		Vector3f*	m_tracked_velocity;
		Vector3f	m_acculated_offset;

		grPlatformControllerOptions	m_opt;

		grPlatformerMotionState	m_motionState;
		grPlatformerMotionState	m_motionStatePrevious;

	protected:

		//	UTILMoveContactY () : Moves in Y direction to the position of the collision (until contact)
		void		UTILMoveContactY ( PrCast& cast, prShapecastQuery& query, Real contactOffset );
		//	UTILMoveContactY () : Moves in X direction to the position of the collision  (until contact)
		void		UTILMoveContactX ( PrCast& cast, prShapecastQuery& query, Real contactOffset );

		//	COMCheckGround () : Common check for ground. Updates m_onGround.
		// Also will moveContactY() with the ground when falling, for additional stability.
		void		COMCheckGround ( void );
		//	COMCollideY () : Common check for y-collision. 
		void		COMCollideY ( void );
		//	COMCollideX () : Common check for x-collision.
		void		COMCollideX ( void );

		//	SUBCheckWallStickStart () : Check for wall stick.
		// Checks wall stick collision in the given X direction. If returns true, then wall stick is valid.
		bool		SUBCheckWallStickStart ( Real checkDirection );
		//	SUBCheckAutoVaultStart () : Check for autovault.
		// Checks autovault in the given X direction. If returns true, then autovault is valid and ready.
		bool		SUBCheckAutoVaultStart ( Real checkDirection );
		
		grPlatformerMotionState	MSDefault ( void );
		grPlatformerMotionState	MSWallStick ( void );
		grPlatformerMotionState	MSAutoVault ( void );

	protected:
		PrShape*	m_hullShape;
		CRigidbody*	m_body;

		Vector2f	m_hullSize;
		PrShape*	m_hullShapeHorizonalCheck;
		PrShape*	m_hullShapeVerticalCheck;
		PrShape*	m_hullShapeHalfvertCheck;

		bool		m_inputConsumed;

		bool		m_onGround;
		bool		m_jumpBegin;

		Vector2f	m_wallstickReference;
		Vector2f	m_wallstickNormal;

		Vector2f	m_autovaultReference;
		Vector2f	m_autovaultTarget;
	};
}

#endif//M04_INTERFACES_CHARACTER_CONTROLLER_PLATFORMER_H_
#ifndef M04_INTERFACES_CHARACTER_CONTROLLER_PLATFORMER_H_
#define M04_INTERFACES_CHARACTER_CONTROLLER_PLATFORMER_H_

#include "core/math/Math3d.h"

class PrShape;
class CRigidbody;
class CInputControl;

namespace M04
{
	enum grPlatformerMotionState
	{
		kPMotionStateDefault,
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
		void*			m_owner;
		CInputControl*	m_input;

		Vector3d*	m_tracked_position;
		Vector3d*	m_tracked_velocity;

		grPlatformControllerOptions	m_opt;

		grPlatformerMotionState	m_motionState;
		grPlatformerMotionState	m_motionStatePrevious;

	protected:

		//	COMCheckGround () : Common check for ground. Updates m_onGround.
		void		COMCheckGround ( void );
		//	COMCollideY () : Common check for y-collision. 
		void		COMCollideY ( void );
		//	COMCollideX () : Common check for x-collision.
		void		COMCollideX ( void );

		grPlatformerMotionState	MSDefault ( void );

	protected:
		PrShape*	m_hullShape;
		CRigidbody*	m_body;

		bool		m_inputConsumed;

		bool		m_onGround;
	};
}

#endif//M04_INTERFACES_CHARACTER_CONTROLLER_PLATFORMER_H_
#include "CharacterControllerPlatformer.h"

#include "core/math/Math.h"
#include "core/time/Time.h"
#include "core-ext/input/CInputControl.h"

#include "physical/interface/tobt.h"
#include "physical/physics/PrPhysics.h"
#include "physical/physics/shapes/PrShapeBox.h"
#include "physical/physics/cast/PrCast.h"

#include "engine/physics/motion/CRigidbody.h"

M04::CharacterControllerPlatformer::CharacterControllerPlatformer ( void* owner )
	: m_owner(owner),
	m_body(NULL), m_input(NULL),
	m_inputConsumed(false),
	m_motionState(kPMotionStateDefault)
{
	m_motionStatePrevious = m_motionState;
}

M04::CharacterControllerPlatformer::~CharacterControllerPlatformer ( void )
{
	delete_safe_decrement(m_body);
	delete_safe(m_hullShape);
	delete_safe(m_hullShapeHorizonalCheck);
	delete_safe(m_hullShapeVerticalCheck);
	delete_safe(m_hullShapeHalfvertCheck);
}

void M04::CharacterControllerPlatformer::InitCollision ( Vector2f hullSize )
{
	if (m_tracked_position == NULL || m_tracked_velocity == NULL || m_input == NULL)
	{
		throw core::InvalidCallException();
	}

	m_hullSize = hullSize;

	// Create the hulls needed for different checks (due to pixel precision)
	m_hullShapeHorizonalCheck = new PrShapeBox( Vector2f(hullSize.x, hullSize.y - 2) );
	m_hullShapeVerticalCheck = new PrShapeBox( Vector2f(hullSize.x - 2, hullSize.y) );
	m_hullShapeHalfvertCheck = new PrShapeBox( Vector2f(hullSize.x - 2, hullSize.y * 0.5F) );

	// Create shape for the rigidbody
	m_hullShape = new PrShapeBox( hullSize ); 

	// Make the rigidbody:
	prRigidbodyCreateParams params = {0};
	params.shape = m_hullShape;
	params.owner = m_owner;
	params.ownerType = core::kBasetypeGameBehavior;
	params.group = physical::layer::PHYS_CHARACTER;
	params.mass = 30.0F;

	m_body = new CRigidbody(params);
	m_body->target_position = m_tracked_position;

	// Set initial position
	m_body->world_transform.position = *m_tracked_position;
	m_body->PushTransform();
	m_body->ApiBody()->setActivationState(DISABLE_DEACTIVATION);

	// Disable rotation & Z motion
	m_body->ApiBody()->setAngularFactor(btVector3(0,0,0));
	m_body->ApiBody()->setLinearFactor(btVector3(1,1,0));

#	ifdef M04_KINEMATIC_PLATFORM_CONTROLLER
	m_body->SetMotionType( physical::motion::kMotionKinematic );
#	endif
}

//===============================================================================================//
// STEPPING THRU GAME LOOP

void M04::CharacterControllerPlatformer::Step ( void )
{
	if (m_inputConsumed)
	{
		m_input->Update(m_owner, Time::deltaTime);
		m_inputConsumed = false;
	}
}

void M04::CharacterControllerPlatformer::PhysicsStep ( void )
{
	// Zero out the offset
	{
		Real zeroing_speed = Time::deltaTime * m_opt.jumpSpeed;
		for (int i = 0; i < 3; ++i)
		{
			if ( fabsf(m_acculated_offset[i]) > zeroing_speed )
				m_acculated_offset[i] -= math::sgn(m_acculated_offset[i]) * zeroing_speed;
			else
				m_acculated_offset[i] = 0.0F;
		}
	}

	if (m_body != NULL)
	{
		// Update motion:
		grPlatformerMotionState prevState = m_motionState;
		grPlatformerMotionState nextState = m_motionState;
		switch (m_motionState)
		{
		case kPMotionStateDefault:		nextState = MSDefault();
			break;
		case kPMotionStateWallStick:	nextState = MSWallStick();
			break;
		case kPMotionStateAutoVault:	nextState = MSAutoVault();
			break;
		default:
			throw core::InvalidCallException();
		}
		// Update state:
		if (nextState != prevState)
		{
			m_motionStatePrevious = prevState;
			m_motionState = nextState;
		}
		// Move the character:
#		ifdef M04_KINEMATIC_PLATFORM_CONTROLLER
		*m_tracked_position += *m_tracked_velocity * Time::deltaTime;
#		endif

	}
	m_inputConsumed = true;
}

//===============================================================================================//
// UTILITIES

//	UTILMoveContactY () : Moves in Y direction to the position of the collision (until contact)
void M04::CharacterControllerPlatformer::UTILMoveContactY ( PrCast& cast, prShapecastQuery& query, Real contactOffset )
{
	Real new_y = math::lerp(cast.HitFraction<0>(), query.start.position.y, query.end.position.y) + contactOffset;
#	ifdef M04_KINEMATIC_PLATFORM_CONTROLLER
	m_tracked_position->y = new_y;
#	else
	btVector3 origin = m_body->ApiBody()->getWorldTransform().getOrigin();
	origin.setY( new_y );
	m_body->ApiBody()->getWorldTransform().setOrigin(origin);
#	endif
}

//	UTILMoveContactY () : Moves in X direction to the position of the collision  (until contact)
void M04::CharacterControllerPlatformer::UTILMoveContactX ( PrCast& cast, prShapecastQuery& query, Real contactOffset )
{
	Real new_x = math::lerp(cast.HitFraction<0>(), query.start.position.x, query.end.position.x) + contactOffset;
#	ifdef M04_KINEMATIC_PLATFORM_CONTROLLER
	m_tracked_position->x = new_x;
#	else
	btVector3 origin = m_body->ApiBody()->getWorldTransform().getOrigin();
	origin.setX( new_x );
	m_body->ApiBody()->getWorldTransform().setOrigin(origin);
#	endif
}

//===============================================================================================//
// COMMON ROUTINES

//	COMCheckGround () : Common check for ground. Updates m_onGround.
// Also will moveContactY() with the ground when falling, for additional stability.
void M04::CharacterControllerPlatformer::COMCheckGround ( void )
{
	prShapecastQuery query = {0};
	query.shape		= m_hullShapeVerticalCheck;
	query.start		= XrTransform(*m_tracked_position);
	query.end		= XrTransform(*m_tracked_position + Vector3f(0, 2, 0));
	query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
	query.owner		= m_owner;
	query.ownerType	= core::kBasetypeGameBehavior;
	query.ignore	= m_body;
	query.maxHits	= 1;
	
	PrCast cast(query);
	if (cast)
	{
		if (!m_onGround)
		{
			m_onGround = true;
			if ( m_tracked_velocity->y > FLOAT_PRECISION )
			{
				// Push along the cast
				m_tracked_velocity->y = 0;
				// Move to contact
				UTILMoveContactY(cast, query, (Real)-math::sgn(m_tracked_velocity->y));
			}
		}
	}
	else
	{
		m_onGround = false;
	}
}

//	COMCollideY () : Common check for y-collision. 
void M04::CharacterControllerPlatformer::COMCollideY ( void )
{
	if ( fabsf(m_tracked_velocity->y) > FLOAT_PRECISION )
	{
		prShapecastQuery query = {0};
		query.shape		= m_hullShapeVerticalCheck;
		query.start		= XrTransform(*m_tracked_position);
		query.end		= XrTransform(*m_tracked_position + Vector3f(0, m_tracked_velocity->y * Time::deltaTime, 0));
		query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
		query.owner		= m_owner;
		query.ownerType	= core::kBasetypeGameBehavior;
		query.ignore	= m_body;
		query.maxHits	= 1;

		PrCast cast(query);
		if (cast)
		{
			// Push along the cast
			m_tracked_velocity->y = 0;
			// Move to contact
			UTILMoveContactY(cast, query, (Real)-math::sgn(m_tracked_velocity->y));
		}
	}
}

//	COMCollideX () : Common check for x-collision.
void M04::CharacterControllerPlatformer::COMCollideX ( void )
{
	if ( fabsf(m_tracked_velocity->x) > FLOAT_PRECISION )
	{
		bool move_free = false; // Has an open spot been found?
		
		const int maxStairHeight = 23;
		const int minStairHeight = -23;
		
		// Check for stairs
		if ( m_input->vDirInput.y > -0.707F )
		{
			for (int i = 0; i < maxStairHeight; ++i)
			{
				prShapecastQuery query = {0};
				query.shape		= m_hullShapeHorizonalCheck;
				query.start		= XrTransform(*m_tracked_position + Vector3f(0, -(Real)i, 0));
				query.end		= XrTransform(*m_tracked_position + Vector3f(m_tracked_velocity->x * Time::deltaTime + math::sgn(m_tracked_velocity->x), -(Real)i, 0));
				query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
				query.owner		= m_owner;
				query.ownerType	= core::kBasetypeGameBehavior;
				query.ignore	= m_body;
				query.maxHits	= 1;

				PrCast cast(query);
				if (cast == false)
				{
					move_free = true;
					m_tracked_position->y -= (Real)i;
					m_acculated_offset.y -= (Real)i;
					break;
				}
			}
		}
		// Check for down stairs
		if ( m_onGround && m_tracked_velocity->y > -FLOAT_PRECISION )
		{
			for (int i = 1; i > minStairHeight; --i)
			{
				prShapecastQuery query = {0};
				query.shape		= m_hullShapeHorizonalCheck;
				query.start		= XrTransform(*m_tracked_position + Vector3f(0, -(Real)i, 0));
				query.end		= XrTransform(*m_tracked_position + Vector3f(m_tracked_velocity->x * Time::deltaTime + math::sgn(m_tracked_velocity->x), -(Real)i, 0));
				query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
				query.owner		= m_owner;
				query.ownerType	= core::kBasetypeGameBehavior;
				query.ignore	= m_body;
				query.maxHits	= 1;

				PrCast cast(query);
				if (cast == false)
				{
					move_free = true;
					m_tracked_position->y -= ((Real)i) - 1.0F;
					m_acculated_offset.y -= ((Real)i) - 1.0F;
					break;
				}
			}
		}
		// If no open spot, stop
		if ( !move_free )
		{
			prShapecastQuery query = {0};
			query.shape		= m_hullShapeHorizonalCheck;
			query.start		= XrTransform(*m_tracked_position + Vector3f(0, 0, 0));
			query.end		= XrTransform(*m_tracked_position + Vector3f(m_tracked_velocity->x * Time::deltaTime + math::sgn(m_tracked_velocity->x), 0, 0));
			query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
			query.owner		= m_owner;
			query.ownerType	= core::kBasetypeGameBehavior;
			query.ignore	= m_body;
			query.maxHits	= 1;

			PrCast cast(query);
			if (cast)
			{
				// Push along the cast
				m_tracked_velocity->x = 0;
				// Move to contact
				UTILMoveContactX(cast, query, (Real)-math::sgn(m_tracked_velocity->x));
			}
			else
			{
				printf("Possible invalid state found in the character X collision routine.\n");
			}
		}
	}
}

//===============================================================================================//
// SUBROUTINE CHECKS

//	SUBCheckWallStickStart () : Check for wall stick.
// Checks wall stick collision in the given X direction. If returns true, then wall stick is valid.
bool M04::CharacterControllerPlatformer::SUBCheckWallStickStart ( Real checkDirection )
{
	checkDirection = (Real)math::sgn<Real>(checkDirection);

	// Create repeated query options.
	prShapecastQuery query = {0};
	query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
	query.owner		= m_owner;
	query.ownerType	= core::kBasetypeGameBehavior;
	query.ignore	= m_body;
	query.maxHits	= 1;

	// We need a bit of wall at our head:
	query.shape		= m_hullShapeHalfvertCheck;
	query.start		= XrTransform(*m_tracked_position + Vector3f(0 * checkDirection, -m_hullSize.y * 0.75F + 2, 0));
	query.end		= XrTransform(*m_tracked_position + Vector3f(4 * checkDirection, -m_hullSize.y * 0.75F + 2, 0));
	
	if (PrCast(query))
	{
		query.shape		= m_hullShapeHalfvertCheck;
		query.start		= XrTransform(*m_tracked_position + Vector3f(0 * checkDirection, 0, 0));
		query.end		= XrTransform(*m_tracked_position + Vector3f(4 * checkDirection, 0, 0));

		PrCast wallCast(query);
		if (wallCast)
		{
			m_wallstickReference.y = m_tracked_position->y;
			m_wallstickReference.x = math::lerp(wallCast.HitFraction<0>(), query.start.position.x, query.end.position.x);
			m_wallstickNormal.y = 0;
			m_wallstickNormal.x = -checkDirection;
			return true;
		}
	}

	return false;
}

//	SUBCheckAutoVaultStart () : Check for autovault.
// Checks autovault in the given X direction. If returns true, then autovault is valid and ready.
bool M04::CharacterControllerPlatformer::SUBCheckAutoVaultStart ( Real checkDirection )
{
	checkDirection = (Real)math::sgn<Real>(checkDirection);

	// Create repeated query options.
	prShapecastQuery query = {0};
	query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
	query.owner		= m_owner;
	query.ownerType	= core::kBasetypeGameBehavior;
	query.ignore	= m_body;
	query.maxHits	= 1;

	// Need to first check for blocked forward motion
	query.shape		= m_hullShapeHorizonalCheck;
	query.start		= XrTransform(*m_tracked_position + Vector3f( 0 * checkDirection, 0, 0));
	query.end		= XrTransform(*m_tracked_position + Vector3f(16 * checkDirection, 0, 0));

	if (PrCast(query))
	{
		// Need to then check for clear forward area
		query.shape		= m_hullShapeHalfvertCheck;
		query.start		= XrTransform(*m_tracked_position + Vector3f( 0 * checkDirection, -m_hullSize.y * 0.25F - 2, 0));
		query.end		= XrTransform(*m_tracked_position + Vector3f(32 * checkDirection, -m_hullSize.y * 0.25F - 2, 0));

		PrCast clearCast(query);
		if (clearCast == false)
		{
			m_autovaultReference = *m_tracked_position;
			m_autovaultTarget = *m_tracked_position + Vector3f(32 * checkDirection, -32.0F, 0);
			return true;
		}
	}

	return false;
}
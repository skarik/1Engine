#include "CharacterControllerPlatformer.h"

#include "core/math/Math.h"
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
}

void M04::CharacterControllerPlatformer::InitCollision ( Vector2f hullSize )
{
	if (m_tracked_position == NULL || m_tracked_velocity == NULL || m_input == NULL)
	{
		throw core::InvalidCallException();
	}

	PrShape* box = new PrShapeBox( hullSize ); 
	m_hullShape = box;

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
}

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
	if (m_body != NULL)
	{
		// Update motion:
		grPlatformerMotionState prevState = m_motionState;
		grPlatformerMotionState nextState = m_motionState;
		switch (m_motionState)
		{
		case kPMotionStateDefault:	nextState = MSDefault();
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
	}
	m_inputConsumed = true;
}


void M04::CharacterControllerPlatformer::COMCheckGround ( void )
{
	m_onGround = false;

	prShapecastQuery query = {0};
	query.shape		= m_hullShape;
	query.start		= XrTransform(*m_tracked_position);
	query.end		= XrTransform(*m_tracked_position + Vector3f(0, 2, 0));
	query.collision	= physical::prCollisionMask(physical::layer::MASK_CHARACTER | physical::layer::MASK_LANDSCAPE, 0, 0);
	query.owner		= m_owner;
	query.ownerType	= core::kBasetypeGameBehavior;
	query.ignore	= m_body;
	query.maxHits	= 1;
	
	// Cast.
	if (PrCast(query))
	{
		m_onGround = true;
	}
}


void M04::CharacterControllerPlatformer::COMCollideY ( void )
{
	if ( fabsf(m_tracked_velocity->y) > FLOAT_PRECISION )
	{
		prShapecastQuery query = {0};
		query.shape		= m_hullShape;
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
			btVector3 origin = m_body->ApiBody()->getWorldTransform().getOrigin();
			origin.setY( math::lerp(cast.HitFraction<0>(), query.start.position.y, query.end.position.y) );
			m_body->ApiBody()->getWorldTransform().setOrigin(origin);
		}
	}
}

void M04::CharacterControllerPlatformer::COMCollideX ( void )
{
	if ( fabsf(m_tracked_velocity->x) > FLOAT_PRECISION )
	{
		prShapecastQuery query = {0};
		query.shape		= m_hullShape;
		query.start		= XrTransform(*m_tracked_position);
		query.end		= XrTransform(*m_tracked_position + Vector3f(m_tracked_velocity->x * Time::deltaTime, 0, 0));
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
			btVector3 origin = m_body->ApiBody()->getWorldTransform().getOrigin();
			origin.setX( math::lerp(cast.HitFraction<0>(), query.start.position.x, query.end.position.x) );
			m_body->ApiBody()->getWorldTransform().setOrigin(origin);
		}
	}
}

M04::grPlatformerMotionState M04::CharacterControllerPlatformer::MSDefault ( void )
{
	// Update velocity first:
	btVector3 bt_velocity = m_body->ApiBody()->getLinearVelocity();
	*m_tracked_velocity = physical::ar(bt_velocity);

	// Perform vertical common:
	COMCheckGround();

	// Apply gravity
	if ( !m_onGround )
	{
		m_tracked_velocity->y += Time::deltaTime * m_opt.gravity;
	}

	// Apply horizontal acceleration
	if ( fabsf(m_input->vDirInput.x) > FLOAT_PRECISION )
	{
		// Move left n right
		float move_accel = m_onGround ? m_opt.runAcceleration : m_opt.airAcceleration;
		if ( m_input->vDirInput.x < 0 )
		{
			m_tracked_velocity->x = std::max( m_tracked_velocity->x - move_accel * Time::deltaTime, -m_opt.runSpeed );
		}
		else if ( m_input->vDirInput.x > 0 )
		{
			m_tracked_velocity->x = std::min( m_tracked_velocity->x + move_accel * Time::deltaTime, +m_opt.runSpeed );
		}
	}
	// Apply horizontal decceleration
	else
	{
		float move_deccel = m_onGround ? m_opt.runDecceleration : m_opt.airDecceleration;
		// Stop movement
		m_tracked_velocity->x = std::max( fabsf(m_tracked_velocity->x) - move_deccel * Time::deltaTime, 0.0F ) * math::sgn( m_tracked_velocity->x );
	}

	// Perform jumping
	if ( m_onGround )
	{
		if ( m_input->axes.jump.pressed() )
		{
			m_onGround = false;
			m_tracked_velocity->y = -m_opt.jumpSpeed;
		}
	}

	// Perform collision for Y
	COMCollideY();
	// Perform horizonal collision
	COMCollideX();

	// Update physics end
	m_body->ApiBody()->setLinearVelocity( physical::bt(*m_tracked_velocity) );

	return kPMotionStateDefault;
}
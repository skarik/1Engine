#include "CharacterControllerPlatformer.h"

#include "core/math/Math.h"
#include "core/time/Time.h"
#include "core-ext/input/CInputControl.h"

//===============================================================================================//
// MOVETYPES

M04::grPlatformerMotionState M04::CharacterControllerPlatformer::MSDefault ( void )
{
	// Update velocity first:
#	ifndef M04_KINEMATIC_PLATFORM_CONTROLLER
	btVector3 bt_velocity = m_body->ApiBody()->getLinearVelocity();
	*m_tracked_velocity = physical::ar(bt_velocity);
#	endif

	// Perform vertical common:
	COMCheckGround();

	// Apply gravity
	if ( !m_onGround )
	{
		m_tracked_velocity->y += Time::deltaTime * m_opt.gravity;
	}
	else
	{
		m_jumpBegin = false;
	}

	// Apply horizontal acceleration
	if ( fabsf(m_input->vDirInput.x) > FLOAT_PRECISION )
	{
		// Move left n right
		float move_accel = m_onGround ? m_opt.runAcceleration : m_opt.airAcceleration;
		if ( m_input->vDirInput.x < 0 )
		{
			m_tracked_velocity->x = std::max( m_tracked_velocity->x - move_accel * Time::deltaTime, m_opt.runSpeed * m_input->vDirInput.x );
		}
		else if ( m_input->vDirInput.x > 0 )
		{
			m_tracked_velocity->x = std::min( m_tracked_velocity->x + move_accel * Time::deltaTime, m_opt.runSpeed * m_input->vDirInput.x );
		}

		// Do wall stick if falling and not on ground
		if ( !m_onGround && m_tracked_velocity->y > FLOAT_PRECISION )
		{
			if ( SUBCheckWallStickStart(m_input->vDirInput.x) )
			{
				SUBWallStickStart(m_input->vDirInput.x);
				return kPMotionStateWallStick;
			}
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
			m_jumpBegin = true;
		}
	}

	// Perform collision for Y
	COMCollideY();
	// Perform horizonal collision
	COMCollideX();

	// Update physics end
#	ifndef M04_KINEMATIC_PLATFORM_CONTROLLER
	m_body->ApiBody()->setLinearVelocity( physical::bt(*m_tracked_velocity) );
#	endif

	return kPMotionStateDefault;
}

M04::grPlatformerMotionState M04::CharacterControllerPlatformer::MSWallStick ( void )
{
	// Stop velocity
	m_tracked_velocity->x = 0;
	m_tracked_velocity->y = 0;
	m_onGround = false;

	// Move X to the target gradually:
	m_tracked_position->x = m_wallstickReference.x;

	// Y is statck.
	m_tracked_position->y = m_wallstickReference.y;

	// If we jump, we want to push off!
	if ( m_input->axes.jump.pressed() )
	{
		m_tracked_velocity->y = -m_opt.jumpSpeed;
		m_jumpBegin = true;

		if ( m_wallstickNormal.x > 0 )
		{
			m_tracked_velocity->x = std::max(m_opt.jumpSpeed, m_opt.runSpeed) * std::max( +0.1F, m_input->vDirInput.x );
		}
		else if ( m_wallstickNormal.x < 0 )
		{
			m_tracked_velocity->x = std::max(m_opt.jumpSpeed, m_opt.runSpeed) * std::min( -0.1F, m_input->vDirInput.x );
		}

		return kPMotionStateDefault;
	}

	return kPMotionStateWallStick;
}
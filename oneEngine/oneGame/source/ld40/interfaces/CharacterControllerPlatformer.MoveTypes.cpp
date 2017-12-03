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
				return kPMotionStateWallStick;
			}
		}
		// Do autovaulting if on the ground
		if ( m_onGround )
		{
			if ( SUBCheckAutoVaultStart(m_input->vDirInput.x) )
			{
				return kPMotionStateAutoVault;
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
	Real delta_x = m_wallstickReference.x - m_tracked_position->x;
	m_tracked_position->x += delta_x;
	m_acculated_offset.x += delta_x;

	// Y is statck.
	Real delta_y = m_wallstickReference.y - m_tracked_position->y;
	m_tracked_position->y += delta_y;
	m_acculated_offset.y += delta_y;

	// If we jump, we want to push off!
	if ( m_input->axes.jump.pressed() )
	{
		m_tracked_velocity->y = m_opt.jumpSpeed * std::max(-1.0F, -m_input->vDirInput.y - 1.0F);
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

M04::grPlatformerMotionState M04::CharacterControllerPlatformer::MSAutoVault ( void )
{
	// Set velocity to move torwards the autovault target.
	m_onGround = true;

	// Get how long it should take to vault this.
	const Real referenceTiming = (m_autovaultReference - m_autovaultTarget).magnitude() / m_opt.runSpeed;

	// Set vertical velocity
	{
		Real target_y_velocity = (m_autovaultTarget.y - m_autovaultReference.y) / referenceTiming * 1.15F;
		// Stop going up if we're too high
		if (m_tracked_position->y < m_autovaultTarget.y + 8)
		{
			target_y_velocity = 0;
		}
		// Perform normal delta towards target
		Real delta = target_y_velocity - m_tracked_velocity->y;
		Real delta_speed = m_opt.runAcceleration * Time::deltaTime;
		if ( fabsf(delta) > delta_speed )
			m_tracked_velocity->y += math::sgn(delta) * delta_speed;
		else
			m_tracked_velocity->y += delta;
	}
	// Set horizontal velocity
	{
		Real target_x_velocity = (m_autovaultTarget.x - m_autovaultReference.x) / referenceTiming;
		// Perform normal delta towards target
		Real delta = target_x_velocity - m_tracked_velocity->x;
		Real delta_speed = m_opt.runAcceleration * Time::deltaTime;
		if ( fabsf(delta) > delta_speed )
			m_tracked_velocity->x += math::sgn(delta) * delta_speed;
		else
			m_tracked_velocity->x += delta;
	}

	// Check if past the target:
	if (m_tracked_position->y < m_autovaultTarget.y)
	{
		if (m_autovaultReference.x < m_autovaultTarget.x)
		{
			if (m_tracked_position->x > m_autovaultTarget.x - m_hullSize.x * 0.5F)
			{
				return kPMotionStateDefault;
			}
		}
		else if (m_autovaultReference.x > m_autovaultTarget.x)
		{
			if (m_tracked_position->x < m_autovaultTarget.x + m_hullSize.x * 0.5F)
			{
				return kPMotionStateDefault;
			}
		}
	}

	// If input is opposite of current motion
	if ( fabsf(m_input->vDirInput.x) > FLOAT_PRECISION )
	{
		if ( math::sgn(m_input->vDirInput.x) != math::sgn(m_autovaultReference.x - m_autovaultTarget.x) )
		{
			return kPMotionStateDefault;
		}
	}

	// If we jump, we want to interrupt and jump instead!
	if ( m_input->axes.jump.pressed() )
	{
		m_tracked_velocity->y = -m_opt.jumpSpeed;
		m_jumpBegin = true;
		m_onGround = false;
		return kPMotionStateDefault;
	}

	return kPMotionStateAutoVault;
}
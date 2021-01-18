
#include "UILightHandle.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/system/Screen.h"

#include "render2d/camera/COrthoCamera.h"

#include "m04-editor/renderer/object/UILightHandleRenderer.h"


M04::UILightHandle::UILightHandle ( DrawStyle style )
	: CGameBehavior()
{
	m_style = style;
	m_mode = Mode::Range;

	m_renderer = new UILightHandleRenderer( this );

	m_dragging		= false;
	m_drag_axis		= Axis::None;
	m_drag_axis_hover	= Axis::None;
	//m_position		= Vector3f::zero;
	//m_position_snap	= Vector3f::zero;
}

M04::UILightHandle::~UILightHandle ( void )
{
	delete_safe( m_renderer );
}


void M04::UILightHandle::Update ( void )
{
	if ( m_dragging == false )
	{
		// Grab mouse position in the world to get hover area
		if ( m_style == DrawStyle::s2D )
		{
			Vector3f worldpos = RrCamera::activeCamera->ScreenToWorldPos( Vector2f( core::Input::MouseX()/(Real)Screen::Info.width, core::Input::MouseY()/(Real)Screen::Info.height ) );
			Rect check_rect;

			// Reset hovers
			m_drag_axis_hover = Axis::None;

			// Check if mouse is in the range handle
			check_rect = Rect( -m_range - 4.0F, -4.0F, 8.0F, 8.0F );
			check_rect.pos += m_position;
			if ( check_rect.Contains(worldpos) )
			{
				m_drag_axis_hover = Axis::Range;
			}
			check_rect = Rect( +m_range - 4.0F, -4.0F, 8.0F, 8.0F );
			check_rect.pos += m_position;
			if ( check_rect.Contains(worldpos) )
			{
				m_drag_axis_hover = Axis::Range;
			}

			// Check if mouse is in the power handle
			float power_pos = m_range / m_power;
			check_rect = Rect( -4.0F, -power_pos - 4.0F, 8.0F, 8.0F );
			check_rect.pos += m_position;
			if ( check_rect.Contains(worldpos) )
			{
				m_drag_axis_hover = Axis::Power;
			}
			check_rect = Rect( -4.0F, +power_pos - 4.0F, 8.0F, 8.0F );
			check_rect.pos += m_position;
			if ( check_rect.Contains(worldpos) )
			{
				m_drag_axis_hover = Axis::Power;
			}

		}

		// Check for mouse input
		if ( m_drag_axis_hover != Axis::None )
		{
			if ( core::Input::MouseDown(core::kMBLeft) )
			{
				if ( m_dragging == false )
				{
					m_drag_axis = m_drag_axis_hover;
					m_dragging = true;
					m_drag_start = Vector2f( core::Input::MouseX(), core::Input::MouseY() );

					m_range_start = m_range;
					m_power_start = m_power;
				}
			}
		}
	}
	else
	{
		// Perform the dragging
		m_drag_end = Vector2f( core::Input::MouseX(), core::Input::MouseY() );
		Vector3f t_drag_delta = m_drag_end - m_drag_start;

		// Different modes have different behavior
		if ( m_style == DrawStyle::s2D )
		{
			// 2D mode uses the pixel size as a baseline
			t_drag_delta = t_drag_delta.mulComponents( Vector2f( 0.5F, 0.5F ) );
			// Perform translation motion
			if ( m_mode == Mode::Range )
			{
				if ( m_drag_axis == Axis::Range )
				{
					m_range = m_range_start + t_drag_delta.x;
				}
				else if ( m_drag_axis == Axis::Power )
				{
					float power_pos = m_range / m_power_start;
					m_power = m_range / (power_pos + t_drag_delta.y);
					m_power = std::max( 0.05F, std::min( 32.0F, m_power ) );
				}
			}
		}

		// Check if still dragging
		if ( core::Input::MouseUp(core::kMBLeft) || !core::Input::Mouse(core::kMBLeft) )
		{
			m_dragging = false;
			m_drag_axis = Axis::None;
			// Apply the drag motion
		}
	}
}

//		SetRenderPosition
// Sets the position that being rendered at
void M04::UILightHandle::SetRenderPosition ( const Vector3f& n_newPosition )
{
	m_position = n_newPosition;
}

//		HasFocus
// Is this UI element in focus or currently working?
bool M04::UILightHandle::HasFocus ( void )
{
	return active && (m_dragging || (m_drag_axis_hover != Axis::None));
}

//		SetRange
// Sets the range of the light
void M04::UILightHandle::SetRange ( const float n_newRange )
{
	m_range = n_newRange;
}
//		GetRange
// Gets the range of the light
float M04::UILightHandle::GetRange ( void )
{
	return m_range;
}

//		SetPower
// Sets the power of the light
void M04::UILightHandle::SetPower ( const float n_newPower )
{
	m_power = n_newPower;
}
//		GetPower
// Gets the power of the light
float M04::UILightHandle::GetPower ( void )
{
	return m_power;
}


//		OnEnable
// When enabled, also enables renderer
void M04::UILightHandle::OnEnable ( void )
{
	m_renderer->SetVisible( true );
}
//		OnEnable
// When disabled, also disables renderer
void M04::UILightHandle::OnDisable ( void )
{
	m_renderer->SetVisible( false );
}
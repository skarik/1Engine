
#include "UIDragHandle.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/system/Screen.h"

#include "render2d/camera/COrthoCamera.h"

#include "m04-editor/renderer/object/UIDragHandleRenderer.h"


M04::UIDragHandle::UIDragHandle ( DrawStyle style )
	: CGameBehavior()
{
	m_style = style;
	m_mode = Mode::Translate;

	m_renderer = new UIDragHandleRenderer( this );

	m_dragging		= false;
	m_drag_axis		= Axis::None;
	m_drag_axis_hover	= Axis::None;
	m_position		= Vector3d::zero;
	m_position_snap	= Vector3d::zero;
}

M04::UIDragHandle::~UIDragHandle ( void )
{
	delete_safe( m_renderer );
}


void M04::UIDragHandle::Update ( void )
{
	if ( m_dragging == false )
	{
		// Grab mouse position in the world to get hover area
		if ( m_style == DrawStyle::s2D )
		{
			Vector3d worldpos = CCamera::activeCamera->ScreenToWorldPos( Vector2d( Input::MouseX()/(Real)Screen::Info.width, Input::MouseY()/(Real)Screen::Info.height ) );
			Rect check_rect;

			// Reset hovers
			m_drag_axis_hover = Axis::None;

			// Check x axis
			check_rect = Rect( 4.0F, -4.0F, 60.0F, 8.0F );
			check_rect.pos += m_position;
			if ( check_rect.Contains(worldpos) )
			{
				m_drag_axis_hover = Axis::X;
			}
			// Check Y axis
			check_rect = Rect( -4.0F, -4.0F, 8.0F, -60.0F );
			check_rect.pos += m_position;
			check_rect.Fix(); // Fix signs
			if ( check_rect.Contains(worldpos) )
			{
				m_drag_axis_hover = Axis::Y;
			}
			// Check ALL pan
			check_rect = Rect( 0,0, 16.0F, -16.0F );
			check_rect.pos += m_position;
			check_rect.Fix(); // Fix signs
			if ( check_rect.Contains(worldpos) )
			{
				m_drag_axis_hover = Axis::All;
			}
		}

		// Check for mouse input
		if ( m_drag_axis_hover != Axis::None )
		{
			if ( Input::MouseDown(Input::MBLeft) )
			{
				if ( m_dragging == false )
				{
					m_drag_axis = m_drag_axis_hover;
					m_dragging = true;
					m_drag_start = Vector2d( Input::MouseX(), Input::MouseY() );

					m_position_start = m_position;
					m_rotation_start = m_rotation;
					m_scaling_start  = m_scaling;
				}
			}
		}
	}
	else
	{
		// Perform the dragging
		m_drag_end = Vector2d( Input::MouseX(), Input::MouseY() );
		Vector3d t_drag_delta = m_drag_end - m_drag_start;

		// Different modes have different behavior
		if ( m_style == DrawStyle::s2D )
		{
			// 2D mode uses the pixel size as a baseline
			t_drag_delta = t_drag_delta.mulComponents( Vector2d( 0.5F, 0.5F ) );
			// Perform translation motion
			if ( m_mode == Mode::Translate )
			{
				switch ( m_drag_axis )
				{
				case Axis::X: t_drag_delta.y = 0; break;
				case Axis::Y: t_drag_delta.x = 0; break;
				}
				m_position = m_position_start + t_drag_delta;
				// Round to snapping divs
				if ( m_position_snap.x > 0.1F ) m_position.x = Math.Round( m_position.x / m_position_snap.x ) * m_position_snap.x;
				if ( m_position_snap.y > 0.1F ) m_position.y = Math.Round( m_position.y / m_position_snap.y ) * m_position_snap.y;
			}
		}

		// Check if still dragging
		if ( Input::MouseUp(Input::MBLeft) || !Input::Mouse(Input::MBLeft) )
		{
			m_dragging = false;
			m_drag_axis = Axis::None;
			// Apply the drag motion
		}
	}
}


//		SetSnapping
// When snapping is enabled, sets the divs that the DragHandle snaps to
void M04::UIDragHandle::SetSnapping ( const Vector3d& n_snappingdivs )
{
	if ( m_mode == Mode::Translate )
	{
		m_position_snap = n_snappingdivs;
	}
}

//		SetRenderPosition
// Sets the position that being rendered at
void M04::UIDragHandle::SetRenderPosition ( const Vector3d& n_newPosition )
{
	m_position = n_newPosition;
}

//		HasFocus
// Is this UI element in focus or currently working?
bool M04::UIDragHandle::HasFocus ( void )
{
	return active && (m_dragging || (m_drag_axis_hover != Axis::None));
}
//		GetGizmoPosition
// Returns the position the handle is now being rendered at
Vector3d M04::UIDragHandle::GetGizmoPosition ( void )
{
	return m_position;
}

//		OnEnable
// When enabled, also enables renderer
void M04::UIDragHandle::OnEnable ( void )
{
	m_renderer->SetVisible( true );
}
//		OnEnable
// When disabled, also disables renderer
void M04::UIDragHandle::OnDisable ( void )
{
	m_renderer->SetVisible( false );
}
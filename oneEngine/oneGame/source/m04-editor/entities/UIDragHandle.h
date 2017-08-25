
#ifndef _EDITOR_UI_DRAGHANDLE_H_
#define _EDITOR_UI_DRAGHANDLE_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"

namespace M04
{
	class UIDragHandleRenderer;
	class UIDragHandle : public CGameBehavior
	{
		ClassName("UIDragHandle");
	public:
		//=========================================//
		// Types
		//=========================================//

		enum class DrawStyle : uint8_t
		{
			s2D,
			s3D
		};
		enum class Mode : uint8_t
		{
			Translate,
			Rotate,
			Scale
		};
		enum class Axis : uint8_t
		{
			None,
			All,
			X,
			Y,
			Z
		};

	public:
		explicit	UIDragHandle ( DrawStyle style=DrawStyle::s2D );
					~UIDragHandle( void );

		void		Update ( void ) override;

		//		SetDrawStyle
		// Sets if the drag handle should be a 2D or 3D tool
		void		SetDrawStyle ( const DrawStyle n_drawstyle );
		//		SetMode
		// Sets the transformation mode of the handle
		void		SetMode ( const Mode n_usemode );
		//		SetSnapping
		// When snapping is enabled, sets the divs that the DragHandle snaps to
		void		SetSnapping ( const Vector3d& n_snappingdivs );

		//		SetRenderPosition
		// Sets the position that being rendered at
		void		SetRenderPosition ( const Vector3d& n_newPosition );

		//		HasFocus
		// Is this UI element in focus or currently working?
		bool		HasFocus ( void );

		//		GetGizmoPosition
		// Returns the position the handle is now being rendered at
		Vector3d	GetGizmoPosition ( void );

	protected:

		//		OnEnable
		// When enabled, also enables renderer
		void OnEnable ( void ) override;
		//		OnEnable
		// When disabled, also disables renderer
		void OnDisable ( void ) override;

	protected:
		DrawStyle	m_style;
		Mode		m_mode;

		Vector3d	m_position_start;
		Vector3d	m_position;
		Vector3d	m_position_snap;

		Vector3d	m_rotation_start;
		Vector3d	m_rotation;
		Vector3d	m_rotation_snap;

		Vector3d	m_scaling_start;
		Vector3d	m_scaling;
		Vector3d	m_scaling_snap;

		bool		m_dragging;
		Vector3d	m_drag_start;
		Vector3d	m_drag_end;

		Axis		m_drag_axis;
		Axis		m_drag_axis_hover;

		UIDragHandleRenderer*	m_renderer;
	private:
		friend	UIDragHandleRenderer;
	};
}

#endif//_EDITOR_UI_DRAGHANDLE_H_
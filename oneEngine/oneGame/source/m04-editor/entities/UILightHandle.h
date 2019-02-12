
#ifndef _EDITOR_UI_LIGHTHANDLE_H_
#define _EDITOR_UI_LIGHTHANDLE_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"

namespace M04
{
	class UILightHandleRenderer;
	class UILightHandle : public CGameBehavior
	{
		ClassName("UILightHandle");
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
			Range,
			Power,
			Color
		};
		enum class Axis : uint8_t
		{
			None,
			All,
			Range,
			Power,
			X,
			Y,
			Z
		};

	public:
		explicit	UILightHandle ( DrawStyle style=DrawStyle::s2D );
		~UILightHandle( void );

		void		Update ( void ) override;

		//		SetDrawStyle
		// Sets if the drag handle should be a 2D or 3D tool
		void		SetDrawStyle ( const DrawStyle n_drawstyle );
		//		SetMode
		// Sets the transformation mode of the handle
		void		SetMode ( const Mode n_usemode );

		//		SetRenderPosition
		// Sets the position that being rendered at
		void		SetRenderPosition ( const Vector3f& n_newPosition );

		//		HasFocus
		// Is this UI element in focus or currently working?
		bool		HasFocus ( void );

		//		SetRange
		// Sets the range of the light
		void		SetRange ( const float n_newRange );
		//		GetRange
		// Gets the range of the light
		float		GetRange ( void );

		//		SetPower
		// Sets the power of the light
		void		SetPower ( const float n_newPower );
		//		GetPower
		// Gets the power of the light
		float		GetPower ( void );

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

		Vector3f	m_position;

		float		m_range_start;
		float		m_range;
		float		m_range_snap;

		float		m_power_start;
		float		m_power;
		float		m_power_snap;

		bool		m_dragging;
		Vector3f	m_drag_start;
		Vector3f	m_drag_end;

		Axis		m_drag_axis;
		Axis		m_drag_axis_hover;

		UILightHandleRenderer*	m_renderer;
	private:
		friend	UILightHandleRenderer;
	};
}

#endif//_EDITOR_UI_DRAGHANDLE_H_
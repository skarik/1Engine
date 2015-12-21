#ifndef _M04_EDITOR_MAP_EDITOR_
#define _M04_EDITOR_MAP_EDITOR_

#include "engine/behavior/CGameBehavior.h"

class CCamera;
class CDuskGUI;

namespace M04
{
	class TileSelector;

	class MapEditor : public CGameBehavior
	{
	public:
		explicit	MapEditor ( void );
					~MapEditor ( void );

		void		Update ( void ) override;

	protected:
		//		doViewNavigationDrag () : view navigation
		// move the map around when middle button pressed
		void		doViewNavigationDrag ( void );

	public:
		enum class Mode : uint32_t
		{
			None = 0,
			TileEdit,
		};

	protected:
		Mode		m_current_mode;
		CCamera*	m_target_camera;

		CDuskGUI*	dusk;
		TileSelector*	m_tile_selector;
	};
}

#endif//_M04_EDITOR_MAP_EDITOR_

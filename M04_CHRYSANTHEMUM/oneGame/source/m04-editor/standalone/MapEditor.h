#ifndef _M04_EDITOR_MAP_EDITOR_
#define _M04_EDITOR_MAP_EDITOR_

#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUIHandle.h"

class CCamera;
class CDuskGUI;
namespace Engine2D
{
	class TileMap;
	class Tileset;
	class Area2DBase;
}
namespace M04
{
	class MapInformation;
}

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
		//		uiCreate () : create the dusk UI
		// create entirety of the dusk gui shit
		void		uiCreate ( void );

		//		uiStepTopEdge () : top edge update
		// handle inputs to the buttons on the top edge
		void		uiStepTopEdge ( void );
		//		uiStepDialogues () : dialogue polling
		// handles dialogue inputs and performs actions based on inputs to them
		void		uiStepDialogues ( void );
		//		uiStepKeyboardShortcuts () : do shortcut shit
		// handles keyboard inputs (shortcuts) to do things fast
		void		uiStepKeyboardShortcuts ( void );
		//		uiStepShitPanel () : S.H.I.T. panel update
		// handles input and updates to the shit panel
		void		uiStepShitPanel ( void );
		//		uiDoShitRefresh () : S.H.I.T. panel reinit
		// resets all input in the shit panel with the current values
		void		uiDoShitRefresh ( void );

		//		doViewNavigationDrag () : view navigation
		// move the map around when middle button pressed
		void		doViewNavigationDrag ( void );

		//		doTileEditing () : tile editing
		// edit the tile when clicking happens
		void		doTileEditing ( void );
		void		_doTileEditingSub ( float mousex, float mousey );

		//		doAreaEditing () : area editing
		// do area editing and such when clicking happens
		void		doAreaEditing ( void );

		//		doMapResize () : resize the map
		// using the size given in m_mapinfo structure, change the map size.
		// fills layer zero with a bunch of default tiles and destroys any tiles out of range
		void		doMapResize ( void );

		//		doIOSaving () : saving
		// save the map to the file in m_current_savetarget
		void		doIOSaving ( void );

	public:
		enum class Mode : uint32_t
		{
			None = 0,
			Properties,
			TileEdit,
			AreaEdit,
			ActorsEdit,
		};
		enum class SubMode : uint32_t
		{
			None = 0,
			Dragging,
		};

	protected:
		Mode		m_current_mode;
		SubMode		m_current_submode;

		CCamera*	m_target_camera;
		bool		m_navigation_busy;

		CDuskGUI*	dusk;
		TileSelector*	m_tile_selector;

		int			m_area_corner_selection;
		Engine2D::Area2DBase*	m_area_target;

		M04::MapInformation*	m_mapinfo;
		Engine2D::TileMap*		m_tilemap;

		string		m_current_savetarget;

		//=========================================//
		// Dusk handles

		Dusk::Handle	ui_file_new;
		Dusk::Handle	ui_file_save;
		Dusk::Handle	ui_file_load;

		Dusk::Handle	ui_mode_shit;
		Dusk::Handle	ui_mode_map;
		Dusk::Handle	ui_mode_area;
		Dusk::Handle	ui_mode_actors;

		Dusk::Handle	ui_dg_save;
		Dusk::Handle	ui_dg_load;

		Dusk::Handle	ui_panel_shit;
		Dusk::Handle	ui_btn_cancel_shit;
		Dusk::Handle	ui_btn_apply_shit;
		Dusk::Handle	ui_fld_map_name;
		Dusk::Handle	ui_fld_map_area;
		Dusk::Handle	ui_fld_map_size_x;
		Dusk::Handle	ui_fld_map_size_y;
		Dusk::Handle	ui_lbl_map_area;
		Dusk::Handle	ui_lbl_map_size;
	};
}

#endif//_M04_EDITOR_MAP_EDITOR_
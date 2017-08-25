#ifndef _M04_EDITOR_MAP_EDITOR_
#define _M04_EDITOR_MAP_EDITOR_

#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUIHandle.h"

//=========================================//
// Prototypes
//=========================================//

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
	class AreaRenderer;
	class TileSelector;
	class ObjectEditorListing;
	class UIDragHandle;
	class UILightHandle;
	class GizmoRenderer;
	class EditorObject;
}

//=========================================//
// Class definition
//=========================================//

namespace M04
{
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
		//		uiStepTilePanel () : tile editor panel update
		// handles inputs and updates to the tile panel
		void		uiStepTilePanel ( void );
		// Called when tile-editing mode ends
		void		_uiStepTilePanel_End ( void );
		//		uiStepAreaPanel () : area panel update
		// handles input and updates to the area panel
		void		uiStepAreaPanel ( void );
		//		uiStepObjectPanel () : object panel update
		// handles input and updates to the object panel
		void		uiStepObjectPanel ( void );
		void		_uiStepObjectPanelSub_ClearProperties ( void );
		//		uiStepObjectPanel () : preferencess panel update
		// handles input and updates to the preferencess panel
		void		uiStepPreferencesPanel ( void );
		//		uiStepBottomEdge () : status panel update
		// updates display of the current editor state
		void		uiStepBottomEdge ( void );

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

		//		doObjectEditing () : actor editing
		// do actor selection, moving, deleting, and such
		void		doObjectEditing ( void );
		void		_doObjectEditingSub_GizmoEnable ( void );
		void		_doObjectEditingSub_GizmoDisable ( void );

		//		doMapResize () : resize the map
		// using the size given in m_mapinfo structure, change the map size.
		// fills layer zero with a bunch of default tiles and destroys any tiles out of range
		void		doMapResize ( void );

		//		doIOSaving () : saving
		// save the map to the file in m_current_savetarget
		void		doIOSaving ( void );
		//		diIOLoading () : loading
		// load the map from the file in m_current_savetarget
		// uses doNewMap() to clear out the data first
		void		doIOLoading ( void );
		//		doNewMap () : delete all items in map, clear out tilemap
		// clears out all information for the map
		void		doNewMap ( void );

	public:
		enum class Mode : uint32_t
		{
			None = 0,
			Properties,
			TileEdit,
			AreaEdit,
			ObjectEdit,
			ScriptEdit,
			UtilityEdit,
			Toolbox,
			Preferences,
		};
		enum class SubMode : uint32_t
		{
			None = 0,
			Dragging,
			Dropdown,

			TilesVisual,
			TilesCollision,
			TilesHeight
		};

	protected:
		Mode		m_current_mode;
		SubMode		m_current_submode;

		CCamera*	m_target_camera;
		Vector3d	m_target_camera_position;
		bool		m_navigation_busy;
		bool		m_preclude_navigation;	// Stop the navigation

		CDuskGUI*	dusk;
		TileSelector*	m_tile_selector;
		AreaRenderer*	m_area_renderer;
		GizmoRenderer*	m_gizmo_renderer;

		int			m_tile_layer_current;

		int			m_area_corner_selection;
		Engine2D::Area2DBase*	m_area_target;

		// M04::EditorObjectProxy*	m_actor_selection;
		// Engine2D::ObjectProxy*	m_actor_info_selection;
		// so it'll be a Engine2D::SpriteContainer which is an interface with only a sprite.
		// constructor of it will be explicit, taking a Vector3d pointer, which will be draw pos
		//										  and a float pointer, which will be angle
		//										  and a Vector3d pointer, for scale
		M04::EditorObject*	m_object_target;

		ObjectEditorListing*	m_listing;
		UIDragHandle*			m_drag_handle;
		UILightHandle*			m_light_handle;

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
		Dusk::Handle	ui_mode_object;
		Dusk::Handle	ui_mode_script;
		Dusk::Handle	ui_mode_utils;
		Dusk::Handle	ui_toolbox_cutscene;
		Dusk::Handle	ui_toolbox_global;
		Dusk::Handle	ui_mode_preferences;

		Dusk::Handle	ui_lbl_mode;
		Dusk::Handle	ui_lbl_mousex;
		Dusk::Handle	ui_lbl_mousey;

		Dusk::Handle	ui_dg_save;
		Dusk::Handle	ui_dg_load;

		Dusk::Handle	ui_panel_tiles;
		Dusk::Handle	ui_lst_tile_layer;
		Dusk::Handle	ui_btn_clear_layer;
		Dusk::Handle	ui_btn_inc_layer;
		Dusk::Handle	ui_btn_dec_layer;
		Dusk::Handle	ui_fld_current_layer;
		Dusk::Handle	ui_btn_tile_mode_visual;
		Dusk::Handle	ui_btn_tile_mode_collision;
		Dusk::Handle	ui_btn_tile_mode_height;

		Dusk::Handle	ui_panel_shit;
		Dusk::Handle	ui_btn_cancel_shit;
		Dusk::Handle	ui_btn_apply_shit;
		Dusk::Handle	ui_fld_map_name;
		Dusk::Handle	ui_fld_map_area;
		Dusk::Handle	ui_fld_map_size_x;
		Dusk::Handle	ui_fld_map_size_y;
		Dusk::Handle	ui_lbl_map_area;
		Dusk::Handle	ui_lbl_map_size;
		Dusk::Handle	ui_fld_map_ambient_color;

		Dusk::Handle	ui_panel_area;
		Dusk::Handle	ui_fld_area_type;

		Dusk::Handle	ui_panel_object;
		Dusk::Handle	ui_fld_object_type;
		Dusk::Handle	ui_lbl_object_properties;
		std::vector<Dusk::Handle>	ui_lbl_object_keys;
		std::vector<Dusk::Handle>	ui_lbl_object_values;

		Dusk::Handle	ui_panel_preferences;
		Dusk::Handle	ui_fld_pref_mouse_sensitivity;
	};
}

#endif//_M04_EDITOR_MAP_EDITOR_

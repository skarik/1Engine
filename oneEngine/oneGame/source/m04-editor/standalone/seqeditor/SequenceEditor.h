#ifndef M04_EDITORS_SEQUENCE_EDITOR_MAIN_
#define M04_EDITORS_SEQUENCE_EDITOR_MAIN_

#include <vector>
#include <map>

#include "core-ext/containers/osfstructure.h"

#include "engine/behavior/CGameBehavior.h"
#include "m04/eventide/UserInterface.h"
#include "engine-common/dusk/Dusk.h"

#include "./TopMenu.h"
#include "./MouseGizmo.h"
#include "./GridGizmo.h"
#include "./NodeBoardState.h"
#include "./RightClickListMenu.h"

class arStringEnumDefinition;

namespace m04 {
namespace editor {

	struct SequenceEditorAction
	{
	};

	class SequenceEditor : public CGameBehavior
	{
	public:
		EDITOR_API explicit		SequenceEditor ( void );
		EDITOR_API				~SequenceEditor ( void );

		EDITOR_API void			Update ( void );

		EDITOR_API ui::eventide::UserInterface*
								GetEventideUI ( void )
			{ return user_interface; }

		EDITOR_API dusk::UserInterface*
								GetDuskUI ( void )
			{ return dusk_interface; }

		EDITOR_API m04::editor::sequence::NodeBoardState*
								GetNodeBoardState ( void )
			{ return board_state; }

		//	GetEnums() : Returns the enum definitions loaded for the current SEL.
		EDITOR_API const std::map<arstring128, arStringEnumDefinition*>&
								GetEnums ( void )
			{ return enum_definitions; }

		//	GetGridState() : Returns the grid state for the editor.
		EDITOR_API m04::editor::sequence::GridState&
								GetGridState ( void )
			{ return grid_state; }

		//	GetWorkspaceDirty() : Returns if workspace is dirty.
		EDITOR_API bool			GetWorkspaceDirty ( void )
			{ return workspace_dirty; }
		//	SetWorkspaceDirty() : Returns if workspace is dirty.
		EDITOR_API void			SetWorkspaceDirty ( bool dirty = true )
			{ workspace_dirty = dirty; }

	protected:
		ui::eventide::UserInterface*
							user_interface = NULL;

		dusk::UserInterface*
							dusk_interface = NULL;

		//	LoadSequenceEditorListing(filepath) : Loads the given editor info.
		void					LoadSequenceEditorListing( const char* sel_path );
		// List of all available externally-loaded enums
		std::map<arstring128, arStringEnumDefinition*>
							enum_definitions;

		// Top menu with all the dropdowns and associated logic.
		m04::editor::sequence::TopMenu*
							top_menu = NULL;
		// Mouse gizmo used to show current mouse position & view motion.
		m04::editor::sequence::MouseGizmo*
							mouse_gizmo = NULL;

		// Is the camera being currently dragged?
		bool				dragging_view = false;
		// XYZ position that we want to keep trained under the mouse
		Vector3f			dragging_reference_position;
		// Is the camera being currently zoom-dragged?
		bool				zooming_view = false;
		//	UpdateCameraControl() : Does camera panning & zooming
		void					UpdateCameraControl ( void );


		// Grid gizmo used to draw the grid
		m04::editor::sequence::GridGizmo*
							grid_gizmo = NULL;
		// Grid state of the editor
		m04::editor::sequence::GridState
							grid_state;

		
		// Right click menu.
		m04::editor::sequence::RightClickListMenu*
							right_click_menu = NULL;

		void					UpdateRightClickMenu ( void );

		// Current state of board
		m04::editor::sequence::NodeBoardState*
							board_state = NULL;
		// Has the workspace been dirtied (worked on?)
		bool				workspace_dirty = false;
		// List of actions to do/undo
		std::vector<SequenceEditorAction>
							actions;
	};
}};

#endif///M04_EDITORS_SEQUENCE_EDITOR_MAIN_
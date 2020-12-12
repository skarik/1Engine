#ifndef M04_EDITORS_SEQUENCE_EDITOR_MAIN_
#define M04_EDITORS_SEQUENCE_EDITOR_MAIN_

#include <vector>

#include "osfstructure.h"

#include "engine/behavior/CGameBehavior.h"
#include "m04/eventide/UserInterface.h"
#include "engine-common/dusk/Dusk.h"

#include "./TopMenu.h"
#include "./MouseGizmo.h"
#include "./NodeBoardState.h"
#include "./RightClickListMenu.h"

namespace m04 {
namespace editor {
	
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

	protected:
		ui::eventide::UserInterface*
							user_interface = NULL;

		dusk::UserInterface*
							dusk_interface = NULL;

		ui::eventide::Element*
							test_element = NULL;

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
		void				UpdateCameraControl ( void );

		
		// Right click menu.
		m04::editor::sequence::RightClickListMenu*
							right_click_menu = NULL;

		void				UpdateRightClickMenu ( void );


		m04::editor::sequence::NodeBoardState*
							board_state = NULL;
	};
}};

#endif///M04_EDITORS_SEQUENCE_EDITOR_MAIN_
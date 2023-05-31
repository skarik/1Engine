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
class RrWindow;
class RrWorld;
class RrCamera;

namespace m04 {
namespace editor {

	struct SequenceEditorAction
	{
	};

	class SequenceNodeDefinition
	{
		arstring64			displayName;
		arstring64			category;
		// Number of outputs to the node. Negative means the number of outputs is automatically determined.
		int					outputCount = 1;
	};

	struct SequenceNodePropertyDefinition
	{
		arstring64			displayName;
		PropertyRenderStyle	type;
		arstring64			enumName;
		arstring64			defaultValue;
		std::vector<SequenceNodePropertyDefinition>
							arraySubproperties;
	};

	enum class SequenceOutputPreference
	{
		kOsf,
		kJson,
	};

	enum class SequenceGUIDType
	{
		kGUID32,
		kUUID4,
	};

	enum class SequenceJumpStyle
	{
		kJump,
		kLink,
	};

	class SequenceEnumDefinition
	{
	public:
		SequenceEnumDefinition ( arStringEnumDefinition* def, std::vector<arstring64>&& names )
			: enumDefinition(def)
			, displayNames(names)
		{}
		~SequenceEnumDefinition ( void );

		arStringEnumDefinition*	enumDefinition = nullptr;
		std::vector<arstring64> displayNames;
	};

	class SELInfo
	{
	public:
		//	LoadSequenceEditorListing(filepath) : Loads the given editor info.
		void					LoadSequenceEditorListing ( const char* sel_path );

		void					Free ( void );

	public:
		// Settings about data generated
		arstring64			next_node_category = "";
		SequenceOutputPreference
							output_preference = SequenceOutputPreference::kOsf;
		SequenceGUIDType	guid_preference = SequenceGUIDType::kGUID32;
		SequenceJumpStyle	jump_style = SequenceJumpStyle::kJump;

		// List of all available externally-loaded enums
		std::map<arstring128, SequenceEnumDefinition*>
							enum_definitions;
		// List of all available externally-loaded nodes
		std::map<arstring128, SequenceNodeDefinition*>
							node_definitions;
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

		EDITOR_API const ArScreen&
								GetScreen ( void );

		EDITOR_API RrCamera*	GetCamera ( void )
			{ return editor_camera; }

		EDITOR_API m04::editor::sequence::NodeBoardState*
								GetNodeBoardState ( void )
			{ return board_state; }

		//	GetEnums() : Returns the enum definitions loaded for the current SEL.
		EDITOR_API const std::map<arstring128, SequenceEnumDefinition*>&
								GetEnums ( void )
			{ return sel.enum_definitions; }

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

		//	GetSaveTargetFilename() : Returns current save target filename
		EDITOR_API const std::string&
								GetSaveTargetFilename ( void )
			{ return save_target_filename; }
		//	SetSaveTargetFilename(filename) : Sets the current save target filename
		EDITOR_API void			SetSaveTargetFilename ( const char* filename );

		//	GetMousePosition3D() : Returns current calculated position of the 3D mouse cursor.
		EDITOR_API Vector3f		GetMousePosition3D ( void );

	protected:
		RrWindow*			window = NULL;
		RrWorld*			editor_world = NULL;
		RrCamera*			editor_camera = NULL;

		ui::eventide::UserInterface*
							user_interface = NULL;

		dusk::UserInterface*
							dusk_interface = NULL;

		// Current information about the available nodes loaded in.
		SELInfo				sel;

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
		// Filename of the current save target
		std::string			save_target_filename = "";
	};
}};

#endif///M04_EDITORS_SEQUENCE_EDITOR_MAIN_
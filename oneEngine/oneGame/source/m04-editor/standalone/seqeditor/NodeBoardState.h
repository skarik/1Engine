#ifndef M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_
#define M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_

#include "SequenceNode.h"

#include "core/math/Vector3.h"
#include "core-ext/containers/arguid.h"

#include "m04/eventide/UserInterface.h"

namespace m04 {
namespace editor {
	class SequenceEditor;
}}

namespace m04 {
namespace editor {
namespace sequence {

	class INodeDisplay;
	class ISequenceSerializer;

	struct BoardNode
	{
	public:
		// Actual data contained in the node
		SequenceNode*		sequenceInfo;

		// Position the node is at
		Vector3f			position;
		// GUID of the node
		arguid32			guid;

		// Associated Eventide element used to render the object
		INodeDisplay*		display;

	public:
		EDITOR_API void 		SetPosition ( const Vector3f& new_position )
			{ position = new_position; }

	};

	class INodeDisplay
	{
	public:
		INodeDisplay(BoardNode* in_node)
			: node(in_node)
			{}

		BoardNode*			GetBoardNode ( void )
			{ return node; }

	protected:
		BoardNode*			node;
	};

	// Board state contains all the nodes for the current sequence, as well as the visuals and ids
	class NodeBoardState
	{
	public:
		explicit				NodeBoardState ( m04::editor::SequenceEditor* editor );

		//	AddDisplayNode( board_node ) : Adds node to display. Allocates and sets up a proper display object.
		void					AddDisplayNode ( BoardNode* board_node );

		//	RemoveNode( board_node ) : Removes node from the board.
		void					RemoveDisplayNode ( BoardNode* board_node );

		// TODO: Save & load from a buffer?
		// Saving:
		//		pass 1: determine which nodes need a label
		//				- nodes after nodes with 2 or more flow outputs
		//				- nodes with 2 or more flow inputs
		//		pass 2: loop through flow:
		//				- current path added to front
		//				- next path added to back
		//				- pop next node to process from the front
		// generate vector<vector<BoardNode>>, and each time there is a duplicate across lists - split the list at that node, do a goto instead?
		// struct EntryLink { BoardNode, Link { vector<BoardNode>, index } }
		void					Save ( ISequenceSerializer* serializer );

	public:
		std::vector<BoardNode*>
							nodes;
		std::vector<INodeDisplay*>
							display;
		std::vector<arguid32*>
							node_guids;

	protected:
		ui::eventide::UserInterface*
							ui;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_
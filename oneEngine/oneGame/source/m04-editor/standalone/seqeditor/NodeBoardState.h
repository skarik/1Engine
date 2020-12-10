#ifndef M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_
#define M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_

#include "SequenceNode.h"

#include "core/math/Vector3.h"
#include "core-ext/containers/arguid.h"

#include "m04/eventide/UserInterface.h"

namespace m04 {
namespace editor {
namespace sequence {

	class INodeDisplay;

	struct BoardNode
	{
	public:
		// Actual data contained in the node
		SequenceNode		sequenceInfo;

		// Position the node is at
		Vector3f			position;
		// GUID of the node
		arguid32			guid;

		// Associated Eventide element used to render the object
		INodeDisplay*		display;
	};

	class INodeDisplay
	{
		BoardNode*			node;
	};

	class NodeBoardState
	{
	public:
		explicit				NodeBoardState (ui::eventide::UserInterface* in_ui)
			: ui(in_ui)
			{}

	public:
		std::vector<BoardNode*>
							nodes;
		std::vector<INodeDisplay*>
							display;

	protected:
		ui::eventide::UserInterface*
							ui;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_
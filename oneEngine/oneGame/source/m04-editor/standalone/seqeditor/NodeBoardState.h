#ifndef M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_
#define M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_

#include "SequenceNode.h"

#include "core/math/Vector3.h"
#include "core-ext/containers/arguid.h"
#include "core-ext/containers/aruuid4.h"

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
	class ISequenceDeserializer;

	// TODO: Unify with SequenceGUIDType??? or a converter
	enum class BoardNodeGUIDType
	{
		kGUID32,
		kUUID4,
	};

	// TODO: move the combined GUID class type elsewhere
	struct BoardNodeGUID
	{
		BoardNodeGUIDType	guidType = BoardNodeGUIDType::kGUID32;
		union
		{
			arguid32	guid32;
			aruuid4		uuid4;
			uint64_t	raw[4];
		};

		std::string toString ( void )
		{
			if (guidType == BoardNodeGUIDType::kGUID32)
				return guid32.toString();
			else if (guidType == BoardNodeGUIDType::kUUID4)
				return uuid4.toString();
			ARCORE_ERROR("Invalid GUID type");
			return "";
		}

		void setFromString ( const std::string& stringValue )
		{
			if (guidType == BoardNodeGUIDType::kGUID32)
				guid32.setFromString(stringValue);
			else if (guidType == BoardNodeGUIDType::kUUID4)
				uuid4.setFromString(stringValue);
		}

		void generate ( void )
		{
			if (guidType == BoardNodeGUIDType::kGUID32)
				guid32.generate();
			else if (guidType == BoardNodeGUIDType::kUUID4)
				uuid4.generate();
		}

		//	generateDistinctTo(container) : Generates a new GUID value that is unique within the given container.
		template <class ContainerType>
		void generateDistinctTo ( const ContainerType& container )
		{
			do
			{
				generate();
			}
			while (!isDistinctTo(container));
		}

		//	isDistinctTo(container) : Checks if the current GUID is unique to the given container.
		template <class ContainerType>
		bool					isDistinctTo ( const ContainerType& container ) const
		{
			for (const BoardNodeGUID* guid : container)
			{
				if (*guid == *this)
				{
					return false;
				}
			}
			return true;
		}

		bool					operator== ( const BoardNodeGUID& other ) const
		{
			if (guidType == other.guidType)
			{
				if (guidType == BoardNodeGUIDType::kGUID32)
					return guid32 == other.guid32;
				else if (guidType == BoardNodeGUIDType::kUUID4)
					return uuid4 == other.uuid4;
				ARCORE_ERROR("Invalid GUID type");
			}
			return false;
		}

		BoardNodeGUID()
			: guidType(BoardNodeGUIDType::kGUID32)
			, raw{0}
			{}
	};

	struct BoardEditorData
	{
		// Position in the board
		Vector3f			position;
		// GUID of the node
		BoardNodeGUID		guid = {};
		// Key-value storage, populated by the editor.
		osf::ObjectValue	data;
	};

	struct BoardNode
	{
	public:
		// Actual data contained in the node
		SequenceNode*		sequenceInfo;

		// Data used by the editor and for saving
		BoardEditorData		editorData;

		// Associated Eventide element used to render the object.
		// Not owned by this object.
		INodeDisplay*		display;

	public:
		EDITOR_API void 		SetPosition ( const Vector3f& new_position )
			{ editorData.position = new_position; }

		//	PushEditorData() : Pushes the editor data to the sequence node's keyvalues.
		EDITOR_API void			PushEditorData ( void );

		//	FreeData() : Frees associated SequenceNode and other information.
		EDITOR_API void			FreeData ( void );

	public:
		//	DebugDumpOSF() : Calls PushEditorData, then dumps out current keyvalues into stdout.
		EDITOR_API void			DebugDumpOSF ( void );
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

		//	UnhookNode( board_node ) : Removes links to this node.
		// Searches through all the flow, sync, and output of everything in the board.
		void					UnhookNode ( BoardNode* board_node );

		//	RemoveNode( board_node ) : Removes node from the board.
		void					RemoveDisplayNode ( BoardNode* board_node );

		[[nodiscard]]
		// todo: save the board to the given file
		bool SaveToFile ( const char* filename );

		[[nodiscard]]
		// todo: load the board from the given file
		bool LoadFromFile ( const char* filename );

		//[[deprecated]]
		//	Save( serializer ) : Saves board state with the given serializer.
		void					Save ( ISequenceSerializer* serializer );

		//[[deprecated]]
		//	Load( deserializer ) : Clears board state, then loads board state with given serializer.
		// Any invalid nodes are still loaded, but with a severely-limited view. Since the views are mostly untouched, their data is preserved.
		void					Load ( ISequenceDeserializer* deserializer );

		//	ClearAllNodes() : Clears all nodes from the board state.
		void					ClearAllNodes ( void );

	public:
		m04::editor::SequenceEditor*
								GetEditor ( void )
			{ return parent_editor; }

	public:
		struct NodeEntry
		{
			BoardNode*		node;
			INodeDisplay*	display;
			BoardNodeGUID*	guid;
		};

		std::vector<NodeEntry>
							nodes;

	protected:
		ui::eventide::UserInterface*
							ui;

		m04::editor::SequenceEditor*
							parent_editor;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_STATE_H_
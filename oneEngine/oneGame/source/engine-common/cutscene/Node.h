#ifndef ENGINE_CUTSCENE_NODE_H_
#define ENGINE_CUTSCENE_NODE_H_

#include "core/types.h"
#include "core/containers/arstring.h"

namespace common {
namespace cts
{
	//	ENodeType : Types of nodes.
	enum ENodeType
	{
		kNodeType_INVALID = 0,

		// It talks
		kNodeTypeTalkbox,
		// It choices
		kNodeTypeChoicebox,
		// Waits for either an external signal or a timer.
		kNodeTypeWait,
		// Create a mover for a character
		kNodeTypeMoveCharacterM04,
		// Prolly isnt needed
		kNodeTypeEnd,

		kNodeType_MAX
	};

	struct nodeTypeInfo
	{
		char strname [64];
		char readable [64];
	};
	//	kNodeTypeInfo : Machine readable and human readable names for nodes. Used in editor settings.
	static const nodeTypeInfo kNodeTypeInfo [kNodeType_MAX] =
	{
		{"INVALID",				"Invalid"},
		{"talkbox",				"Talkbox"},
		{"choicebox",			"Choicebox"},
		{"wait",				"Wait"},
		{"movecharacterm04",	"Move Character (M04)"},
		{"end",					"End"},
	};

	//	ENodeKeyType : Types of keys that are stored for nodes
	enum ENodeKeyType
	{
		kNodeKeyType_INVALID = 0,
		kNodeKeyContentTextCount,
		kNodeKeyContentText0,
		kNodeKeyContentText1,
		kNodeKeyContentText2,
		kNodeKeyContentText3,
		kNodeKeyCharacterSource,
		kNodeKeyCharacterTarget,
		kNodeKeyMotionTarget,
		kNodeKeyMotionSpeed,
		kNodeKeyMotionTime,
		kNodeKeySignalType,
		kNodeKeySignalName,
		kNodeKeySignalTime,
		kNodeKeyType_MAX
	};
	//	nodeKeyValue : Structure for storing node IO data
	struct nodeKeyValue
	{
		ENodeKeyType	key;
		arstring256		value;
	};

	
	//	Node - basis for actions in the cutscene system
	class Node
	{
	protected:
		explicit					Node ( void ) {}

	public:
		virtual						~Node ( void ) {}

		ENGCOM_API virtual ENodeType	GetNodeType ( void ) =0;
		ENGCOM_API virtual int		GetOutputNodeCount ( void ) =0;
		ENGCOM_API virtual Node*	GetOutputNode ( const int index ) =0;

	public:
		//	Node Input & Output:

		//	IOSetKey( key, value ) : Sets internal data
		// Since the node type may not be defined in this module, the loader cannot set values directly, making this function necessary.
		// Used by the loader in order to deserialize from OSF format.
		ENGCOM_API virtual void		IOSetKey ( const ENodeKeyType key, const char* value );
		//	IOGetKeys( array ) : Get an array of keys that represent this node
		// Since the node type may not be defined in this module, the loader cannot read values directly, making this function necessary.
		// NULL may be passed in to retrieve the size of the needed data.
		ENGCOM_API virtual int		IOGetKeys ( nodeKeyValue* keyValueArray );
		//	IOSetOutputNodeCount( count ) : Sets number of output nodes.
		// This call may possibly be ignored by the implementation.
		ENGCOM_API virtual void		IOSetOutputNodeCount ( const int count ) =0;
		//	IOSetOutputNode( index, node ) : Sets the output node in the output node list at index.
		// If there is only one output node available, the first argument is ignored.
		// Output node is also known as "the node that comes after this."
		// This call will be ignored by certain node types, such as NodeEnd.
		ENGCOM_API virtual void		IOSetOutputNode ( const int index, Node* node ) =0;

	};

}}

#endif//ENGINE_CUTSCENE_NODE_H_
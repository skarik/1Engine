#ifndef ENGINE_CUTSCENE_NODE_TALKBOX_H_
#define ENGINE_CUTSCENE_NODE_TALKBOX_H_

#include "engine-common/cutscene/Node.h"

namespace common {
namespace cts
{
	//	Node - basis for actions in the cutscene system
	class NodeTalkbox : public Node
	{
	public:
		explicit			NodeTalkbox ( void )
			: Node() {}
		virtual			~NodeTalkbox ( void )
			{}

		virtual ENodeType	GetNodeType ( void ) 
			{ return kNodeTypeTalkbox; }
		virtual int		GetOutputNodeCount ( void )
			{ return 1; }
		virtual Node*	GetOutputNode ( const int index ) 
			{ return NULL; }
	};

}}

#endif//ENGINE_CUTSCENE_NODE_TALKBOX_H_
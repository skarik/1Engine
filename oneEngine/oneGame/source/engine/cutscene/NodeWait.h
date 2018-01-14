#ifndef ENGINE_CUTSCENE_NODE_WAIT_H_
#define ENGINE_CUTSCENE_NODE_WAIT_H_

#include "engine/cutscene/Node.h"

namespace engine {
	namespace cts
	{
		//	Node - basis for actions in the cutscene system
		class NodeWait : public Node
		{
		public:
			explicit			NodeWait ( void )
				: Node() {}
			virtual			~NodeWait ( void )
				{}

			virtual eNodeType	GetNodeType ( void ) 
				{ return kNodeTypeWait; }
			virtual int		GetOutputNodeCount ( void )
				{ return 1; }
			virtual Node*	GetOutputNode ( const int index ) 
				{ return NULL; }
		};

	}}

#endif//ENGINE_CUTSCENE_NODE_WAIT_H_
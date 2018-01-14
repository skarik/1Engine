#ifndef ENGINE_CUTSCENE_NODE_END_H_
#define ENGINE_CUTSCENE_NODE_END_H_

#include "engine/cutscene/Node.h"

namespace engine {
	namespace cts
	{
		//	Node - basis for actions in the cutscene system
		class NodeEnd : public Node
		{
		public:
			explicit			NodeEnd ( void )
				: Node() {}
			virtual			~NodeEnd ( void )
				{}

			virtual eNodeType	GetNodeType ( void ) 
				{ return kNodeTypeEnd; }
			virtual int		GetOutputNodeCount ( void )
				{ return 0; }
			virtual Node*	GetOutputNode ( const int index ) 
				{ return NULL; }
		};

	}}

#endif//ENGINE_CUTSCENE_NODE_END_H_
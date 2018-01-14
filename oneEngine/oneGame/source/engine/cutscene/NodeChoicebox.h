#ifndef ENGINE_CUTSCENE_NODE_CHOICEBOX_H_
#define ENGINE_CUTSCENE_NODE_CHOICEBOX_H_

#include "engine/cutscene/Node.h"

namespace engine {
	namespace cts
	{
		//	Node - basis for actions in the cutscene system
		class NodeChoicebox : public Node
		{
		public:
			explicit			NodeChoicebox ( void )
				: Node() {}
			virtual			~NodeChoicebox ( void )
				{}

			virtual eNodeType	GetNodeType ( void ) 
				{ return kNodeTypeChoicebox; }
			virtual int		GetOutputNodeCount ( void )
				{ return 4; }
			virtual Node*	GetOutputNode ( const int index ) 
				{ return NULL; }
		};

	}}

#endif//ENGINE_CUTSCENE_NODE_CHOICEBOX_H_
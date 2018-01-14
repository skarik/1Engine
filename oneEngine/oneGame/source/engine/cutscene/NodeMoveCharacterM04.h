#ifndef ENGINE_CUTSCENE_NODE_MOVE_CHARACTER_M04_H_
#define ENGINE_CUTSCENE_NODE_MOVE_CHARACTER_M04_H_

#include "engine/cutscene/Node.h"

namespace engine {
	namespace cts
	{
		//	Node - basis for actions in the cutscene system
		class NodeMoveCharacterM04 : public Node
		{
		public:
			explicit			NodeMoveCharacterM04 ( void )
				: Node() {}
			virtual			~NodeMoveCharacterM04 ( void )
				{}

			virtual eNodeType	GetNodeType ( void ) 
				{ return kNodeTypeMoveCharacterM04; }
			virtual int		GetOutputNodeCount ( void )
				{ return 1; }
			virtual Node*	GetOutputNode ( const int index ) 
				{ return NULL; }
		};

	}}

#endif//ENGINE_CUTSCENE_NODE_MOVE_CHARACTER_M04_H_
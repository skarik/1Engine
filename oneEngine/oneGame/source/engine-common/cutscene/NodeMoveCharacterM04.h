#ifndef ENGINE_CUTSCENE_NODE_MOVE_CHARACTER_M04_H_
#define ENGINE_CUTSCENE_NODE_MOVE_CHARACTER_M04_H_

#include "engine-common/cutscene/Node.h"

namespace common {
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

		virtual ENodeType	GetNodeType ( void ) 
			{ return kNodeTypeMoveCharacterM04; }
		int				GetOutputNodeCount ( void ) override
			{ return 1; }
		Node*			GetOutputNode ( const int index ) override
			{ return m_outputNode; }

	public:
		//	IOSetOutputNode( IGNORED, node ) : Sets the output node in the output node list at index.
		void			IOSetOutputNode ( const int index, Node* node ) override
			{ m_outputNode = node; }

	protected:
		Node*		m_outputNode;
	};

}}

#endif//ENGINE_CUTSCENE_NODE_MOVE_CHARACTER_M04_H_
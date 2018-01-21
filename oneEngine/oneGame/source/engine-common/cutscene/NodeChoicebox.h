#ifndef ENGINE_CUTSCENE_NODE_CHOICEBOX_H_
#define ENGINE_CUTSCENE_NODE_CHOICEBOX_H_

#include "engine-common/cutscene/Node.h"

namespace common {
namespace cts
{
	//	Node - basis for actions in the cutscene system
	class NodeChoicebox : public Node
	{
	public:
		ENGCOM_API explicit	NodeChoicebox ( void );
		virtual				~NodeChoicebox ( void )
			{}

		ENodeType			GetNodeType ( void ) override
			{ return kNodeTypeChoicebox; }
		int					GetOutputNodeCount ( void ) override
			{ return 4; }
		Node*				GetOutputNode ( const int index ) override
			{ return m_outputNodes[index]; }

	public:
		//	IOSetOutputNodeCount( count ) : Sets number of output nodes.
		// Count must be in the range of 0 to 3 (inclusive)
		ENGCOM_API void		IOSetOutputNodeCount ( const int count ) override;
		//	IOSetOutputNode( index, node ) : Sets the output node in the output node list at index.
		// Index must be in the range of 0 to 3 (inclusive)
		void				IOSetOutputNode ( const int index, Node* node ) override
			{ m_outputNodes[index] = node; }

	protected:
		int		m_outputNodeCount;
		Node*	m_outputNodes [4];
	};

}}

#endif//ENGINE_CUTSCENE_NODE_CHOICEBOX_H_
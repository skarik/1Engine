#ifndef ENGINE_CUTSCENE_NODE_END_H_
#define ENGINE_CUTSCENE_NODE_END_H_

#include "engine-common/cutscene/Node.h"

namespace common {
namespace cts
{
	//	NodeEnd - ending node action that signals the cutscene system should rpobably end.
	class NodeEnd : public Node
	{
	public:
		explicit			NodeEnd ( void )
			: Node() {}
		virtual			~NodeEnd ( void )
			{}

		virtual ENodeType	GetNodeType ( void ) 
			{ return kNodeTypeEnd; }
		virtual int		GetOutputNodeCount ( void )
			{ return 0; }
		virtual Node*	GetOutputNode ( const int index ) 
			{ return NULL; }

	public:
		//	IOSetOutputNode( IGNORED, IGNORED ) : Sets the output node in the output node list at index.
		// Ignored for Ending nodes
		void			IOSetOutputNode ( const int, Node* ) override
			{}

	};

}}

#endif//ENGINE_CUTSCENE_NODE_END_H_
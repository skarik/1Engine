#ifndef ENGINE_CUTSCENE_NODE_H_
#define ENGINE_CUTSCENE_NODE_H_

#include "core/types.h"

namespace engine {
namespace cts
{
	enum eNodeType
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
	
	//	Node - basis for actions in the cutscene system
	class Node
	{
	protected:
		explicit					Node ( void ) {}

	public:
		virtual						~Node ( void ) {}

		ENGINE_API virtual eNodeType	GetNodeType ( void ) =0;
		ENGINE_API virtual int		GetOutputNodeCount ( void ) =0;
		ENGINE_API virtual Node*	GetOutputNode ( const int index ) =0;

	};

}}

#endif//ENGINE_CUTSCENE_NODE_H_
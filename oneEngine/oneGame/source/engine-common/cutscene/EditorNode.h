#ifndef ENGCOM_CUTSCENE_EDITOR_NODE_H_
#define ENGCOM_CUTSCENE_EDITOR_NODE_H_

#include "core/math/Vector2d.h"
#include "engine-common/cutscene/Node.h"

namespace common
{
	namespace cts
	{
		//	EditorNode : class used for storing nodes w/ visual component.
		// Used in the cutscene editor.
		class EditorNode 
		{
		public:
			common::cts::Node*	node;
			Vector2d			position;
		};
	}
}

#endif//ENGCOM_CUTSCENE_EDITOR_NODE_H_
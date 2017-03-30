#ifndef _EDITOR_STANDALONE_CTSEDITOR_CTS_NODE_BASE_H_
#define _EDITOR_STANDALONE_CTSEDITOR_CTS_NODE_BASE_H_

#include "engine-common/dialogue/CDialogueTree.h"

namespace before
{
	class CtsNodeBase
	{
	public:

		//		SaveOSFText ( file ) : writes text for current node
		// Should be overridden by the representing node type to save correct data.
		virtual void SaveOSFText ( FILE* file );

		virtual cutsceneNodeType_t NodeType ( void ) { return DIALOGUE_NULL; }
	};
}


#endif//_EDITOR_STANDALONE_CTSEDITOR_CTS_NODE_BASE_H_
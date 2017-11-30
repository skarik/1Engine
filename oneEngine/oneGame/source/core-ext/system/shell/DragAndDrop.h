//===============================================================================================//
//
//		core-ext/system/shell/DragAndDrop.h
//
// Contains shell-specific feature interfaces
//
//===============================================================================================//
#ifndef CORE_EXT_SYSTEM_SHELL_DRAG_AND_DROP_H_
#define CORE_EXT_SYSTEM_SHELL_DRAG_AND_DROP_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/math/vect2d_template.h"

namespace core
{
	namespace shell
	{
		struct arDragAndDropEntry
		{
			arstring256	filename;
			Vector2i	point;
		};

		//		AddDragAndDropEntry ( ) : Adds a drag and drop entry to list.
		CORE_API void AddDragAndDropEntry ( const arDragAndDropEntry& entry );
		//		PopDragAndDropEntry ( ) : Pops a drag and drop entry from the list.
		// Order is first in, first out. Returns false when queried list is empty.
		CORE_API bool PopDragAndDropEntry ( arDragAndDropEntry& entry );
	}
}

#endif//CORE_EXT_SYSTEM_SHELL_DRAG_AND_DROP_H_
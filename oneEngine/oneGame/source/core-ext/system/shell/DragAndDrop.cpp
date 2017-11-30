#include "DragAndDrop.h"
#include <list>

static std::list<core::shell::arDragAndDropEntry> l_entries;

//		AddDragAndDropEntry ( ) : Adds a drag and drop entry to list.
void core::shell::AddDragAndDropEntry ( const arDragAndDropEntry& entry )
{
	l_entries.push_back(entry);
}
//		PopDragAndDropEntry ( ) : Pops a drag and drop entry from the list.
// Order is first in, first out. Returns false when queried list is empty.
bool core::shell::PopDragAndDropEntry ( arDragAndDropEntry& entry )
{
	if (l_entries.empty()) 
	{
		return false;
	}
	else
	{
		entry = l_entries.front();
		l_entries.pop_front();
		return true;
	}
}
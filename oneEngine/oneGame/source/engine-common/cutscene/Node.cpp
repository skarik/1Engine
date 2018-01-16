#include "Node.h"

//	IOSetKey( key, value ) : Sets internal data
void common::cts::Node::IOSetKey ( const ENodeKeyType key, const char* value )
{
	// Nothing right now.
}

//	IOGetKeys( array ) : Get an array of keys that represent this node
// NULL may be passed in to retrieve the size of the needed data.
int common::cts::Node::IOGetKeys ( nodeKeyValue* keyValueArray )
{
	return 0;
}
#include "NodeChoicebox.h"
#include <string>
#include <algorithm>

common::cts::NodeChoicebox::NodeChoicebox ( void )
	: Node(), m_outputNodeCount(0)
{
	// Clear out update nodes
	memset(m_outputNodes, 0, sizeof(m_outputNodes));
}

//	IOSetOutputNodeCount( count ) : Sets number of output nodes.
void common::cts::NodeChoicebox::IOSetOutputNodeCount ( const int count )
{
	m_outputNodeCount = std::max(0, std::min(3, count));
}
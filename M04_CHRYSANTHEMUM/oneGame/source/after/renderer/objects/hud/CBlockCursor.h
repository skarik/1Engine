
#ifndef _C_BLOCK_CURSOR_
#define _C_BLOCK_CURSOR_

#include "renderer/object/CRenderableObject.h"
#include "after/types/WorldVector.h"

class CBlockCursor : public CRenderableObject
{
public:
	CBlockCursor ( void );

	void SetSize ( const RangeVector& );

	bool Render ( const char pass );
private:
	RangeVector	m_size;
};

#endif
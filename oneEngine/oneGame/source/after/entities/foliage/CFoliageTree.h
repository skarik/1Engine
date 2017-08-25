
#ifndef _C_FOLIAGE_TREE_H_
#define _C_FOLIAGE_TREE_H_

// Includes
#include "CFoliage.h"

// Class Deifnition
class CFoliageTree : public CFoliage
{
public:
	CFoliageTree ( void );
	~CFoliageTree ( void );

	void Update ( void );

private:
	CModel * pTreeModel;


};

#endif
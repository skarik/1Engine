
#ifndef _FOLIAGE_TREE_PINE_
#define _FOLIAGE_TREE_PINE_

#include "after/entities/foliage/CTreeBase.h"

class FoliageTreePine : public CTreeBase
{
	ClassName( "FoliageTreePine" );
public:
	FoliageTreePine ( void );

	void GenerateTreeData ( void );

};

#endif
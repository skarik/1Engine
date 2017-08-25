
#ifndef _C_MORPHER_SET_H_
#define _C_MORPHER_SET_H_

// ==Includes==
#include "core/types/ModelData.h"

// This class actually is the container for morpher data
class CMorpherSet
{

public:
	explicit		CMorpherSet ( const unsigned int meshVertexCount, const unsigned int morphCount );
					~CMorpherSet ( void );

	int iMorphTarget;

	arModelVertex*	vertexData;

public:
	const arModelVertex*	GetMorphData ( const unsigned int nMorphId ) const;

	unsigned int	mVertexCount;
	unsigned int	mMorphCount;
};


#endif//_C_MORPHER_SET_H_
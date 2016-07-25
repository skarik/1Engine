
#include "CMorpherSet.h"

CMorpherSet::CMorpherSet ( const unsigned int meshVertexCount, const unsigned int morphCount )
	: mVertexCount( meshVertexCount ), mMorphCount( morphCount ), vertexData( NULL ), iMorphTarget(0)
{
	vertexData = new CModelVertex [mVertexCount*mMorphCount];
}
CMorpherSet::~CMorpherSet ( void )
{
	delete [] vertexData;
}


const CModelVertex*	CMorpherSet::GetMorphData ( const unsigned int nMorphId ) const
{
	if ( nMorphId >= mMorphCount ) {
		return NULL;
	}
	return &(vertexData[mVertexCount*nMorphId]);
}
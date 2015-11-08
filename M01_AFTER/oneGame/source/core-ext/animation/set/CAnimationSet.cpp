
#include "CAnimationSet.h"
#include "../curve/CAnimationCurve.h"

// Constructor
CAnimationSet::CAnimationSet ( void )
{
	// Nothing??
}
// Destructor
CAnimationSet::~CAnimationSet ( void )
{
	// Nothing??!?
}


// Export animation to targets
void CAnimationSet::Export ( std::vector<void*> & referenceList )
{
	for ( unsigned int i = 0; i < referenceList.size(); i++ )
	{
		CAnimationCurveBase* currentCurve = animMap[i];
		CAnimationCurveBase::eCurveType curveType = currentCurve->GetType();

		if ( curveType == CAnimationCurveBase::CT_XTRANSFORM )
		{
			//((CArrayAnimationCurve<Matrix4x4>*)(currentCurve))->;
			(*((XTransform*)(referenceList[i]))) = ((CAnimationCurve<XTransform>*)(currentCurve))->GetResult();
		}
		else if ( curveType == CAnimationCurveBase::CT_MATRIX4 )
		{
			//((CArrayAnimationCurve<Matrix4x4>*)(currentCurve))->;
			(*((Matrix4x4*)(referenceList[i]))) = ((CAnimationCurve<Matrix4x4>*)(currentCurve))->GetResult();
		}
	}
}


#ifndef _C_ANIMATION_CURVE_
#define _C_ANIMATION_CURVE_

#include "CAnimationCurveBase.h"
#include "core/math/matrix/CMatrix.h"
#include "core/math/XTransform.h"

// Class Definition
template <typename CurveType>
class CAnimationCurve : public CAnimationCurveBase
{
public:
	CAnimationCurve ( void ) {;};
	virtual ~CAnimationCurve ( void ) {;};
	
	virtual CurveType GetResult ( void )
	{
		CurveType temp = currentValue;
		currentValue = zeroValue;
		return temp;
	};

	eCurveType GetType ( void );

	CurveType currentValue;
	CurveType zeroValue;
};


// Specific shiz
inline CAnimationCurveBase::eCurveType CAnimationCurve<Matrix4x4>::GetType ( void )
{
	return CT_MATRIX4;
}
inline CAnimationCurveBase::eCurveType CAnimationCurve<XTransform>::GetType ( void )
{
	return CT_XTRANSFORM;
}

#endif
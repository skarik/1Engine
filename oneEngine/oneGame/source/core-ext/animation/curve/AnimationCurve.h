#ifndef CORE_EXT_ANIMATION_CURVE_
#define CORE_EXT_ANIMATION_CURVE_

#include "AnimationCurveBase.h"
#include "core/math/matrix/CMatrix.h"
#include "core/math/XTransform.h"

// Class Definition
template <typename CurveType>
class AnimationCurve : public AnimationCurveBase
{
public:
	AnimationCurve ( void ) {;};
	virtual ~AnimationCurve ( void ) {;};
	
	virtual CurveType GetResult ( void )
	{
		CurveType temp = currentValue;
		currentValue = zeroValue;
		return temp;
	};

	ECurveType GetType ( void );

	CurveType currentValue;
	CurveType zeroValue;
};


// Specific shiz
template <>
inline AnimationCurveBase::ECurveType AnimationCurve<Matrix4x4>::GetType ( void )
{
	return kCurveTypeMatrix4;
}
template <>
inline AnimationCurveBase::ECurveType AnimationCurve<XTransform>::GetType ( void )
{
	return kCurveTypeXTransform;
}

#endif//CORE_EXT_ANIMATION_CURVE_
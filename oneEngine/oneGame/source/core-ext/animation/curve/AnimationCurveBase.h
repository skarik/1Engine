
#ifndef _C_ANIMATION_CURVE_BASE_
#define _C_ANIMATION_CURVE_BASE_

#include "core/types/float.h"

class AnimationCurveBase
{
public:
	enum ECurveType
	{
		kCurveTypeDefault,
		kCurveTypeMatrix4,
		kCurveTypeMatrix2,
		kCurveTypeVector4,
		kCurveTypeVector3,
		kCurveTypeVector2,
		kCurveTypeFloat,
		kCurveTypeXTransform,
	};
	virtual ECurveType GetType ( void )
	{
		return kCurveTypeDefault;
	}

	virtual void SampleAt ( Real time, Real weight, bool additive=false ) =0;
};

#endif
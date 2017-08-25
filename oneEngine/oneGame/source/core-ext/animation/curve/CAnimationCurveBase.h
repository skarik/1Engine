
#ifndef _C_ANIMATION_CURVE_BASE_
#define _C_ANIMATION_CURVE_BASE_

#include "core/types/float.h"

class CAnimationCurveBase
{
public:
	enum eCurveType
	{
		CT_DEFAULT,
		CT_MATRIX4,
		CT_MATRIX2,
		CT_VECTOR3,
		CT_VECTOR2,
		CT_FLOAT,
		CT_XTRANSFORM
	};
	virtual eCurveType GetType ( void )
	{
		return CT_DEFAULT;
	}

	virtual void SampleAt ( Real time, Real weight, bool additive=false ) =0;
};

#endif
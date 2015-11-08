
#ifndef _C_ANIMATION_SET_
#define _C_ANIMATION_SET_

#include "core-ext/animation/CAnimationCommon.h"
#include "core-ext/animation/curve/CAnimationCurveBase.h"
#include "core-ext/animation/curve/CAnimationCurve.h"

#include <vector>
#include <map>

class CAnimationSet
{
public:
	explicit CAnimationSet ( void );
	virtual ~CAnimationSet ( void );
	
	void Add ( CAnimationCurve<Matrix4x4>* newCurve )
	{
		animMap.push_back( newCurve );
	}
	void Add ( CAnimationCurve<Matrix2x2>* );
	void Add ( CAnimationCurve<Vector3d>* );
	void Add ( CAnimationCurve<Vector2d>* );
	void Add ( CAnimationCurve<ftype>* );
	void Add ( CAnimationCurve<XTransform>* newCurve )
	{
		animMap.push_back( newCurve );
	}

	virtual void Export ( std::vector<void*> & );

	/*struct sCurveReference
	{
		void*	targetObject;
		CAnimationCurveBase*	baseCurve;
	};*/

	//vector<sCurveReference>	animMap;
	std::vector<CAnimationCurveBase*>	animMap;

private:
	/*map<Matrix4x4*,CAnimationCurve<Matrix4x4>>	animMap_m4;
	map<Matrix2x2*,CAnimationCurve<Matrix2x2>>	animMap_m2;
	map<Vector3d*,CAnimationCurve<Vector3d>>	animMap_v3;
	map<Vector2d*,CAnimationCurve<Vector2d>>	animMap_v2;
	map<ftype*,CAnimationCurve<ftype>>			animMap_f;*/
	//map<unsigned int,sCurveReference>	animMap;
	
};


#endif
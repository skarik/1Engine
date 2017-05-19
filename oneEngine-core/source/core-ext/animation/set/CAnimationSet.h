
#ifndef CORE_ANIMATION_SET_
#define CORE_ANIMATION_SET_

#include "core/containers/arstring.h"
#include "core-ext/animation/CAnimationCommon.h"
#include "core-ext/animation/curve/CAnimationCurveBase.h"
#include "core-ext/animation/curve/CAnimationCurve.h"

#include <vector>
#include <map>

class CAnimation;

//	class CAnimationSet
// Base class for storage for raw animation data and action definitions.
// Animation sets should be expected to be accessed when the animation is sampled.
class CAnimationSet
{
public:
	CORE_API explicit CAnimationSet ( void );
	CORE_API virtual ~CAnimationSet ( void );
	
	void Add ( CAnimationCurve<Matrix4x4>* newCurve )
	{
		throw core::DeprecatedCallException();
		//animMap.push_back( newCurve );
	}
	void Add ( CAnimationCurve<Matrix2x2>* );
	void Add ( CAnimationCurve<Vector3d>* );
	void Add ( CAnimationCurve<Vector2d>* );
	void Add ( CAnimationCurve<Real>* );
	void Add ( CAnimationCurve<XTransform>* newCurve )
	{
		throw core::DeprecatedCallException();
		//animMap.push_back( newCurve );
	}

	CORE_API virtual void Export ( std::vector<void*> & );

	/*struct sCurveReference
	{
		void*	targetObject;
		CAnimationCurveBase*	baseCurve;
	};*/

	//vector<sCurveReference>	animMap;
	//std::vector<CAnimationCurveBase*>	animMap;

	//	AddActions ( target ) : [SLOW] Adds this animation set's associated actions to the given Animation object 
	// Finds the filename for the animation, and loads in the listing of the animation.
	// This function reads from the disk, so is fairly slow. This should be kept in mind when reading in the data.
	// Ownership of the calling CAnimationSet is not transferred.
	CORE_API virtual void AddActions ( CAnimation* target );

	Real	Framerate ( void ) const
	{
		return framerate;
	}

private:
	//map<unsigned int,sCurveReference>	animMap;

	// Framerate of the source data
	Real framerate;
	// Filename of where the animation set is sourced
	arstring256 filename;
};

#endif//CORE_ANIMATION_SET_
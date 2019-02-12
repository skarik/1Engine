#ifndef CORE_ANIMATION_SET_
#define CORE_ANIMATION_SET_

#include "core/containers/arstring.h"
#include "core-ext/animation/Common.h"
#include "core-ext/animation/curve/AnimationCurveBase.h"
#include "core-ext/animation/curve/AnimationCurve.h"

#include <vector>
#include <map>

class AnimationControl;

//	class AnimationSet
// Base class for storage for raw animation data and action definitions.
// Animation sets should be expected to be accessed when the animation is sampled.
class AnimationSet
{
public:
	CORE_API explicit AnimationSet ( void );
	CORE_API virtual ~AnimationSet ( void );
	
	void Add ( AnimationCurve<Matrix4x4>* newCurve )
	{
		throw core::DeprecatedCallException();
		//animMap.push_back( newCurve );
	}
	void Add ( AnimationCurve<Matrix2x2>* );
	void Add ( AnimationCurve<Vector3f>* );
	void Add ( AnimationCurve<Vector2f>* );
	void Add ( AnimationCurve<Real>* );
	void Add ( AnimationCurve<XTransform>* newCurve )
	{
		throw core::DeprecatedCallException();
		//animMap.push_back( newCurve );
	}

	CORE_API virtual void Export ( std::vector<void*> & );

	/*struct sCurveReference
	{
		void*	targetObject;
		AnimationCurveBase*	baseCurve;
	};*/

	//vector<sCurveReference>	animMap;
	//std::vector<AnimationCurveBase*>	animMap;

	//	AddActions ( target ) : [SLOW] Adds this animation set's associated actions to the given Animation object 
	// Finds the filename for the animation, and loads in the listing of the animation.
	// This function reads from the disk, so is fairly slow. This should be kept in mind when reading in the data.
	// Ownership of the calling AnimationSet is not transferred.
	CORE_API virtual void AddActions ( AnimationControl* target );

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
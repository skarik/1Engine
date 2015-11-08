
#ifndef _C_ANIMATION_CURVE_ARRAY_
#define _C_ANIMATION_CURVE_ARRAY_

#include "CAnimationCurve.h"

// Uses vector
#include <vector>
//using std::vector;

// Class Definition
template <typename CurveType>
class CArrayAnimationCurve : public CAnimationCurve<CurveType>
{
public:
	CArrayAnimationCurve ( void )
	{
		//zeroValue = vData[0];
	};
	~CArrayAnimationCurve ( void )
	{
		vData.clear();	
	};

	void SampleAt ( ftype time, ftype weight, bool additive=false );

	void AddValue ( CurveType& newValue )
	{
		vData.push_back( newValue );
		zeroValue = vData[0];
	};

protected:
	CurveType LerpValues ( CurveType const&, CurveType const& );

private:
	std::vector<CurveType>	vData;
};

// Specifics
/*inline CAnimationCurveBase::eCurveType CAnimationCurve<Matrix4x4>::GetType ( void )
{
	return CT_MATRIX4;
}*/

// Sample At
inline void CArrayAnimationCurve<Matrix4x4>::SampleAt( ftype time, ftype weight, bool additive )
{
	Matrix4x4	targetMatx;

	// First find target val
	Matrix4x4	matx2;
	unsigned int nextindex = (unsigned int)(ceil( time ));
	if ( nextindex >= vData.size() )
		nextindex = 0;
	matx2 = vData[nextindex];
	targetMatx = vData[(unsigned int)(floor( time ))];

	ftype intpart, fracpart;
	fracpart = modf ( time, &intpart );
	targetMatx.Lerp( matx2, fracpart );

	// Now lerp towards that value
	if ( !additive )
		currentValue.Lerp( targetMatx, weight );
	else
		currentValue += targetMatx*weight; // lol, this is so wrong (TODO)
}

// Sample At
inline void CArrayAnimationCurve<XTransform>::SampleAt( ftype time, ftype weight, bool additive )
{
	XTransform	trans1;

	// First find target val
	XTransform	trans2;
	unsigned int nextindex = (unsigned int)(ceil( time ));
	if ( nextindex >= vData.size() )
		nextindex = 0;
	trans2 = vData[nextindex];
	trans1 = vData[(unsigned int)(floor( time ))];

	ftype intpart, fracpart;
	fracpart = modf ( time, &intpart );

	trans1.position	= trans1.position.lerp( trans2.position, fracpart );
	trans1.rotation = trans1.rotation.Lerp( trans2.rotation, fracpart );
	trans1.scale	= trans1.scale.lerp( trans2.scale, fracpart );

	// Now lerp towards that value
	if ( !additive )
	{
		currentValue.position	= currentValue.position.lerp( trans1.position, weight );
		currentValue.rotation	= currentValue.rotation.Slerp( trans1.rotation, weight );
		currentValue.scale		= currentValue.scale.lerp( trans1.scale, weight );
	}
	else // still wrong
	{
		//currentValue += targetMatx*weight; // lol, this is so wrong (TODO)
		currentValue.position	+= trans1.position*weight;
		//currentValue.rotation	+= trans1.rotation*weight;
		currentValue.scale		+= trans1.scale * weight;
	}
}

#endif
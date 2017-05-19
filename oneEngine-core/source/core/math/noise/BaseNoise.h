//===============================================================================================//
//
//		class BaseNoise
//
// Abstract base class for a noise generator.
//
//===============================================================================================//
#ifndef CORE_NOISE_BASE_H_
#define CORE_NOISE_BASE_H_

class BaseNoise
{
public:
	virtual float Get(float x,float y)=0;
	virtual float Get3D(float x,float y,float z)=0;

	virtual float Unnormalize(float result) { return result; }

	bool unnormalize;

};

#endif//CORE_NOISE_BASE_H_
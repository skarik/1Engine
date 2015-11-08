
#ifndef _BASE_NOISE_H_
#define _BASE_NOISE_H_

class BaseNoise
{
public:
	virtual float Get(float x,float y)=0;
	virtual float Get3D(float x,float y,float z)=0;

	virtual float Unnormalize(float result) { return result; }

	bool unnormalize;

};

#endif
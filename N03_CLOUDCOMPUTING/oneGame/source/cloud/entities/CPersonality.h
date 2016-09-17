#ifndef _C_PERSONALITY_H_
#define _C_PERSONALITY_H_

#include "core/math/Vector3d.h"

class CPersonality
{
public:
	CPersonality();

	~CPersonality();

	//Likely going to need some arguments
	virtual void Execute(Vector3d &turn, Vector3d &acceleration, int &flags) = 0;

private:
	//Knowledge
	bool mPlayerLocation;
	//Timers
	int mCurrent;
};

#endif
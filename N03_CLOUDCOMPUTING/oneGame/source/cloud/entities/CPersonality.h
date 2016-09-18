#ifndef _C_PERSONALITY_H_
#define _C_PERSONALITY_H_

#include "core/math/Vector3d.h"
#include "core/math/Rotator.h"

class CCloudEnemy;

class CPersonality
{
public:
	CPersonality(){}
	CPersonality(CCloudEnemy *host);

	~CPersonality();

	virtual void Execute(Rotator &turn, Vector3d &acceleration, int &flags);

protected:
	//Pointer to host, so the location doesn't have to be passed in all the damn time
	CCloudEnemy *pHost;
	//Knowledge
	bool mPlayerLocation;
	//Timers
	int mCurrent;
};

#endif
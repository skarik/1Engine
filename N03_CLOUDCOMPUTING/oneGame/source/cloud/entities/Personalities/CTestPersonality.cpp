#include "CTestPersonality.h"
#include "core/time.h"

CTestPersonality::CTestPersonality(CCloudEnemy *host) : CPersonality(host)
{
	mTimer = 1.5;
	mSwitch = true;
}

CTestPersonality::~CTestPersonality() {}

void CTestPersonality::Execute (Rotator &turn, Vector3d &acceleration, int &flags)
{
	if (mSwitch)
	{
		turn = Rotator(0,0,1);
		acceleration = Vector3d (1.0, 0.0, 0.0);
		mTimer -= Time::deltaTime;
		if (mTimer <= 0)
		{
			mSwitch = !mSwitch;
			mTimer = 1.5;
		}
	}
	else
	{
		turn = Rotator(0,0,-1);
		acceleration.x = 1.0;
		acceleration.y = 0;
		acceleration.z = 0;
		mTimer -= Time::deltaTime;
		if (mTimer <= 0)
		{
			mSwitch = !mSwitch;
			mTimer = 1.5;
		}
	}

	flags = 0;
}
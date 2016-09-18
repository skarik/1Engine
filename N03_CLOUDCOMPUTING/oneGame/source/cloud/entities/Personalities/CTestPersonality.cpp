#include "CTestPersonality.h"
#include "core/time.h"

CTestPersonality::CTestPersonality(CCloudEnemy *host) : CPersonality(host)
{
	mTimer = 1.0;
}

CTestPersonality::~CTestPersonality() {}

void CTestPersonality::Execute (Rotator &turn, Vector3d &acceleration, int &flags)
{
	/*if (mTimer > 0)
	{
		turn = Rotator(0,0,1);
		acceleration = Vector3d (0.0, 0.0, 0.0);
		mTimer -= Time::deltaTime;
	}*/
	//else
	{
		turn = Rotator(0,0,0);
		acceleration.x = 1.0;
		acceleration.y = 0;
		acceleration.z = 0;
	}

	flags = 0;
}
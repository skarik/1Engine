#include "CPersonalityVanguard.h"
#include "engine-common/network/playerlist.h"
#include "engine-common/entities/CActor.h"
#include "../CCloudEnemy.h"
#include "core/time.h"

CPersonalityVanguard::CPersonalityVanguard (CCloudEnemy *host) : CPersonality(host)
{
	Calc = Rotator();
	mIsRotating = true;
	mRotateTimer = 6.0;
	mChargeTimer = 0.0;
}

CPersonalityVanguard::~CPersonalityVanguard() 
{
}

void  CPersonalityVanguard::Execute (Rotator &turn, Vector3d &acceleration, int &flags)
{
	//if (mIsRotating)
	if (mRotateTimer > 0)
	{
		//Get player info
		CActor *player = Network::GetPlayerActors()[0].actor;
	
		//Turn towards the player
		Calc.RotationTo(pHost->transform.position, player->transform.position);

		turn = Calc;

		acceleration = Vector3d(0.0, 0.0, 0.0);
		
		mRotateTimer -= Time::deltaTime;

		/*mRotateTimer -= Time::deltaTime;
		if (mRotateTimer <= 0.0)
		{
			mIsRotating = !mIsRotating;
			mChargeTimer = 3.0;
		}*/
	}
	else
	{
		turn = Rotator(0,0,0);
		acceleration = Vector3d(1.0, 0.0, 0.0);
	}
	//Sprint. Don't sprint
	flags = 0;
}
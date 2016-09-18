#include "CPersonalityVanguard.h"
#include "engine-common/network/playerlist.h"
#include "engine-common/entities/CActor.h"
#include "../CCloudEnemy.h"
#include "core/time.h"

CPersonalityVanguard::CPersonalityVanguard (CCloudEnemy *host) : CPersonality(host)
{
	Calc = Rotator();
	mIsRotating = true;
	mRotateTimer = 2.0;
	mChargeTimer = 0.0;
}

CPersonalityVanguard::~CPersonalityVanguard() 
{
}

void  CPersonalityVanguard::Execute (Rotator &turn, Vector3d &acceleration, int &flags)
{
	//if (mIsRotating)
	{
		CActor *player = Network::GetPlayerActors()[0].actor;

		//Turn towards the player
		Calc.RotationTo(pHost->transform.Forward(), (player->transform.position - pHost->transform.position).normal());

		turn = Calc;
		acceleration = Vector3d(1.0, 0.0, 0.0);

		flags = 0;
		/*mRotateTimer -= Time::deltaTime;
		if (mRotateTimer <= 0)
		{
			mChargeTimer = 2.0;
			mIsRotating = !mIsRotating;
		}*/
	}
	/*else
	{
		turn = Rotator(0, 0, 0);
		acceleration = Vector3d(1.0, 0.0, 0.0);

		flags = 0;
		mChargeTimer -= Time::deltaTime;
		if (mChargeTimer <= 0)
		{
			mRotateTimer = 2.0;
			mIsRotating = !mIsRotating;
		}
	}*/
}
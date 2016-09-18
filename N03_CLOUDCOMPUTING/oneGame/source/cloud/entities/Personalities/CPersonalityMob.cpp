#include "CPersonalityMob.h"
#include "engine-common/network/playerlist.h"
#include "../CCloudEnemy.h"
#include "../CPersonalityFactory.h"

int CPersonalityMob::mFireCount = 20;
float CPersonalityMob::mRegenTime = Time::currentTime;

CPersonalityMob::CPersonalityMob(CCloudEnemy *host) : CPersonality(host)
{

}

CPersonalityMob::~CPersonalityMob()
{

}

void CPersonalityMob::Execute(Rotator &turn, Vector3d &acceleration, int &flags)
{
	CActor *player = Network::GetPlayerActors()[0].actor;

	//Turn towards the player
	Calc.RotationTo(pHost->transform.Forward(), (player->transform.position - pHost->transform.position).normal());

	turn = Calc;

	Real dist = DistanceFromPlayer(player);

	if (dist > 75)
	{
		acceleration.x = 1.0;
		acceleration.y = 0.0;
		acceleration.z = 0.0;
	}
	else if (dist < 65)
	{
		acceleration.x = -1.0;
		acceleration.y = 0.0;
		acceleration.z = 0.0;
	}
	else
	{
		acceleration.x = 0.0;
		acceleration.y = 1.0;
		acceleration.z = -1.0;
	}

	if (CPersonalityMob::mFireCount > 0)
	{
		CActor *player = Network::GetPlayerActors()[0].actor;

		//Turn towards the player
		Rotator Calc;
		Calc.RotationTo(pHost->transform.Forward(), (player->transform.position - pHost->transform.position).normal());

		turn = Calc;

		if ((pfac->GetRand() % 100) < 50)
		{
			if (pHost->FireGun())
				mFireCount--;
		}
	}

	if (Time::currentTime - CPersonalityMob::mRegenTime > .25)
	{
		mFireCount++;
		mRegenTime = Time::currentTime;
	}
}

Real CPersonalityMob::DistanceFromPlayer(CActor *player)
{
	Real xdif = pHost->transform.position.x - player->transform.position.x;
	Real ydif = pHost->transform.position.y - player->transform.position.y;
	Real zdif = pHost->transform.position.z - player->transform.position.z;

	return sqrt(xdif *xdif + ydif * ydif + zdif * zdif);
}
#include "CPersonalityMob.h"
#include "engine-common/network/playerlist.h"
#include "../CCloudEnemy.h"

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
}

Real CPersonalityMob::DistanceFromPlayer(CActor *player)
{
	Real xdif = pHost->transform.position.x - player->transform.position.x;
	Real ydif = pHost->transform.position.y - player->transform.position.y;
	Real zdif = pHost->transform.position.z - player->transform.position.z;

	return sqrt(xdif *xdif + ydif * ydif + zdif * zdif);
}
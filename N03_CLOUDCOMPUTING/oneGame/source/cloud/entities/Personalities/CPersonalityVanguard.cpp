#include "CPersonalityVanguard.h"
#include "engine-common/network/playerlist.h"
#include "../CCloudEnemy.h"
#include "core/time.h"
#include "cloud/entities/projectiles/ProjectileBullet.h"


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
	CActor *player = Network::GetPlayerActors()[0].actor;

	//Turn towards the player
	Calc.RotationTo(pHost->transform.Forward(), (player->transform.position - pHost->transform.position).normal());

	turn = Calc;
	acceleration = Vector3d(1.0, 0.0, 0.0);

	flags = 0;
	
	if (DistanceFromPlayer(player) < 10)
		pHost->FireGun();
}

Real CPersonalityVanguard::DistanceFromPlayer(CActor *player)
{
	Real xdif = pHost->transform.position.x - player->transform.position.x;
	Real ydif = pHost->transform.position.y - player->transform.position.y;
	Real zdif = pHost->transform.position.z - player->transform.position.z;

	return sqrt(xdif *xdif + ydif * ydif + zdif * zdif);
}
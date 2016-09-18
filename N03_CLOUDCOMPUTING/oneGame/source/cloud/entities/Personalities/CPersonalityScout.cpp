#include "CPersonalityScout.h"
#include "../CCloudEnemy.h"
#include "core/time.h"
#include <random>

CPersonalityScout::CPersonalityScout(CCloudEnemy *host) : CPersonality(host)
{
	mStart = pHost->transform.position;
	mDist = 200.0;
	std::random_device rd;
	std::mt19937 mt(rd());

	switch (mt() % 3)
	{
	case 0:
		mDirection.x = -1.0;
		break;
	case 1:
		mDirection.x = 0;
		break;
	case 2:
		mDirection.x = 1.0;
		break;
	}

	switch (mt() % 3)
	{
	case 0:
		mDirection.y = -1.0;
		break;
	case 1:
		mDirection.y = 0;
		break;
	case 2:
		mDirection.y = 1.0;
		break;
	}

	switch (mt() % 3)
	{
	case 0:
		mDirection.z = -1.0;
		break;
	case 1:
		mDirection.z = 0;
		break;
	case 2:
		mDirection.z = 1.0;
		break;
	}

	if (mDirection.magnitude() < FTYPE_PRECISION)
	{
		switch(mt() % 3)
		{
		case 0:
			mDirection.x = 1;
			break;
		case 1:
			mDirection.y = 1;
			break;
		case 2:
			mDirection.z = 1;
			break;
		default:
			mDirection.x = -1;
			break;
		}
	}
}

CPersonalityScout::~CPersonalityScout() {}

void CPersonalityScout::Execute (Rotator &turn, Vector3d &acceleration, int &flags)
{
	turn = Rotator(0, 0, 0);
	flags = 0;

	if (!mIsAtDest)
	{
		acceleration = mDirection;

		if (DistanceFromStart() > mDist)
			mIsAtDest = true;
	}
	else
	{
		Vector3d velocity = pHost->GetVelocity();

		if (velocity.magnitude() > FTYPE_PRECISION)
		{
			acceleration.x = -velocity.x;
			acceleration.y = -velocity.y;
			acceleration.z = -velocity.z;
		}
	}
}

Real CPersonalityScout::DistanceFromStart(void)
{
	Real xdif = pHost->transform.position.x - mStart.x;
	Real ydif = pHost->transform.position.y - mStart.y;
	Real zdif = pHost->transform.position.z - mStart.z;

	return sqrt(xdif *xdif + ydif * ydif + zdif * zdif);
}

#include "CPersonality.h"

CPersonality::CPersonality(CCloudEnemy *host) : pHost(host)
{
	if (pHost == NULL)
		throw "CPersonality Error: NULL host";
}

CPersonality::~CPersonality() {}

void CPersonality::Execute (Rotator &turn, Vector3d &acceleration, int &flags)
{
	/*turn.x = 1.0;
	turn.y = 1.0;
	turn.z = 1.0;*/
	turn = Rotator(0, 0, 0);

	acceleration.x = 1.0;
	acceleration.y = 0;
	acceleration.z = 0;

	flags = 0;
}
#include "CPersonality.h"

CPersonality::CPersonality() {}

CPersonality::~CPersonality() {}

void CPersonality::Execute (Vector3d &turn, Vector3d &acceleration, int &flags)
{
	turn.x = 1.0;
	turn.y = 1.0;
	turn.z = 1.0;

	acceleration.x = 1.0;
	acceleration.y = 0;
	acceleration.z = 0;

	flags = 0;
}
#ifndef _C_PERSONALITY_VANGUARD_H_
#define _C_PERSONALITY_VANGUARD_H_

#include "../CPersonality.h"
#include "core/math/Rotator.h"
//#include "core/math/Quaternion.h"

class CPersonalityVanguard: public CPersonality
{
public:
	CPersonalityVanguard() {}
	CPersonalityVanguard(CCloudEnemy *host);
	~CPersonalityVanguard();

	void Execute(Rotator &turn, Vector3d &acceleration, int &flags) override;

private:
	Rotator Calc;
	bool mIsRotating;
	Real mRotateTimer;
	Real mChargeTimer;
};

#endif
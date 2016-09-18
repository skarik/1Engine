#ifndef _C_PERSONALITY_VANGUARD_H_
#define _C_PERSONALITY_VANGUARD_H_

#include "../CPersonality.h"
#include "core/math/Rotator.h"
#include "core/math/Quaternion.h"

//This personality just blindly rushes at the player 
class CPersonalityVanguard: public CPersonality
{
public:
	CPersonalityVanguard() {}
	CPersonalityVanguard(CCloudEnemy *host);
	~CPersonalityVanguard();

	void Execute(Rotator &turn, Vector3d &acceleration, int &flags) override;

private:
	Quaternion Calc;
	bool mIsRotating;
	Real mRotateTimer;
	Real mChargeTimer;
};

#endif
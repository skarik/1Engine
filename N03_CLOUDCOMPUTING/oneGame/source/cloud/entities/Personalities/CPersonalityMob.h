#ifndef _C_PERSONALITY_MOB_H_
#define _C_PERSONALITY_MOB_H_

#include "../CPersonality.h"
#include "core/math/Rotator.h"
#include "engine-common/entities/CActor.h"

class CPersonalityMob : public CPersonality
{
public:
	CPersonalityMob(){}
	CPersonalityMob(CCloudEnemy *host);
	~CPersonalityMob();

	void Execute(Rotator &turn, Vector3d &acceleration, int &flags) override;

private:
	Rotator Calc;
	Real DistanceFromPlayer(CActor *player);

};

#endif

#ifndef _C_TEST_PERSONALITY_H_
#define _C_TEST_PERSONALITY_H_

#include "../CPersonality.h"
#include "core/math/Rotator.h"

class CTestPersonality: public CPersonality
{
public:
	CTestPersonality(){}
	CTestPersonality(CCloudEnemy *host);
	~CTestPersonality();

	void Execute(Rotator &turn, Vector3d &acceleration, int &flags) override;

private:
	Real mTimer;
};

#endif
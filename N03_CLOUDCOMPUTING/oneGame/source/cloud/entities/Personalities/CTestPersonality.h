#ifndef _C_TEST_PERSONALITY_H_
#define _C_TEST_PERSONALITY_H_

#include "../CPersonality.h"

class CTestPersonality: public CPersonality
{
public:
	CTestPersonality();
	~CTestPersonality();

	void Execute(Vector3d &turn, Vector3d &acceleration, int &flags) override;
};

#endif
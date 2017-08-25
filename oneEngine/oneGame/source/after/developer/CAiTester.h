
#ifndef _C_AI_TESTER_H_
#define _C_AI_TESTER_H_

#include "engine/behavior/CGameBehavior.h"

class CAiTester : public CGameBehavior
{

public:
	CAiTester ( void );

	void Update ( void ) override;

};

#endif//_C_AI_TESTER_H_
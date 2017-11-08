//===============================================================================================//
//
//		CTime : Timer class, MacOS
//
//===============================================================================================//
#if (__APPLE__ && TARGET_OS_MAC)

#ifndef C_TIME_
#define C_TIME_

#include "core/os.h"
#include "core/types/types.h"
#include "core/types/float.h"

class CTime
{
public:
	static void Init();
	static void Tick();

public:
	static Real deltaTime;
	static Real smoothDeltaTime;

	static Real fixedTime;
	static Real targetFixedTime;

private:
	static clock_t iLastTick;
	static clock_t iCurrentTick;

	static Real fDeltaTimes[10];

};

typedef CTime Time;

#endif//C_TIME_
#endif
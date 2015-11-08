#ifdef __APPLE__
	#ifdef TARGET_OS_MAC

// Includes
#include "CTimeOsmac.h"

// Static variable declares
ftype CTime::deltaTime;
ftype CTime::smoothDeltaTime;

ftype CTime::fixedTime;
ftype CTime::targetFixedTime;

clock_t CTime::iLastTick;
clock_t CTime::iCurrentTick;

ftype CTime::fDeltaTimes [10];

// Timer initialization
//  Sets default timer values
//  Grabs current tick
void CTime::Init ( void )
{
	deltaTime = 0.01f;
	smoothDeltaTime = 0.01f;
	fixedTime = 0.05f;
	targetFixedTime = 0.05f;

	for ( char i = 0; i < 10; i += 1 )
	{
		fDeltaTimes[i] = 0.01f;
	}

	iLastTick = clock();
	iCurrentTick = clock();
}

// Timer Tick
//  updates delta time values
void CTime::Tick ( void )
{
	iLastTick = iCurrentTick;
	iCurrentTick = clock();

	smoothDeltaTime = 0.0f;
	for ( char i = 9; i > 0; i -= 1 )
	{
		fDeltaTimes[i] = fDeltaTimes[i-1];
		smoothDeltaTime += fDeltaTimes[i];
	}
	fDeltaTimes[0] = ftype( ftype( iCurrentTick-iLastTick )/CLOCKS_PER_SEC );
	smoothDeltaTime += fDeltaTimes[0];

	// Set output
	deltaTime = fDeltaTimes[0];
	smoothDeltaTime = smoothDeltaTime * 0.1f;
}

	#endif
#endif
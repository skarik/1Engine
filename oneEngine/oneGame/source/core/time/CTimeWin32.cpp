
// Includes
#include "CTimeWin32.h"
#include <algorithm>

// Static variable declares
Real CTime::deltaTime;
Real CTime::limitedDeltaTime;
Real CTime::smoothDeltaTime;

Real CTime::fixedTime;
Real CTime::targetFixedTime;

Real CTime::currentTime;

LARGE_INTEGER CTime::iLastTick;
LARGE_INTEGER CTime::iCurrentTick;

LARGE_INTEGER CTime::iFrequency;

Real CTime::fDeltaTimes [10];

// Timer initialization
//  Sets default timer values
//  Grabs current tick
void CTime::Init ( void )
{
	deltaTime = 0.01f;
	limitedDeltaTime = 0.01f;
	smoothDeltaTime = 0.01f;
	fixedTime = 1.0F / 120;//0.01f;
	targetFixedTime = fixedTime;//0.01f;
	//targetFixedTime = 1/60.0f;
	//targetFixedTime = 1/15.0f;
	currentTime = 0.0f;

	for ( char i = 0; i < 10; i += 1 )
	{
		fDeltaTimes[i] = 0.01f;
	}

	//iLastTick = clock();
	//iCurrentTick = clock();
	QueryPerformanceCounter( &iLastTick );
	QueryPerformanceCounter( &iCurrentTick );
	QueryPerformanceFrequency( &iFrequency );
}

// Timer Tick
//  updates delta time values
void CTime::Tick ( void )
{
	iLastTick = iCurrentTick;
	QueryPerformanceCounter( &iCurrentTick );

	smoothDeltaTime = 0.0f;
	for ( char i = 9; i > 0; i -= 1 )
	{
		fDeltaTimes[i] = fDeltaTimes[i-1];
		smoothDeltaTime += fDeltaTimes[i];
	}
	fDeltaTimes[0] = std::min<Real>( 1.0f/5.0f, Real( Real( iCurrentTick.QuadPart-iLastTick.QuadPart )/iFrequency.QuadPart ) );
	smoothDeltaTime += fDeltaTimes[0];

	// Set output
	deltaTime = fDeltaTimes[0];
	smoothDeltaTime = smoothDeltaTime * 0.1f;

	currentTime += deltaTime;
}

// Timer get trainer factor.
//   Returns factor for "a+=(t-a)*f" expressions
#include <algorithm>
Real CTime::TrainerFactor ( Real input )
{
	return std::min<Real>( input*smoothDeltaTime*40.0f, 0.95f );
}
Real CTime::SqrtTrainerFactor ( Real input )
{
	return std::min<Real>( input*sqrt(smoothDeltaTime)*6.3f, 0.96f );
}


// Timer return current CPU time in seconds
Real CTime::CurrentTime ( void )
{
	LARGE_INTEGER tempTime;
	QueryPerformanceCounter( &tempTime );

	return Real(tempTime.QuadPart)/iFrequency.QuadPart;
}

// Timer return current CPU time in milliseconds
Real_d CTime::GetCurrentCPUTime ( void )
{
	LARGE_INTEGER tempTime;
	QueryPerformanceCounter( &tempTime );

	return (Real_d(tempTime.QuadPart)/iFrequency.QuadPart)*1000;
}
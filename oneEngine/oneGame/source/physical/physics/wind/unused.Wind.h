
#ifndef _BASE_WIND_TESTER_H_
#define _BASE_WIND_TESTER_H_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3.h"

// Class Definition
class CBaseWindTester
{
protected:
	static CBaseWindTester*	Active;
	// Hidden constructor only visible to child classes
	PHYS_API explicit CBaseWindTester ( void );
public:
	PHYS_API static CBaseWindTester* Get ( void );

	// Returns the fluid velocity at the given position
	virtual Vector3f	GetFlowField	( const Vector3f& pos ) = 0;
	virtual Vector3f	GetFlowFieldFast( const Vector3f& pos ) = 0;

	// Returns the humidity at the given position
	virtual Real		GetHumidity		( const Vector3f& pos ) = 0;
	virtual Real		GetHumidityFast	( const Vector3f& pos ) = 0;

	// Returns the temperature at the given position
	virtual Real		GetTemperature  ( const Vector3f& pos ) = 0;
	virtual Real		GetTemperatureFast ( const Vector3f& pos ) = 0;

	// Returns the cloud density at the given position
	virtual Real		GetCloudDensity ( const Vector3f& pos ) = 0;

	// Returns the calculated weather at the given position
	virtual uchar		GetWeather		( const Vector3f& pos ) = 0;

	// Sets the weather at the given position. Forces the weather system to add bullshit values.
	virtual bool		SetWeather		( const Vector3f& pos, const uchar weather ) = 0;
};
typedef CBaseWindTester WindTester; 

#endif//_BASE_WIND_TESTER_H_
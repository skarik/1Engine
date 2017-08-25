

#ifndef _WIND_MOTION_H_
#define _WIND_MOTION_H_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3d.h"

#include "physical/physics/wind/Wind.h"

// Needed Typdef
//typedef unsigned int r_bitmask;

// Class Prototpes
class CWeatherSimulator;

// Class Definition
class CWindMotion : public CBaseWindTester
{
public:
	CWindMotion ( void );

	// Returns the fluid velocity at the given position
	Vector3d	GetFlowField	( const Vector3d& pos );
	Vector3d	GetFlowFieldFast( const Vector3d& pos );

	// Returns the humidity at the given position
	ftype		GetHumidity		( const Vector3d& pos );
	ftype		GetHumidityFast	( const Vector3d& pos );

	// Returns the temperature at the given position
	ftype		GetTemperature  ( const Vector3d& pos );
	ftype		GetTemperatureFast ( const Vector3d& pos );

	// Returns the cloud density at the given position
	ftype		GetCloudDensity ( const Vector3d& pos );

	// Returns the calculated weather at the given position
	uchar		GetWeather		( const Vector3d& pos );


	// Sets the weather at the given position. Forces the weather system to add bullshit values.
	bool		SetWeather		( const Vector3d& pos, const uchar weather );

private:
	CWeatherSimulator*	pActiveSim;
	bool GetSimulationInstance ( void );
};

// Global Instance
extern CWindMotion WindMotion;

#endif
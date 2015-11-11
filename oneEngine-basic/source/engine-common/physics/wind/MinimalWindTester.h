
#ifndef _ENGINE_COMMON_MINIMAL_WIND_TESTER_H_
#define _ENGINE_COMMON_MINIMAL_WIND_TESTER_H_

#include "physical/physics/wind/Wind.h"
#include "core/math/random/Random.h"
#include "core/math/Math.h"
#include "core/time/time.h"

class MinimalWindTester : public CBaseWindTester
{
public:
	MinimalWindTester ( void ) 
		: CBaseWindTester()
	{
		;
	}

	// Returns the fluid velocity at the given position
	virtual Vector3d	GetFlowField	( const Vector3d& pos ) override
	{
		return
			Vector3d( sinf(Time::currentTime*0.23f)*2.0f,sinf(Time::currentTime*0.25f)*2.0f,sinf(Time::currentTime*0.07f)*0.2f )
			+Vector3d( Random.Range(-1,1),Random.Range(-1,1),Random.Range(-1,1) )*0.3f;
	}
	virtual Vector3d	GetFlowFieldFast( const Vector3d& pos ) override
	{
		return GetFlowField(pos);
	}

	// Returns the humidity at the given position
	virtual Real		GetHumidity		( const Vector3d& pos ) 
	{
		return 0;
	}
	virtual Real		GetHumidityFast	( const Vector3d& pos )
	{
		return 0;
	}

	// Returns the temperature at the given position
	virtual Real		GetTemperature  ( const Vector3d& pos )
	{
		return 1;
	}
	virtual Real		GetTemperatureFast ( const Vector3d& pos )
	{
		return 1;
	}

	// Returns the cloud density at the given position
	virtual Real		GetCloudDensity ( const Vector3d& pos )
	{
		return 1;
	}

	// Returns the calculated weather at the given position
	virtual uchar		GetWeather		( const Vector3d& pos )
	{
		return 0;
	}

	// Sets the weather at the given position. Forces the weather system to add bullshit values.
	virtual bool		SetWeather		( const Vector3d& pos, const uchar weather )
	{
		return true;
	}

};

#endif//_ENGINE_COMMON_MINIMAL_WIND_TESTER_H_
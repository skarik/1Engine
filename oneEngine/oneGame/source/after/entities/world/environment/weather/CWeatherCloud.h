
#ifndef _C_WEATHER_CLOUD_H_
#define _C_WEATHER_CLOUD_H_

//#include "CGameBehavior.h"
#include "core/math/Vector3d.h"

class CWeatherSimulator;

class CWeatherCloud //: public CGameBehavior
{

public:
	explicit CWeatherCloud ( Vector3d const& vInPos, Vector3d const& vInSize );
	~CWeatherCloud ( void );

public:
	void Update ( void );

public:
	Vector3d position;
	Vector3d velocity;
	Vector3d size;

	bool	skip;

};

#endif
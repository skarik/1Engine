
#ifndef _ENV_HEAVY_WEATHER_H_
#define _ENV_HEAVY_WEATHER_H_

#include "engine/behavior/CGameObject.h"

class CModel;
class glMaterial;

class EnvHeavyWeather : public CGameObject
{

public:
				EnvHeavyWeather ( void );
				~EnvHeavyWeather ( void );

	void		Update ( void );

	void		SetWeather ( const uchar nWeatherType );
	void		SetInside ( const bool nIsInside );

private:
	CModel* model;

	uchar	mTargetWeather;
	uchar	mCurrentWeather;
	bool	mInside;
	bool	mWasInside;

	ftype	weatherStrength;

	glMaterial*	targetMaterial;
};

#endif//_ENV_HEAVY_WEATHER_H_
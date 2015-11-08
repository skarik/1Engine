
#ifndef _C_ENVIRONMENT_EFFECTS_H_
#define _C_ENVIRONMENT_EFFECTS_H_

#include "core/math/Color.h"
#include "engine/behavior/CGameObject.h"

class Daycycle;
class Mooncycle;
class CloudSphere;
class CExtendableGameObject;
class CParticleEmitter;
class CParticleRenderer;
class CParticleUpdater;
class CParticleModifier;
class CSoundBehavior;
class EnvHeavyWeather;
class CColorFilterShader;

class CWeatherSimulator;
class CPhysicsWindManager;

class CEnvironmentEffects : public CGameObject 
{
	ClassName( "EnvironmentEffects" );
public:

	CEnvironmentEffects ( void );
	~CEnvironmentEffects ( void );

	void Update ( void );
	void PostUpdate ( void );

	void OnEnable ( void ) override;
	void OnDisable ( void ) override;

public:
	Daycycle*		pWorldDaycycle;
	Mooncycle*		pWorldMooncycle;
	CloudSphere*	pWorldCloudsphere;
protected:
	CWeatherSimulator*	m_weatherSim;
	CPhysicsWindManager*m_weatherWind;

protected:
	// Initialize the fog effects
	virtual void InitializeFogEffects ( void );
	// Update the fog colors (pre-update)
	virtual void UpdateFogColors ( void );
	// Update the fog effects (post-update)
	virtual void UpdateFogEffects ( void );

	// Initialize the particle effects
	virtual void InitializeParticleEffects ( void );
	// Update the particle effects
	virtual void UpdateParticleEffects ( void );
	// Free the particle effects
	virtual void FreeParticleEffects ( void );

	// Initialize the sound effects
	virtual void InitializeSoundEffects ( void );
	// Update the sound effects
	virtual void UpdateSoundEffects ( void );
	// Free the sound effects
	virtual void FreeSoundEffects ( void );

	// Calculate if the camera is indoors or not
	virtual void UpdateCameraIndoor ( void );

	// Update the screen color filter effects
	virtual void UpdateColorFilter ( void );

protected:
	// Current position of the camera
	Vector3d		vCameraPosition;
	bool			bCameraUnderwater;
	bool			bCameraIndoors;
	bool			bCameraIndoorStateChanged;

	Vector3d		vLastCameraCheckPosition;
	ftype			fCameraCheckTimer;

	// Particle systems
	CExtendableGameObject*	pBiomeParticles;
	CExtendableGameObject*	pUnderwaterParticles;
	CExtendableGameObject*	pDustParticles;
	CExtendableGameObject*	pWeatherParticles;
	CParticleEmitter*		pBiomeEmitter;
	CParticleEmitter*		pUnderwaterEmitter;
	CParticleRenderer*		pUnderwaterRenderer;
	CParticleEmitter*		pDustEmitter;
	CParticleEmitter*		pWeatherEmitter;
	CParticleRenderer*		pWeatherRenderer;

	// Effect for heavy weather
	EnvHeavyWeather*		pHeavyWeatherEffect;

	// Screen effect for color filter effects
	CColorFilterShader*		pColorFilter;

	// Sounds for wind and terrain
	CSoundBehavior*			sndBeach;
	CSoundBehavior*			sndWind;
	CSoundBehavior*			sndRain;
	ftype	fBeachGainLerp;
	ftype	fBeachGain_Start;
	ftype	fBeachGain;
	ftype	fWindGain;
	ftype	fWindPitch;
	ftype	fRainGain;

	// Dust counter
	ftype	fDustCooldown;
	ftype	fDustTimer;
	ftype	fDustOffset;

	// Color multipliers
	Color	cFogMultiplier;
	Color	cFogOffset;
	ftype	fFogEndMultiplier;
	Color	cAmbientMultiplier;

	float	fFogLerp;
	Color	cFogMultiplier_Start;
	Color	cFogOffset_Start;
	ftype	fFogEndMultiplier_Start;
	Color	cAmbientMultiplier_Start;

	// Last set biome
	int		iNextBiomeType;
	int		iBiomeType;
	int		iNextWeatherType;
	int		iWeatherType;
	
	// Weather properties
	ftype	fTemperature;
	ftype	fTemperatureSmoother;

	// Cloud cover value
	ftype	fCloudCover;

};

#endif
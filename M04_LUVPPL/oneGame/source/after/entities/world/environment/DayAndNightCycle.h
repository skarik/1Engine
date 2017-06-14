#ifndef _DAY_AND_NIGHT_CYCLE_H_
#define	_DAY_AND_NIGHT_CYCLE_H_

#include "core/math/Color.h"
#include "core/math/blend_vector.h"
#include "engine/behavior/CGameBehavior.h"

class EnvSunlight;
class CModel;
class CRenderPlane;
class glMaterial;
class CCamera;

class Daycycle : public CGameBehavior
{
public:
			Daycycle ( void );
			~Daycycle( void );

	void	Update ( void );
	void	PostUpdate ( void );

	void	SetTimeOfDay ( const ftype );
	ftype	GetTimeOfDay ( void );
	void	SetSkyCoverage ( const ftype );
	Color	GetSkyobjectDiffuse ( void );
	void	SetSpaceEffect ( const Real );

	Vector3d GetSunPosition ( CCamera* );
	Vector3d GetSunDirection ( void );
public:
	// Horizon colors (also background clear color)
	blend_vector<Color> clearColors;
	// Sun glow (scatter) color
	blend_vector<Color> sunColors;
	// Fullsky color
	blend_vector<Color>	skyColors;

	// Ambient light color
	blend_vector<Color> ambientColors;

	// Ambient light offset
	Color	cAmbientOffset;

	// Dominant day cycle
	static Daycycle* DominantCycle;
private:
	Vector3d	vPosition;

	// Current time of day in seconds
	ftype		timeofDay;		
	// Speed of time
	//ftype		speed;
	//  Cloud coverage
	// 0 is almost clear skys. 1 is almost completely covered in clouds.
	// The value can be beyond the range of 0 and 1.
	// For example, A cloudless desert will often have a value of -1.
	// Regardless of spaceEffect, will still affect brightness of the the sunlight.
	ftype		cloudCoverage;	
	// Set true when daytime.
	// Is basically a read-only variable for externals.
	bool		dayPhase;
	//  Space effect
	// Amount to blend towards space colors
	// 0 for atmosphere skysphere colors
	// 1 for space skysphere colors
	Real		spaceEffect;

	// Pointers to objects that represent the look of the sky

	EnvSunlight*	skyObject;
	CModel*			skyModel;
	CModel*			starModel;
	glMaterial*		pSkyMat;
	glMaterial*		pStarMat;
	CRenderPlane*	horizonPlane;
	glMaterial*		pHorizonMat;
};

class Mooncycle : public CGameBehavior
{
public:
			Mooncycle ( void );
			~Mooncycle( void );

	void	Update ( void );
	void	PostUpdate ( void );

	void	SetTimeOfCycle ( ftype );
	void	SetSkyCoverage ( ftype );

public:

	Vector3d		vPosition;
	ftype			timeofCycle;	// Current time of cycle in seconds
	ftype			cycleLength;	// Length of cycle in seconds

	ftype			timeofFullCycle;
	ftype			fullCycleLength;

	ftype			speed;			// Speed of time
	ftype			cloudCoverage;	// Cloud coverage

	CModel*			moonModel;
	glMaterial*		pMoonMat;
};

#endif
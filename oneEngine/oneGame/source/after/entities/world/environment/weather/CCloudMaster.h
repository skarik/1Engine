
#ifndef _C_CLOUD_MASTER_H_
#define _C_CLOUD_MASTER_H_

#include <vector>
#include "core/math/Vector3d.h"

class CWeatherCloud;
class CParticleEmitterCloud;

class CCloudMaster
{
public:
	void AddCloud ( CWeatherCloud* );
	void RemoveCloud ( CWeatherCloud* );

	static CCloudMaster* pActive;
	static void Init ( void );
	static void Free ( void );

	void UpdateClouds ( void );

	ftype GetCloudDensity ( const Vector3d & position );

protected:
	friend CParticleEmitterCloud;
	//friend CWeatherSimulator;
	std::vector<CWeatherCloud*> cloudList;

};

#endif
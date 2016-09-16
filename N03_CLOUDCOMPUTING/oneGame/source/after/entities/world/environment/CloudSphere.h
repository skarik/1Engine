#ifndef _CLOUD_SPHERE_H_
#define	_CLOUD_SPHERE_H_

#include "engine/behavior/CGameBehavior.h"
#include "EnvSunlight.h"

class CModel;
class glMaterial;

class CloudSphere : public CGameBehavior
{
public:
	CloudSphere();
	~CloudSphere(void);
	void Update(void) {};
	void PostUpdate ( void );
	
	void SetTimeOfDay ( const ftype );
	void SetCloudDensity ( const ftype );

private:
	Vector3d vPosition;
	ftype		timeofDay;
	ftype		cloudDensity;

	CModel* cloudModel;
	glMaterial* pCloudMat;
};

#endif//_CLOUD_SPHERE_H_
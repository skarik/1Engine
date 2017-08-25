
#ifndef _PROJECTILE_C_BOLT_
#define _PROJECTILE_C_BOLT_

#include "../AfterBaseProjectile.h"

class CModel;

class ProjectileCBolt : public AfterBaseProjectile
{
public:
	explicit ProjectileCBolt ( const Ray &, ftype );
	~ProjectileCBolt ( void );

	void Update ( void );

	void OnHit ( CGameObject*, Item::HitType ) override;

private:
	CModel*	pModel;
};

#endif
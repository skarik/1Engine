
#ifndef _C_FX_MATERIAL_HIT_H_
#define _C_FX_MATERIAL_HIT_H_

#include "engine/behavior/CGameBehavior.h"
#include "engine/physics/material/physMaterial.h"
#include "physical/physics/raycast/RaycastHit.h"

class CFXMaterialHit : public CGameBehavior
{
public:
	enum EHitType {
		HT_HIT,
		HT_COLLIDE,
		HT_STEP
	};
public:
	ENGCOM_API explicit		CFXMaterialHit ( const physMaterial& hitMat, const RaycastHit& hitResult, const EHitType hitType );
	//						~CFXMaterialHit ( void );

	ENGCOM_API void			Update ( void );
};

#endif//_C_FX_MATERIAL_HIT_H_
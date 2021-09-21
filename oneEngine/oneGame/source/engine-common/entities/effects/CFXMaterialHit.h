#ifndef ENGINE_COMMON_EFFECTS_MATERIAL_HIT_H_
#define ENGINE_COMMON_EFFECTS_MATERIAL_HIT_H_

#include "engine/behavior/CGameBehavior.h"
#include "physical/material/physMaterial.h"
#include "physical/physics/cast/RaycastHit.h"

class CFXMaterialHit : public CGameBehavior
{
public:
	enum EHitType {
		HT_HIT,
		HT_COLLIDE,
		HT_STEP
	};
public:
	ENGCOM_API explicit		CFXMaterialHit ( const PrPhysMaterialType hitMat, const RaycastHit& hitResult, const EHitType hitType );
	//						~CFXMaterialHit ( void );

	ENGCOM_API void			Update ( void );
};

#endif//ENGINE_COMMON_EFFECTS_MATERIAL_HIT_H_
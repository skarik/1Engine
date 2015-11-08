// This is the base class for a raytracing projectile.
// Every step, it performs a collision check. On hit, it'll call OnHit().

#ifndef _C_PROJECTILE_H_
#define _C_PROJECTILE_H_

// Includes
#include "engine/behavior/CGameObject.h"
//#include "Raytracer.h"
//#include "Water.h"
//#include "Damage.h"
//#include "ProjectileEffects.h"
#include "physical/physics/raycast/RaycastHit.h"
#include "physical/physics/shapes/physShape.h"

#include "engine-common/types/Damage.h"
#include "engine-common/types/ItemProperties.h"
//#include "game/item/CWeaponItem.h"
#include "engine-common/entities/item/CItemBase.h"

// Class Prototypes
class CActor;

// Class Definition
class CProjectile : public CGameObject
{
public:
	ENGCOM_API explicit CProjectile( Ray const& rnInRay, ftype fnInSpeed, ftype fnWidth=0.2f );
	ENGCOM_API ~CProjectile ( void );

	ENGCOM_API void SetOwner ( CActor* nActor ) { mOwner = nActor; }

	// == Update ==
	ENGCOM_API void Update ( void );

	// ===Callbacks===
	ENGCOM_API virtual void OnHit ( CGameObject*, Item::HitType );
	ENGCOM_API virtual void OnEnterWater ( void );

	// ==Option Set==
	ENGCOM_API void SetDamping ( ftype fInDamping = 0.02f, ftype fInDropPoint = 0.7f, ftype fInMaxRange = 500.0f );
	ENGCOM_API void SetDamage ( const Damage & );
	// ==Movement Set==
	ENGCOM_API void SetHeading ( const Vector3d& newHeading );

	// ==Movement Get==
	ENGCOM_API Vector3d GetHeading ( void );
	// ==Property Get==
	ENGCOM_API ftype GetWidth ( void );

	// Buff Accessor for editing effects on creation
	//Debuffs::ProjectileEffects* Effects ( void ) { return &effects; };

protected:
	CActor*		mOwner;
	Ray			rStartDirection;
	ftype		fStartSpeed;
	physShape*	mProjectileShape;
	ftype		fShapeRadius;

	Vector3d	vStartPosition;

	// Motion properties
	ftype	fDamping;
	ftype	fDropPoint;
	ftype	fMaxRange;

	// Projectile properties
	Damage		dDamage;
	RaycastHit	rhLastHit;
	//Debuffs::ProjectileEffects effects;

	// Motion States
	bool		bPerformDrop;
	Vector3d	vVelocity;
	Vector3d	vPreviousPosition;
	bool		bInWater;

	// Bullet states
	Real		fDamageMultiplier;

};


#endif
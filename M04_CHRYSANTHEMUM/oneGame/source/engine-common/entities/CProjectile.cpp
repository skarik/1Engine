
// Includes
#include "CProjectile.h"
//#include "CCharacter.h"
#include "CActor.h"
#include "engine-common/entities/effects/CFXMaterialHit.h"
//#include "engine-common/entities/item/CWeaponItem.h"
#include "physical/physics/water/Water.h"
#include "physical/physics/CPhysics.h"

#include "engine/physics/raycast/Raycaster.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine-common/physics/motion/CRagdollCollision.h"

// == Constructor and Destructor ==
// Constructor
CProjectile::CProjectile( Ray const& rnInRay, ftype fnInSpeed, ftype fnWidth )
	: CGameObject(), rStartDirection( rnInRay ), fStartSpeed( fnInSpeed ), fShapeRadius( fnWidth )
{
	// Immediately set the position of the projectile
	transform.position	= rStartDirection.pos;
	vPreviousPosition	= rStartDirection.pos;
	vStartPosition		= rStartDirection.pos;

	// Immediately set the velocity
	vVelocity	= rStartDirection.dir.normal() * fStartSpeed;

	// Set default options for the rest
	fDamping	= 0.02f;
	fDropPoint	= 0.70f;
	fMaxRange	= 500.0f;

	// Set default damage
	dDamage.amount	= 10;
	dDamage.type	= DamageType::Bullet;
	dDamage.direction	= rStartDirection.dir;
	dDamage.source		= rStartDirection.pos;
	fDamageMultiplier	= 1.0f;

	// Set if in water
	if ( WaterTester::Get() && WaterTester::Get()->PositionInside( transform.position ) ) {
		bInWater = true;
		vVelocity *= 0.5f;
	}
	else {
		bInWater = false;
	}

	// Create shape
	mProjectileShape = CPhysics::CreateSphereShape( fShapeRadius );

	// Initialize buffs
	//effects.UpdateParent( this );
}

// Destructor
CProjectile::~CProjectile ( void )
{
	delete_safe( mProjectileShape );
}

// == Update ==
void CProjectile::Update ( void )
{
	// First check water
	if ( WaterTester::Get() && WaterTester::Get()->PositionInside( transform.position ) ) {
		OnEnterWater();
	}

	// Update buffs
	//effects.Update();

	// Perform movement
	vPreviousPosition = transform.position;
	if ( bPerformDrop )
	{
		vVelocity.z -= Time::deltaTime * ( 3.0f + std::max<Real>( 0, fStartSpeed*fDropPoint - vVelocity.magnitude() ) * 2.0f );
	}
	if ( fDamping > 0 )
	{
		vVelocity -= vVelocity * fDamping * Time::smoothDeltaTime;

		if ( vVelocity.magnitude() <= ( fStartSpeed*fDropPoint ) )
		{
			bPerformDrop = true;
		}
	}
	if ( vVelocity.magnitude() > fStartSpeed )
	{
		vVelocity = vVelocity.normal() * fStartSpeed;
	}
	transform.position += vVelocity * Time::deltaTime;
	//transform.rotation = vVelocity.normal().toEulerAngles();
	transform.rotation = Quaternion::CreateRotationTo( Vector3d::forward, vVelocity.normal() );

	// Check to make sure we're in range
	if ( (vPreviousPosition - vStartPosition).sqrMagnitude() > sqr( fMaxRange ) )
	{
		DeleteObject( this );
	}
	else
	{
		// Raytrace between current position and previous position
		Ray castRay;
		castRay.pos = vPreviousPosition;
		castRay.dir = vVelocity.normal();
		CGameBehavior*	hitBehavior;
		Real	multiplier = fDamageMultiplier;

		//=========================================//
		// Perform casting
		Item::HitType hittype;
		if ( Raycaster.Linecast( castRay, vVelocity.magnitude() * Time::deltaTime, mProjectileShape, &rhLastHit, 1, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), mOwner ) )
		{
			if ( rhLastHit.pHitBehavior == NULL )
			{
				hitBehavior = NULL;
				hittype = Item::HIT_UNKNOWN;
			}
			// CVoxelTerrain::terrainList[0] is always guarenteed to be the current active terrain, even if there are multiple terrains existing.
			/*else if (( !CVoxelTerrain::terrainList.empty() )&&( outHitResult->pHitBehavior == CVoxelTerrain::terrainList[0] ))
			{
				*outHitBehavior = outHitResult->pHitBehavior;
				return HIT_TERRAIN;
			}
			else if (( COctreeTerrain::GetActive() )&&( outHitResult->pHitBehavior == COctreeTerrain::GetActive() ))
			{
				*outHitBehavior = outHitResult->pHitBehavior;
				return HIT_TERRAIN;
			}*/
			else if ( rhLastHit.pHitBehavior->GetTypeName() == "Terrain" )
			{
				hitBehavior = rhLastHit.pHitBehavior;
				hittype = Item::HIT_TERRAIN;
			}
			else if ( rhLastHit.pHitBehavior->GetTypeName() == "CRagdollCollision" )
			{
				CRagdollCollision* pHitRagdoll = (CRagdollCollision*) rhLastHit.pHitBehavior;
				multiplier = pHitRagdoll->GetMultiplier( (physRigidBody*) rhLastHit.pHitBody );
				hitBehavior = pHitRagdoll->GetActor();
				if ( hitBehavior ) {
					hittype = Item::HIT_CHARACTER;
				}
			}
			else if ( ((CRigidBody*)rhLastHit.pHitBehavior)->GetOwner() == NULL )
			{
				hitBehavior = NULL;
				hittype = Item::HIT_UNKNOWN;
			}
			else if ( ((CRigidBody*)rhLastHit.pHitBehavior)->GetOwner()->GetBaseClassName() == "CFoliage_TreeBase" )
			{
				hitBehavior = ((CRigidBody*)rhLastHit.pHitBehavior)->GetOwner();
				hittype = Item::HIT_TREE;
			}
			else if ( ((CRigidBody*)rhLastHit.pHitBehavior)->GetOwner()->GetBaseClassName() == "CGameObject_TerrainProp" )
			{
				hitBehavior = ((CRigidBody*)rhLastHit.pHitBehavior)->GetOwner();
				hittype = Item::HIT_COMPONENT;
			}
			else
			{
				CGameObject* pHitObject = ((CRigidBody*)rhLastHit.pHitBehavior)->GetOwner();
				if ( pHitObject )
				{
					if ( pHitObject->GetBaseClassName() == "CActor_Character" )
					{
						hitBehavior = pHitObject;
						multiplier = 1;
						hittype = Item::HIT_CHARACTER;
					}
					else if ( pHitObject->GetTypeName() == "CActor" )
					{
						hitBehavior = pHitObject;
						multiplier = 1;
						hittype = Item::HIT_ACTOR;
					}
					else
					{
						hitBehavior = NULL;
						hittype = Item::HIT_UNKNOWN;
						std::cout << "Casted into a rigidbody that could not be classified!" << std::endl;
					}
				}
				else
				{
					hitBehavior = NULL;
					hittype = Item::HIT_UNKNOWN;
					std::cout << "Casted into a rigidbody with no associated Gamebehavior!" << std::endl;
				}
			}
		}
		else
		{
			hitBehavior = NULL;
			hittype = Item::HIT_NONE;
		}

		// Set damage multiplier
		fDamageMultiplier = multiplier;

		//=========================================//
		// Act on the cast
		//if ( Raytracer.Raycast( castRay, vVelocity.magnitude(), &rhLastHit,  ) )
		//Item::HitType hittype = CWeaponItem::Linecast( castRay, vVelocity.magnitude() * Time::deltaTime, mProjectileShape, &rhLastHit, &hitBehavior, &multiplier, 0, mOwner );
		if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
		{
			if ( hittype == Item::HIT_TERRAIN )
			{
				// Do material effects
				CFXMaterialHit* newHitEffect = new CFXMaterialHit(
					//Terrain::MaterialOf( Raycaster.HitBlock().block.block ),
					*Raycaster.m_lastHitMaterial,
					rhLastHit, CFXMaterialHit::HT_HIT );
				newHitEffect->RemoveReference();

				OnHit( (CGameObject*)rhLastHit.pHitBehavior, Item::HIT_TERRAIN );
			}
			else
			{
				switch ( hittype ) {
					case Item::HIT_TREE:
					case Item::HIT_COMPONENT:
						OnHit( (CGameObject*)hitBehavior, hittype );
						break;
					case Item::HIT_ACTOR:
					case Item::HIT_CHARACTER:
						if ( hitBehavior != mOwner ) {
							OnHit( (CGameObject*)hitBehavior, hittype );
						}
						break;
				};
			}
		}

		// End hit
	}
	// End range
}


// == Callbacks ==
void CProjectile::OnHit ( CGameObject* pHitObject, Item::HitType nHitType )
{
	// Hit information is stored in rhLastHit.

	// Check if the hit object is of type actorcharacter
	//if ( pHitObject->GetBaseClassName() == "CActor_Character" )
	if ( nHitType == Item::HIT_CHARACTER || nHitType == Item::HIT_ACTOR )
	{
		std::cout << "Projectile damage: " << dDamage.amount << " (d" << dDamage.type << ")" << std::endl;

		dDamage.actor = mOwner;

		// If it is, then hurt it. Bah. HURT IT.
		CActor*	pCharacter = (CActor*)pHitObject;
		//bool bDestroy = effects.OnHitEnemy( pCharacter );
		if ( mOwner ) mOwner->OnDealDamage( dDamage, pCharacter );
		pCharacter->OnDamaged( dDamage );

		//if ( bDestroy ) {
			// Finally, delete ourselves.
			DeleteObject( this );
		//}
	}
	else if ( nHitType == Item::HIT_TERRAIN )
	{
		//bool bDestroy = effects.OnHitWorld( rhLastHit.hitPos, rhLastHit.hitNormal );

		//if ( bDestroy ) {
			// Finally, delete ourselves.
			DeleteObject( this );
		//}
	}
	
}
void CProjectile::OnEnterWater ( void )
{
	if ( !bInWater ) {
		// What is this I don't even
		DeleteObject( this );
	}
	else {
		vVelocity *= 0.5f;
	}
}

// == Set Options ==
// Set motion damping
void CProjectile::SetDamping ( ftype fInDamping, ftype fInDropPoint, ftype fInMaxRange )
{
	fDamping	= fInDamping;
	fDropPoint	= fInDropPoint;
	fMaxRange	= fInMaxRange;
}
// Set damage
void CProjectile::SetDamage ( const Damage & dInDamage )
{
	dDamage = dInDamage;
}

// ==Movement Set==
// Set new direction of movement
void CProjectile::SetHeading ( const Vector3d& newHeading )
{
	vVelocity = newHeading.normal() * vVelocity.magnitude();
}

// ==Movement Get==
// Get the current velocity
Vector3d CProjectile::GetHeading ( void )
{
	return vVelocity;
}

// ==Property Get==
// Return width of the collision used
ftype CProjectile::GetWidth ( void )
{
	return fShapeRadius;
}
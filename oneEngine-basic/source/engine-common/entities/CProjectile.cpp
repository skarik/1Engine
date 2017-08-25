#include "CProjectile.h"
#include "CActor.h"

//#include "physical/physics/water/Water.h"
#include "physical/physics/cast/PrCast.h"
#include "physical/physics/fluid/PrWaterQuery.h"

//#include "engine/physics/collider/types/CCapsuleCollider.h"
#include "engine/physics/motion/CRigidbody.h"

#include "engine-common/physics/motion/CRagdollCollision.h"
#include "engine-common/entities/effects/CFXMaterialHit.h"

// Constructor
CProjectile::CProjectile( Ray const& rnInRay, Real fnInSpeed, Real fnWidth )
	: CGameObject(), rStartDirection( rnInRay ), fStartSpeed( fnInSpeed ), fShapeRadius( fnWidth )
{
	// Immediately set the position of the projectile
	transform.world.position	= rStartDirection.pos;
	vPreviousPosition			= rStartDirection.pos;
	vStartPosition				= rStartDirection.pos;

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
	//if ( WaterTester::Get() && WaterTester::Get()->PositionInside( transform.world.position ) ) {
	if ( PrWaterQuery( physical::water::prInside( transform.world.position ) ) )
	{
		bInWater = true;
		vVelocity *= 0.5f;
	}
	else
	{
		bInWater = false;
	}

	// Create shape
	//mProjectileShape = CPhysics::CreateSphereShape( fShapeRadius );
	//mProjectileCollider = new CCapsuleCollider( fShapeRadius * 2, fShapeRadius, true );

	// Initialize buffs
	//effects.UpdateParent( this );
}

// Destructor
CProjectile::~CProjectile ( void )
{
	//delete_safe( mProjectileShape );
	//delete_safe( mProjectileCollider );
}

// == Update ==
void CProjectile::Update ( void )
{
	// First check water
	//if ( WaterTester::Get() && WaterTester::Get()->PositionInside( transform.world.position ) ) {
	if ( PrWaterQuery( physical::water::prInside( transform.world.position ) ) )
	{
		OnEnterWater();
	}

	// Update buffs
	//effects.Update();

	// Perform movement
	vPreviousPosition = transform.world.position;
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
	transform.world.position += vVelocity * Time::deltaTime;
	//transform.rotation = vVelocity.normal().toEulerAngles();
	transform.world.rotation = Quaternion::CreateRotationTo( Vector3d::forward, vVelocity.normal() );

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
		// Cast motion forward

		Item::HitType hittype;

		// Create cast query
		prRaycastQuery l_castQuery = {0};
		l_castQuery.world = NULL;
		l_castQuery.ray = Ray(
			castRay.pos,
			castRay.dir * vVelocity.magnitude() * Time::deltaTime
		);
		l_castQuery.collision.layer = physical::layer::PHYS_BULLET_TRACE;
		l_castQuery.collision.group = 0;
		l_castQuery.collision.id = 31;
		l_castQuery.owner = mOwner.cast<void*>();
		l_castQuery.ownerType = core::kBasetypeGameBehavior;
		l_castQuery.maxHits = 1; // Stop on the first hit

		// Perform cast
		PrCast cast(l_castQuery);

		//if ( Raycaster.Linecast( castRay, vVelocity.magnitude() * Time::deltaTime, mProjectileShape, &rhLastHit, 1, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), mOwner ) )
		/*if ( Raycaster.Linecast(
			castRay, vVelocity.magnitude() * Time::deltaTime,
			mProjectileCollider->GetCollisionShape(), &rhLastHit, 1,
			Physics::GetCollisionFilter(physical::layer::PHYS_BULLET_TRACE,0,31), mOwner.cast<void*>() ) )*/
		if ( cast.Hit() ) // Same as hit<0>
		{
			CGameBehavior* l_hitObject0 = cast.HitObjectAs<0, CGameBehavior>();
			CRigidbody* l_hitObject0asRigidBody = static_cast<CRigidbody*>(l_hitObject0);

			//if ( rhLastHit.pHitBehavior == NULL )
			if ( cast.HitType<0>() != core::kBasetypeGameBehavior )
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
			//else if ( rhLastHit.pHitBehavior->GetTypeName() == "Terrain" )
			else if ( l_hitObject0->GetTypeName() == "Terrain" )
			{
				hitBehavior = l_hitObject0;
				hittype = Item::HIT_TERRAIN;
			}
			else if ( l_hitObject0->GetTypeName() == "CRagdollCollision" )
			{
				CRagdollCollision* pHitRagdoll = (CRagdollCollision*) l_hitObject0;
				multiplier = pHitRagdoll->GetMultiplier( cast.HitRigidBody<0>() );
				hitBehavior = pHitRagdoll->GetActor();
				if ( hitBehavior ) {
					hittype = Item::HIT_CHARACTER;
				}
			}
			else if ( l_hitObject0asRigidBody->GetOwner<CGameBehavior>() == NULL )
			{
				hitBehavior = NULL;
				hittype = Item::HIT_UNKNOWN;
			}
			else if ( l_hitObject0asRigidBody->GetOwner<CGameBehavior>()->GetBaseClassName() == "CFoliage_TreeBase" )
			{
				hitBehavior = l_hitObject0asRigidBody->GetOwner<CGameBehavior>();
				hittype = Item::HIT_TREE;
			}
			else if ( l_hitObject0asRigidBody->GetOwner<CGameBehavior>()->GetBaseClassName() == "CGameObject_TerrainProp" )
			{
				hitBehavior = l_hitObject0asRigidBody->GetOwner<CGameBehavior>();
				hittype = Item::HIT_COMPONENT;
			}
			else
			{
				CGameBehavior* pHitObject = l_hitObject0asRigidBody->GetOwner<CGameBehavior>();
				if ( pHitObject )
				{
					if ( pHitObject->GetBaseClassName() == "CActor_Character" )
					{
						hitBehavior = pHitObject;
						multiplier = 1;
						hittype = Item::HIT_CHARACTER;
					}
					else if ( pHitObject->GetTypeName() == "CActor" || pHitObject->GetBaseClassName() == "CActor" )
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
				/*CFXMaterialHit* newHitEffect = new CFXMaterialHit(
					*Raycaster.m_lastHitMaterial,
					rhLastHit, CFXMaterialHit::HT_HIT );
				newHitEffect->RemoveReference();*/

				//OnHit( (CGameObject*)rhLastHit.pHitBehavior, Item::HIT_TERRAIN );
				OnHit( cast.HitObjectAs<0, CGameObject>(), Item::HIT_TERRAIN );
			}
			else
			{
				switch ( hittype )
				{
					case Item::HIT_ACTOR:
					case Item::HIT_CHARACTER:
						if ( hitBehavior != mOwner.cast<CGameBehavior*>() )
						{
							OnHit( (CGameObject*)hitBehavior, hittype );
						}
						break;
					case Item::HIT_TREE:
					case Item::HIT_COMPONENT:
						OnHit( (CGameObject*)hitBehavior, hittype );
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
		// If it is, then hurt it. Bah. HURT IT.
		CActor*	pCharacter = dynamic_cast<CActor*>((CGameBehavior*)pHitObject);
		CActor* pOwner = mOwner.cast<CActor*>();

		dDamage.actor = pOwner;

		if ( pOwner != NULL )
			pOwner->OnDealDamage( dDamage, pCharacter );
		if ( pCharacter != NULL )
			pCharacter->OnDamaged( dDamage );

		// Finally, delete ourselves.
		DeleteObject( this );
	}
	else if ( nHitType == Item::HIT_TERRAIN )
	{
		// Finally, delete ourselves.
		DeleteObject( this );
	}
	
}
void CProjectile::OnEnterWater ( void )
{
	if ( !bInWater )
	{	// What is this I don't even
		DeleteObject( this );
	}
	else 
	{
		vVelocity *= 0.5f;
	}
}

// == Set Options ==
// Set motion damping
void CProjectile::SetDamping ( Real fInDamping, Real fInDropPoint, Real fInMaxRange )
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
Real CProjectile::GetWidth ( void )
{
	return fShapeRadius;
}

#include "CTerraExplosion.h"

#include "core/math/Math.h"
#include "core/debug/console.h"

#include "engine/physics/raycast/Raycaster.h"

#include "engine-common/entities/physics/CPhysExplosion.h"

#include "after/entities/character/CCharacter.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/Zones.h"
#include "after/types/terrain/BlockType.h"

CTerraExplosion::CTerraExplosion( Vector3d &vPos, ftype fMagnitude, ftype fPhysicsMultiplier )
	: CGameObject()
{
	transform.position = vPos;
	magnitude = fMagnitude;
	physics_multiplier = fPhysicsMultiplier;

	exploded = false;
}

CTerraExplosion::~CTerraExplosion ( void )
{
	// Not sure yet
}

void CTerraExplosion::Update ( void )
{
	if ( exploded )
		DeleteObject( this );
}

void CTerraExplosion::Explode ( void )
{
	CVoxelTerrain* pActiveTerrain = CVoxelTerrain::GetActive();
	if ( !pActiveTerrain )
	{
		Debug::Console->PrintError( "WARNING: Exploding terrain with no terrain to explode!\n" );
		throw Core::NoTerrainException();
		return;
	}

	throw Core::NotYetImplementedException();

	// Need to generate a sphere of random points
	// Luckily, there is a function defined in the Math class that provides a sphere of equidistant vectors. It's really great.
	/*const int pointCount = 64;
	Vector3d* vDirList = Math.pointsOnSphere( pointCount );
	if ( vDirList )
	{
		const ftype stepSize = 1.8f; // blocks are 2 feet
		// Loop through each direction
		for ( char i = 0; i < pointCount; ++i )
		{
			Vector3d vPos = transform.position;
			ftype rayPower = magnitude;
			ftype currentDistance = 0;
			// Continue moving the ray while still can (technically could have crystal blocks amplify power)
			while ( rayPower > 0 )
			{
				BlockInfo hitBlock;
				// Get the current block at the position
				if ( !pActiveTerrain->GetBlockInfoAtPosition( vPos, hitBlock ) )
					break;

				// Replace the following with Caster::Raycast

				// If the block is air, then raytrace to the position to see if there's something else to hurt
				if (( hitBlock.block.block == Terrain::EB_NONE )||( hitBlock.block.block == Terrain::EB_WATER ))
				{
					if ( hitBlock.block.block == Terrain::EB_WATER )
					{
						rayPower *= 0.7f + 0.1f; // Water doubles the slowdown rate (sort of)
					}
					if ( currentDistance > 1.0f )
					{
						// Raytrace towards the position
						RaycastHit		result;
						BlockTrackInfo	raycastBlock;
						Ray	exploRay;
						exploRay.pos	= transform.position;
						exploRay.dir	= vDirList[i];
						if ( Raytracer.Raycast( exploRay, currentDistance+1.0f, &result, &raycastBlock, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
						{
							// Check the type of hit
							if ( result.pHitBehavior == pActiveTerrain )
							{
								// no one cares
								// (might want to destroy it to be more forgiving to the player)
							}
							else
							{
								CGameBehavior* pHitObject = ((CRigidBody*)result.pHitBehavior)->GetOwner();
								if ( pHitObject->GetBaseClassName() == "CFoliage_TreeBase" )
								{
									// trees are gay...actually no, more of asexual?
								}
								else if ( pHitObject->GetBaseClassName() == "CGameObject_TerrainProp" )
								{
									// bowties are cool
								}
								else if ( pHitObject->GetBaseClassName() == "CActor_Character" )
								{
									// FINALLY. BLAST THE FECES OUT OF THE BACK END OF THIS SUCKER :D
									CCharacter* pHitCharacter = (CCharacter*)(pHitObject);

									// Deal damage to player based on the ray's power
									Damage dmg;
									dmg.amount = rayPower * 1.1f;
									dmg.type = DamageType::Blast;
									pHitCharacter->OnDamaged( dmg );

									// Now, weaken ray based on health left
									rayPower -= (ftype)(pHitCharacter->GetHealth()) * 0.7f;
								}
							}
						}
					}
				}
				else // All other blocks are considered solid. For now.
				{
					rayPower -= Terrain::cDefaultBlockHardness[hitBlock.block.block] * 0.8f;
					// If the power is still above zero, then the block is decimated
					if ( rayPower > 0 )
					{
						pActiveTerrain->ItemizeBlock( hitBlock );
					}
					//if ( hitBlock.block.block == EB_STONE )
					//	cout << (int)Terrain::cDefaultBlockHardness[hitBlock.block.block] << " _ " << rayPower << endl;
				}

				// Decrease power exponentially (as to approximate squared falloff but in a more gameplay-friendly way)
				rayPower = rayPower * 0.7f - 0.1f;
				vPos += vDirList[i]*stepSize;	// Move the ray out
				currentDistance += stepSize;
			}
		}

		// Free owned vector list
		delete vDirList;
	}
	vDirList = NULL;

	CPhysExplosion* physExplo = new CPhysExplosion ( transform.position, magnitude, physics_multiplier*120 );
	physExplo->Explode();
	physExplo->RemoveReference();

	exploded = true;*/

	// Delete this object
	DeleteObject( this );
}

//fezzes are cool
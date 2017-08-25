
// Unified AI code
// General usage routines

#include "NPC_AIState.h"

#include "physical/physics/CPhysics.h"

#include "engine/physics/raycast/Raycaster.h"
#include "engine/physics/motion/CRigidbody.h"

#include "renderer/debug/CDebugDrawer.h"

#include "after/entities/character/npc/zoned/CZonedCharacter.h"
#include "after/types/terrain/BlockType.h"



bool NPC::AIState::AI_MoveTo ( const Vector3d& target, bool nAllowSprint )
{
	ai_move.target = target;

	ftype targetDistance = (owner->transform.position - target).magnitude();

	if ( nAllowSprint ) {
		if ( targetDistance > 8.0f ) {
			ai_move.state.isRunning = true;
		}
		else {
			ai_move.state.isRunning = false;
			ai_move.state.isWalking = true;
		}
	}
	else {
		ai_move.state.isRunning = false;
		ai_move.state.isWalking = true;
	}

	if ( targetDistance > ((hull.radius+0.7f)+0.54f)*1.1f ) {
		return false;
	}
	else {
		ai_move.state.isRunning = false;
		ai_move.state.isWalking = false;
		return true;
	}
}
void NPC::AIState::AI_Move ( void )
{
	// Don't try to move it not going to move
	if ( ai_move.state.isWalking || ai_move.state.isRunning ) {
		// Prepare moving
	}
	else {
		ai_move.state.final_target = owner->transform.position;
		return; // No moving.
	}

	// Downtrace the movement target
	Ray castRay;
	castRay.dir = Vector3d( 0,0,-1 );
	castRay.pos = ai_move.target + Vector3d( 0,0,1 );
	RaycastHit hitInfo;
	// If it hits, bring it down to the ground
	if ( Raycaster.Raycast( castRay, 20.0f, &hitInfo, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) ) {
		ai_move.target = hitInfo.hitPos + hitInfo.hitNormal * 0.5f;
		ai_move.validTarget = true;
	}
	else {
		ai_move.validTarget = false;
	}

	// Check for path updating
	if (
		( ai_move.path_time <= 0.0f ) // Path is 13 seconds old
		||( ((ai_move.path_target-ai_move.target).sqrMagnitude() > 7.0f) && (ai_move.path_time < 9.0f) ) // Path is outdated and more than 4 seconds old
		)
	{
		ai_move.path_time = 13.0f;
		ai_move.path_pos = 0;
		ai_move.path.clear();
		ai_move.path_target = ai_move.target;

		// Create next path position
		Vector3d nextPoint;
		nextPoint = owner->transform.position;
		nextPoint.x -= fmodf( fabs(nextPoint.x), 2.0f );
		nextPoint.y -= fmodf( fabs(nextPoint.y), 2.0f );
		nextPoint.z -= fmodf( fabs(nextPoint.z), 2.0f );
		nextPoint += Vector3d(1,1,1);
		ai_move.path.push_back( nextPoint );

		bool makePath = true;
		short lastChoice = -1;
		short backtrackCount = 0;
		while ( makePath ) {
			Vector3d current = ai_move.path.back();

			// Now, look at the options we have in this point (basically, sample the surrounding area)
			Vector3d dirOffset [12] = {
				Vector3d(2,0,0),Vector3d(0,2,0),Vector3d(-2,0,0),Vector3d(0,-2,0),
				Vector3d(2,0,2),Vector3d(0,2,2),Vector3d(-2,0,2),Vector3d(0,-2,2),
				Vector3d(2,0,-2),Vector3d(0,2,-2),Vector3d(-2,0,-2),Vector3d(0,-2,-2)
			};
			ftype	baseWeight [12] = {
				 0, 0, 0, 0,
				10,10,10,10,
				-1,-1,-1,-1
			};
			// Reduce weight that takes us backwards
			if ( lastChoice >= 0 ) {
				short killChoice;
				switch ( lastChoice%4 ) {
					case 0:	killChoice = 2; break;
					case 1: killChoice = 3; break;
					case 2: killChoice = 0; break;
					case 3: killChoice = 1; break;
				}
				for ( uint i = 0; i < 3; ++i ) {
					baseWeight[killChoice+i*4] += 17;
				}
			}

			// Sample the first 4 directions
			ushort blockInfo [12];
			for ( uint i = 0; i < 4; ++i ) {
				blockInfo[i] = Zones.GetBlockAtPosition(current+dirOffset[i]).block;
			}
			// Check each block for a valid movespot, compute weights, and check upper positions
			for ( uint i = 0; i < 4; ++i ) {
				// Block is in the way,
				if (( blockInfo[i] != Terrain::EB_NONE )&&( blockInfo[i] != Terrain::EB_WATER ))
				{
					baseWeight[i] += 9999; // Increase weight to unusable
					baseWeight[i+8] += 9999; // Low block is also unusable (not visible)
					// Sample block above it
					blockInfo[i+4] = Zones.GetBlockAtPosition(current+dirOffset[i+4]).block;
					// Still a block in the way
					if (( blockInfo[i+4] != Terrain::EB_NONE )&&( blockInfo[i+4] != Terrain::EB_WATER )) {
						baseWeight[i+4] += 9999; // Increase weight to unusable
					}
					else { // Block is open
						// Compute block weight based on distance
						baseWeight[i+4] += (current+dirOffset[i+4]-ai_move.path_target).sqrMagnitude() * 0.4f;
					}
				}
				else
				{
					// Block is open, check downwards
					// Sample block below it
					blockInfo[i+8] = Zones.GetBlockAtPosition(current+dirOffset[i+8]).block;
					// Has a block below it.
					if (( blockInfo[i+8] != Terrain::EB_NONE )&&( blockInfo[i+8] != Terrain::EB_WATER ))
					{
						baseWeight[i+4] += 9999; // Increase weight of top block to unusable
						baseWeight[i+8] += 9999; // Low block is unusable
						// Compute block weight based on distance
						baseWeight[i] += (current+dirOffset[i]-ai_move.path_target).sqrMagnitude() * 0.4f;
					}
					else
					{ // Block is open
						baseWeight[i] += 9999; // Level block is unusable
						baseWeight[i+4] += 9999; // High block is unusable
						// Raytrace downards to check if there's a bottom
						castRay.pos = current+dirOffset[i+8];
						castRay.dir = Vector3d(0,0,-1);
						// If it hit, then there's a bottom we can go to
						if ( Raycaster.Raycast( castRay, 8.0f, &hitInfo, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) )
						{
							// Set the down block position to the hit area
							dirOffset[i+8] = hitInfo.hitPos+hitInfo.hitNormal-current;
							// Compute block weight based on distance
							baseWeight[i+8] += (current+dirOffset[i+8]-ai_move.path_target).sqrMagnitude() * 0.4f;
						}
						else
						{
							// It didn't hit, so disable that
							baseWeight[i+8] += 9999;
						}
					}

				}
				// End for loop
			}

			// Check all blocks, and find minimum cost point with cost above 0 and below 9000
			int validPathTarget = -1;
			ftype minWeight = 9999;
			for ( uint i = 0; i < 12; ++i )
			{
				if ( (baseWeight[i] > 0) && (baseWeight[i] < 9000) )
				{
					if ( minWeight > baseWeight[i] )
					{
						minWeight = baseWeight[i];
						validPathTarget = i;
					}
				}
			}
			// Add point if valid
			nextPoint = current+dirOffset[validPathTarget];
			if ( validPathTarget != -1 ) {
				ai_move.path.push_back( nextPoint );
				lastChoice = (short)validPathTarget;
			}
			else {
				makePath = false;
			}
			// Check previous path points
			for ( int i = ai_move.path.size()-2; i > 1; --i ) {
				if ( ai_move.path[i] == nextPoint ) {
					backtrackCount += 1;
					i = 0;
				}
			}
			// If too many backtracks, then stop
			if ( backtrackCount >= 7 ) {
				makePath = false;
				ai_move.validTarget = false; // No longer valid target when backtracking.
			}

			// If hit target, then stop
			if ( ai_move.validTarget ) {
				if ( (current-ai_move.path_target).sqrMagnitude() < 4.0f ) {
					makePath = false;
				}
				else if ( ai_move.path.size() > 150 ) {
					makePath = false;
				}
			}
			else {
				if ( ai_move.path.size() > 50 ) {
					makePath = false;
				}
				else if ( (current-ai_move.path_target).sqrMagnitude() < 16.0f ) {
					makePath = false;
				}
			}
			
			// Go to next point to check
		}
		if ( ai_move.validTarget && (!ai_move.path.empty()) ) { // Add last part
			ai_move.path.push_back( ai_move.path_target );
		}
		// End path creation

		// Smooth out path first based on 1-pass simple elastics
		for ( int i = 1; i < (int)ai_move.path.size()-1; ++i ) {
			Vector3d targetPos = ai_move.path[i-1].lerp( ai_move.path[i+1], 0.5f );
			ai_move.path[i] = ai_move.path[i].lerp( targetPos, 0.2f );
		}
		// Smooth out path based on collision radius
		for ( int i = 1; i < (int)ai_move.path.size()-2; i += 2 ) {
			if ( (ai_move.path[i]-ai_move.path[i+1]).sqrMagnitude() < (sqr(hull.radius+0.7f)+1.44f) )
			{
				ai_move.path[i] = ai_move.path[i].lerp( ai_move.path[i+1], 0.3f );
				ai_move.path.erase( ai_move.path.begin()+(i+1) );
				i -= 2;
			}
		}
	}
	else
	{
		// If angry, recreate path faster
		if ( ai_think.infostate == ai_think.AI_ANGRY )
		{
			ai_move.path_time -= 2 * Time::deltaTime;
		}
		else {
			ai_move.path_time -= Time::deltaTime;
		}
	}

	// Get next target point
	Vector3d nextPointTarget, lastPointTarget;
	if ( !ai_move.path.empty() )
	{
		if ( ai_move.path_pos < (int)ai_move.path.size() )
		{
			nextPointTarget = ai_move.path[ai_move.path_pos];
			lastPointTarget = ai_move.path.back();
			// If close enough to point, move to next point
			Vector3d delta = (owner->transform.position+Vector3d(0,0,1)-nextPointTarget);
			delta.z *= 0.3f; // Increase possible difference on Z axis
			if ( delta.sqrMagnitude() < (sqr(hull.radius+0.7f)+0.3f) ) {
				ai_move.path_pos += 1;
				if ( ai_move.path_pos >= (int)ai_move.path.size() ) {
					// Decrease timer a LOT (need a new path NOW)
					ai_move.path_time = std::min<ftype>( ai_move.path_time, -1.0f );
					// Stop moving now
					ai_move.state.isRunning = false;
					ai_move.state.isWalking = false;
				}
			}
			else if ( ai_move.path_pos+1 >= (int)ai_move.path.size() )
			{
				// If near end of path, recreate path faster
				ai_move.path_time -= 2 * Time::deltaTime;
			}
		}
		else
		{
			// Decrease timer a LOT (need a new path NOW)
			ai_move.path_time = std::min<ftype>( ai_move.path_time, -1.0f );
			// Stop moving now
			ai_move.state.isRunning = false;
			ai_move.state.isWalking = false;
		}
	}
	else
	{
		nextPointTarget = ai_move.target;
		lastPointTarget = ai_move.target;

		
		// If close enough to point, call success
		Vector3d delta = (owner->transform.position+Vector3d(0,0,1)-nextPointTarget);
		delta.z *= 0.3f; // Increase possible difference on Z axis
		if ( delta.sqrMagnitude() < (sqr(hull.radius+0.7f)+0.3f) )
		{
			// Decrease timer a LOT (need a new path NOW)
			ai_move.path_time = std::min<ftype>( ai_move.path_time, -1.0f );
			// Stop walking
			ai_move.state.isRunning = false;
			ai_move.state.isWalking = false;
		}

	}

	// Debug draw the path out
#ifdef _ENGINE_DEBUG
	for ( uint i = ai_move.path_pos+1; i < ai_move.path.size(); ++i ) {
		Debug::Drawer->DrawLine( ai_move.path[i-1], ai_move.path[i], Color(1,(ai_think.infostate==ai_think.AI_ANGRY) ? 0.0f : 0.5f,0) );
	}
	if ( ai_move.path_pos < (int)ai_move.path.size() ) {
		Debug::Drawer->DrawLine( owner->transform.position+Vector3d(0,0,1), ai_move.path[ai_move.path_pos], Color(1,(ai_think.infostate==ai_think.AI_ANGRY) ? 0.0f : 0.5f,0) );
		Debug::Drawer->DrawLine( owner->transform.position, nextPointTarget, Color(1,(ai_think.infostate==ai_think.AI_ANGRY) ? 0.0f : 0.5f,0) );
	}
	Debug::Drawer->DrawLine( owner->transform.position+Vector3d(0,0,1), lastPointTarget, Color(0,(ai_think.infostate==ai_think.AI_ANGRY) ? 0.0f : 0.5f,0.5f) );
	//cout << ai_move.path_pos << "/" << ai_move.path.size() << endl;
	Debug::Drawer->DrawLine( owner->transform.position, owner->transform.position+Vector3d(0,0,4), Color(0.5,0.2,1.0) );
#endif

	ai_move.state.final_target = nextPointTarget;

	if ( ai_move.state.isRunning && !ai_think.isBusy ) {
		ai_move.state.isWalking = false;
		//cout << "Running" << endl;
	}

	if ( ai_move.state.isRunning || ai_move.state.isWalking ) {
		// Face at target position
		//ai_lookat.facingPos = (nextPointTarget+lastPointTarget)*0.5f + Vector3d( 0,0,hull.height*0.6f );
		ai_lookat.walkingFacingPos = nextPointTarget + Vector3d( 0,0,hull.height*0.6f );
		ai_lookat.walkingLookatPos = (nextPointTarget+lastPointTarget)*0.5f + Vector3d( 0,0,hull.height*0.7f );

		// ==
		// Do autoduck
		bool prevCrouch	= ai_move.state.wantsCrouch;
		bool prevProne	= ai_move.state.wantsProne;

		Vector3d horizMovedir = rigidbody->GetVelocity();
		horizMovedir.z = 0;
		castRay.pos = owner->transform.position + Vector3d( 0,0,1.0f ) + ( horizMovedir.normal() * (0.2f + hull.radius * 1.1f) );
		castRay.dir = Vector3d( 0,0,1 );
		if ( Raycaster.Raycast( castRay, 5.0f, &hitInfo, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) )
		{
			if ( hitInfo.distance <= 4.0f ) {
				ai_move.state.wantsCrouch = true;
			}
			else if ( hitInfo.distance <= 2.0f ) {
				ai_move.state.wantsProne = true;
			}
			else {
				ai_move.state.wantsCrouch = false;
				ai_move.state.wantsProne = false;
			}
		}
		else {
			ai_move.state.wantsCrouch = false;
			ai_move.state.wantsProne = false;
		}

		// Check for changes and then delay them
		if ( prevCrouch != ai_move.state.wantsCrouch || prevProne != ai_move.state.wantsProne ) {
			if ( ai_move.wants_reset_timer <= 0.01f ) {
				// Leave the changes alone
			}
			else {
				ai_move.wants_reset_timer -= Time::deltaTime;
				// Revert the changes
				ai_move.state.wantsCrouch	= prevCrouch;
				ai_move.state.wantsProne	= prevProne;
			}
		}
		// ==
	}
}
/*
Vector3d	NPC::AIState::GetMoveTarget ( void )
{
	return ai_move.final_target;
}*/

NPC::ai_move_feedback_state_t& NPC::AIState::GetMoveFeedback ( void )
{
	return ai_move.state;
}
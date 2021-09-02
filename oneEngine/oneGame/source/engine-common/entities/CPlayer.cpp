#include "CPlayer.h"
#include "core-ext/input/CInputControl.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/state/RrRenderer.h"

#include "engine/audio/AudioInterface.h"

// ==Constructor and Destructor==
CPlayer::CPlayer ( void )
	: CActor ()
{
	// ==Input==
	// Create input controller
	input = new CInputControl( this );
	input->Capture();
	// Set input to default values
	bCanMove		= true;
	bOverrideInput	= false;
	bHasInput		= true;
	bCanMouseMove	= true;

	// ===Camera===
	// Grab the camera from the output.
	pCamera = RrRenderer::Active->GetOutput<0>().camera; 
	if (pCamera == nullptr)
	{
		pCamera	= new RrCamera (false);
		bOwnCamera = true;
	}
	//m_cameraUpdateType	= (stateFunc_t)&CPlayer::camDefault;

	fTurnSensitivity	= 1;

	// ===Audio===
	pListener = engine::Audio.CreateListener();
}
CPlayer::~CPlayer ( void )
{
	if (bOwnCamera)
	{
		delete_safe(pCamera);
	}

	delete_safe(input);

	pListener->Destroy();
}


// ==Step Functions==
// Update. Grabs new input, updates the camera, and then 
void CPlayer::LateUpdate ( void )
{
	GrabInput();
	
	// Update the camera based on the input
	//(this->*m_cameraUpdateType)();
	camDefault();
	mvtPhaseFlying();

	// Update listener position
	pListener->velocity = pCamera->transform.position - pListener->position;
	pListener->position = pCamera->transform.position;
	pListener->orient_forward = pCamera->transform.rotation * Vector3f::forward;
	pListener->orient_up = pCamera->transform.rotation * Vector3f(0,0,1);
}

//LateUpdate for updating the current movement
void CPlayer::Update ( void ) 
{
	// Update base class code first
	CActor::Update();

	//m_moveType = *((stateFunc_t*)( (this->*m_moveType)() ));
	// Call the movement type
}


// ==Input==
// Receive all input and save in the player object
void CPlayer::GrabInput ( void )
{
	// If there's input, grab it.
	input->Update( this, Time::deltaTime );
	if ( !bHasInput )
	{
		// If there's no user input and there's no override present
		// Then reset the object's input
		if ( !bOverrideInput )
		{
			//input->ZeroValues( this );
			vDirInput	= Vector3f( 0,0,0 );
			vTurnInput	= Vector3f( 0,0,0 );

			input->axes.prone.Value				= input->axes.prone.PreviousValue;
			input->axes.prone.PreviousValue		= 0.0f;

			input->axes.crouch.Value			= input->axes.crouch.PreviousValue;
			input->axes.crouch.PreviousValue	= 0.0f;

			//input->axes.jump.Value				= input->axes.jump.PreviousValue;
			input->axes.jump.Value				= 0.0f;
			input->axes.jump.PreviousValue		= 0.0f;

			input->axes.sprint.Value			= input->axes.sprint.PreviousValue;
			input->axes.sprint.PreviousValue	= 0.0f;

			//input->Update( this );
		}
		input->axes.primary.Value			= 0.0f;
		input->axes.primary.PreviousValue	= 0.0f;

		input->axes.secondary.Value			= 0.0f;
		input->axes.secondary.PreviousValue	= 0.0f;

		input->axes.use.Value				= 0.0f;
		input->axes.use.PreviousValue		= 0.0f;

		input->axes.defend.Value				= 0.0f;
		input->axes.defend.PreviousValue		= 0.0f;

		input->axes.tertiary.Value				= 0.0f;
		input->axes.tertiary.PreviousValue		= 0.0f;

		input->axes.tglCompanion.Value			= 0.0f;
		input->axes.tglCompanion.PreviousValue	= 0.0f;
	}
	else
	{
		vDirInput	= input->vDirInput;
		if ( bCanMouseMove ) {
			vTurnInput	+= input->vMouseInput;
		}
		else {
			vTurnInput	= Vector3f( 0,0,0 );
		}
	}
}



// ==Camera States==
// Default camera update state
void*	CPlayer::camDefault ( void )
{
	// Set the position of the view
	pCamera->transform.position = transform.world.position;

	// Do mouse look
	vCameraRotation.z = vPlayerRotation.z;
	vCameraRotation.y -= vTurnInput.y;
	vTurnInput.y = 0;

	// Limit vertical mouselook angle
	if ( vCameraRotation.y > 95 )
		vCameraRotation.y = 95;
	if ( vCameraRotation.y < -95 )
		vCameraRotation.y = -95;

	// Set the final camera rotation and view angle
	pCamera->transform.rotation.Euler( vCameraRotation );
	pCamera->fieldOfView = m_cameraFieldOfView;

	return NULL;
}
// == Phase Movetype ==
// Phase flying. For the win.
void*	CPlayer::mvtPhaseFlying ( void )
{
	// Turn first
	vPlayerRotation.z += vTurnInput.x;
	vTurnInput.x = 0.0f;

	Vector3f moveVector(vDirInput.y, -vDirInput.x, 0.0F);

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( pCamera->transform.rotation );
	moveVector = rotMatrix*moveVector;

	if ( input->axes.jump ) {
		moveVector.z += 6.0F;
	}
	if ( input->axes.crouch ) {
		moveVector.z -= 6.0F;
	}

	if ( input->axes.sprint ) { 
		transform.world.position += moveVector * Time::deltaTime * 30.0f;
	}
	else {
		transform.world.position += moveVector * Time::deltaTime * 6.0f;
	}

	return NULL;
}

/*
// Needed misc includes
#include "CInput.h"
#include "CInputControl.h"
#include "Time.h"
#include "Raytracer.h"
#include "CPlayerInventory.h"

// Include audio
#include "CAudioInterface.h"
#include "CAudioListener.h"


CPlayer* CPlayer::m_activePlayer = NULL;

// ==Constructor and Destructor==
CPlayer::CPlayer ( void )
	: CCharacter ()
{
	m_activePlayer = this;

	// ==Input==
	// Create input controller
	input = new CInputControl( this );
	input->Capture();
	// Set input to default values
	bCanMove		= true;
	bOverrideInput	= false;
	bHasInput		= true;
	bCanMouseMove	= true;

	// ===Camera===
	m_cameraUpdateType	= (stateFunc_t)&CPlayer::camDefault;
	pCamera	= new RrCamera ();
	pCamera->SetActive();
	
	vPlayerRotation		= Vector3f( 0,0,0 );
	vCameraRotation		= Vector3f( 0,0,0 );
	vCameraAnimOffsetRotation = Vector3f( 0,0,0 );
	vViewBob			= Vector3f( 0,0,0 );
	fViewBobTimer		= 0;
	fViewBobSpeed		= 0;
	fViewBobAmount		= 0;
	fViewBobTarget		= 0;
	fViewAngle			= 100;
	fViewAngleOffset	= 0;
	fViewAnglePOffset	= 0;
	fViewAnglePTarget	= 0;
	fViewRollOffset		= 0;
	fViewRollTarget		= 0;
	vViewPunch			= Vector3f( 0,0,0 );
	vViewPunchVelocity	= Vector3f( 0,0,0 );

	fTurnSensitivity	= 1;

	// ===Audio===
	pListener = Audio.CreateListener();

	// All inventory related items need to go to CMccPlayer
	// ===Inventory===
	//pMyInventory	= new CInventory( this );

	pCurrentLookedAt	= NULL;
	fMaxUseDistance	= 4.0f;

}
CPlayer::~CPlayer ( void )
{
	if ( m_activePlayer == this ) {
		m_activePlayer = NULL;
	}

	//FreeMovement();
	delete pCamera;
	//delete pMyLight;

	delete input;
	input = NULL;
}

// ==Step Functions==
// Update. Grabs new input, updates the camera, and then 
void CPlayer::LateUpdate ( void )
{
	GrabInput();
	vTurnInput *= ((fViewAngle+fViewAngleOffset+5)/90.0f) * fTurnSensitivity; //CGameSettings::Active()->f_cl_MouseSensitivity;

	// Perform player actions
	DoPlayerActions();
	// Update light position
	//pMyLight->diffuseColor = Color( 1.0f,1.0f,1.0f,1.0f );
	//pMyLight->transform.position = pCamera->transform.position;

	// Update the camera based on the input
	(this->*m_cameraUpdateType)();

	// Update listener position
	pListener->velocity = pCamera->transform.position - pListener->position;
	pListener->position = pCamera->transform.position;
	pListener->orient_forward = pCamera->transform.Forward();
	pListener->orient_up = pCamera->transform.Up();
}

void CPlayer::PostUpdate ( void )
{
	//(this->*m_cameraUpdateType)();
}

//LateUpdate for updating the current movement
void CPlayer::Update ( void ) 
{
	// Update base class code first
	CCharacter::Update();

	//m_moveType = *((stateFunc_t*)( (this->*m_moveType)() )); // MOVE THIS TO FIXEDUPDATE when FIXEDUPDATE is FIXED
	// UPDATE: IT IS ON LATEUPDATE BECAUSE OF THE WONKEY MOUSE-CONTROL, *NOT* BECAUSE FIXEDUPDATE IS BROKEN
	// Call the movement type
}

void CPlayer::FixedUpdate ( void )
{
	//mvtPhaseFlying();
}

#include "RrDebugDrawer.h"


#include "COctreeTerrain.h"
#include "COctreeMesher.h"
#include "Math.h"
// ==Player Actions==
// Do player actions.
// For example, modifying the inventory, pausing the game, and other non-movement related actions should be done here.
void CPlayer::DoPlayerActions ( void )
{
	// Calling the lookAt functions
	DoLookAtCommands();
	
	if ( core::Input::Keydown( 'B' ) )
	{
		if ( rhLookAtResult.hit ) {
			rhLookAtResult.hitPos.x = (Real)Math.Round( rhLookAtResult.hitPos.x );
			rhLookAtResult.hitPos.y = (Real)Math.Round( rhLookAtResult.hitPos.y );
			rhLookAtResult.hitPos.z = (Real)Math.Round( rhLookAtResult.hitPos.z );
			COctreeTerrain::GetActive()->CSG_AddBlock( rhLookAtResult.hitPos, Vector3f(1,1,1)*4 );
			COctreeTerrain::GetActive()->TerrainUpdate();
		}
	}
	if ( core::Input::Keydown( 'N' ) )
	{
		if ( rhLookAtResult.hit ) {
			rhLookAtResult.hitPos.x = (Real)Math.Round( rhLookAtResult.hitPos.x );
			rhLookAtResult.hitPos.y = (Real)Math.Round( rhLookAtResult.hitPos.y );
			rhLookAtResult.hitPos.z = (Real)Math.Round( rhLookAtResult.hitPos.z );
			COctreeTerrain::GetActive()->CSG_AddBlock( rhLookAtResult.hitPos, Vector3f(1,1,1) );
			COctreeTerrain::GetActive()->TerrainUpdate();
		}
	}
	if ( core::Input::Keydown( 'X' ) )
	{
		if ( rhLookAtResult.hit ) {
			rhLookAtResult.hitPos.x = (Real)Math.Round( rhLookAtResult.hitPos.x );
			rhLookAtResult.hitPos.y = (Real)Math.Round( rhLookAtResult.hitPos.y );
			rhLookAtResult.hitPos.z = (Real)Math.Round( rhLookAtResult.hitPos.z );
			COctreeTerrain::GetActive()->CSG_RemoveSphere( rhLookAtResult.hitPos, 4 );
			COctreeTerrain::GetActive()->TerrainUpdate();
		}
	}
	if ( core::Input::Keydown( 'L' ) )
	{
		COctreeMesher::m_useDualContouring = !COctreeMesher::m_useDualContouring;
		COctreeTerrain::GetActive()->TerrainUpdate();
	}
}

#include "CRagdollCollision.h"
void CPlayer::DoLookAtCommands ( void )
{
	CGameBehavior* pHitObject = NULL;

	// Raycast from eye position
	Ray viewRay = GetEyeRay();

	//RaycastHit result; //rhLookAtResult;
	BlockTrackInfo block;

	// Raycasting
	Raytracer.Raycast( viewRay, pCamera->zFar, &rhLookAtResult, &block, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), this );
	RaycastHit result = rhLookAtResult;
	// Do the raycast
	Vector3f weightedDistance = rhLookAtResult.hitPos - viewRay.pos;
	weightedDistance.z *= 0.5f;
	//if ( Raytracer.Raycast( viewRay, fMaxUseDistance, &result, &block, 1|2|4 ) )
	if (( rhLookAtResult.hit )&&( rhLookAtResult.distance <= fMaxUseDistance || weightedDistance.magnitude() <= fMaxUseDistance ))
	{
		pHitObject = result.pHitBehavior;
		// Get parent object if the object is a rigidbody
		if ( (pHitObject->layer & Layers::Rigidbody) != 0 ) {
			pHitObject = ((CRigidbody*)result.pHitBehavior)->GetOwner();
		}
		else if ( (pHitObject->layer & Layers::Hitboxes) != 0 ) {
			pHitObject = ((CRagdollCollision*)result.pHitBehavior)->GetActor();
		}

		// Depending on the layer of the hit object, call the lookAt functions
		if ( pHitObject && (pHitObject->layer & Layers::WeaponItem) != 0 ) // Looking at a weaponItem
		{
			if ( pCurrentLookedAt != pHitObject )
			{
				// Set old looked at
				if (( pCurrentLookedAt != NULL )&&( CGameState::pActive->ObjectExists( pCurrentLookedAt ) ))
				{
					// Perform the proper routine
					if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
						((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
					else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
						((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
				}
				// Set new looked at
				pCurrentLookedAt = pHitObject;
				// Perform the proper routine
				((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAt( this );
			}
		}
		else if ( pHitObject && (pHitObject->layer & Layers::Actor) != 0 ) // Looking at an actor
		{
			if ( pCurrentLookedAt != pHitObject )
			{
				// Set old looked at
				if (( pCurrentLookedAt != NULL )&&( CGameState::pActive->ObjectExists( pCurrentLookedAt ) ))
				{
					// Perform the proper routine
					if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
						((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
					else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
						((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
				}
				// Set new looked at
				pCurrentLookedAt = pHitObject;
				// Perform the proper routine
				((CActor*)pCurrentLookedAt)->OnInteractLookAt( this );
			}
		}
		else if ( pCurrentLookedAt != pHitObject ) // Not looking at anything
		{
			// Set old looked at
			if (( pCurrentLookedAt != NULL )&&( CGameState::pActive->ObjectExists( pCurrentLookedAt ) ))
			{
				// Perform the proper routine
				if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
					((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
				else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
					((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
			}
			// Set new looked at
			pCurrentLookedAt = NULL;
		}
	}
	else // Use cast didn't even touch
	{
		// Set old looked at
		if (( pCurrentLookedAt != NULL )&&( CGameState::pActive->ObjectExists( pCurrentLookedAt ) ))
		{
			// Perform the proper routine
			if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
				((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
			else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
				((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
		}
		// Set new looked at
		pCurrentLookedAt = NULL;
	}

	// Set camera's focal distance based on the LookAt result
	Real targetFocalDistance = pCamera->focalDistance;
	if ( rhLookAtResult.hit ) {
		targetFocalDistance = rhLookAtResult.distance;
	}
	else {
		targetFocalDistance = pCamera->zFar * 0.5f;
	}
	pCamera->focalDistance += ( targetFocalDistance - pCamera->focalDistance ) * Time::TrainerFactor( 0.3f );
}

// ==Input==
// Receive all input and save in the player object
void CPlayer::GrabInput ( void )
{
	// If there's input, grab it.
	input->Update( this );
	if ( !bHasInput )
	{
		// If there's no user input and there's no override present
		// Then reset the object's input
		if ( !bOverrideInput )
		{
			//input->ZeroValues( this );
			vDirInput	= Vector3f( 0,0,0 );
			vTurnInput	= Vector3f( 0,0,0 );

			input->axes.prone.Value				= input->axes.prone.PreviousValue;
			input->axes.prone.PreviousValue		= 0.0f;

			input->axes.crouch.Value			= input->axes.crouch.PreviousValue;
			input->axes.crouch.PreviousValue	= 0.0f;

			//input->axes.jump.Value				= input->axes.jump.PreviousValue;
			input->axes.jump.Value				= 0.0f;
			input->axes.jump.PreviousValue		= 0.0f;

			input->axes.sprint.Value			= input->axes.sprint.PreviousValue;
			input->axes.sprint.PreviousValue	= 0.0f;

			//input->Update( this );
		}
		input->axes.primary.Value			= 0.0f;
		input->axes.primary.PreviousValue	= 0.0f;

		input->axes.secondary.Value			= 0.0f;
		input->axes.secondary.PreviousValue	= 0.0f;

		input->axes.use.Value				= 0.0f;
		input->axes.use.PreviousValue		= 0.0f;

		input->axes.defend.Value				= 0.0f;
		input->axes.defend.PreviousValue		= 0.0f;

		input->axes.tertiary.Value				= 0.0f;
		input->axes.tertiary.PreviousValue		= 0.0f;

		input->axes.tglCompanion.Value			= 0.0f;
		input->axes.tglCompanion.PreviousValue	= 0.0f;
	}
	else
	{
		vDirInput	= input->vDirInput;
		if ( bCanMouseMove ) {
			vTurnInput	+= input->vMouseInput;
		}
		else {
			vTurnInput	= Vector3f( 0,0,0 );
		}
	}
}

*/
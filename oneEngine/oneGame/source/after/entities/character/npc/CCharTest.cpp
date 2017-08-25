
#include "CCharTest.h"

#include "core-ext/animation/CAnimation.h"

#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/debug/CDebugDrawer.h"

#include "after/entities/character/CMccCharacterModel.h"
#include "after/types/terrain/BlockType.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/edit/CTerrainAccessor.h"

using namespace NPC;

REGISTER_ZCC(CCharTest);

CCharTest::CONSTRUCTOR_ZCC_V2(CCharTest)
{	ZCC_AddInstance();

	pCollision = new CCapsuleCollider ( 5.7f, 0.7f );
	pRigidBody = NULL;

	// Create Racial stats for a Level 1 Dark Elf
	m_stats = new CRacialStats();
	m_stats->SetDefaults();
	m_stats->iRace = CRACE_KITTEN;
	m_stats->SetLuaDefaults();
	// Give the new stats our own stats (TODO: constructor for CRacialStats to take stats)
	delete m_stats->stats;
	m_stats->stats = &stats;
	m_stats->pOwner = this;

	// Set the name to wanderer
	strcpy( charName, "Wanderer" );
	m_stats->sPlayerName = charName;

	// Character Model
	pCharModel = new CMccCharacterModel( this );
	pCharModel->LoadBase( "clara" );

	CMccCharacterModel* mccModel = (CMccCharacterModel*)pCharModel;
	mccModel->SetVisualsFromStats( m_stats );

	// Initialize the LookAt AI
	pLookAtCharacter = NULL;
	bLookingAtCharacter = false;
	bTalkingAtCharacter = false;

	fAttentionDistance = 14.0f;

	vFacingPos = transform.position + transform.rotation * Vector3d::forward * 24 + Vector3d( 0,0,3 );
	vLookatPos = vFacingPos;

	fGlanceAwayTimer = 0;
	vGlanceAwayAngle = Vector3d::zero;
	vCurrentGlanceAwayAngle = Vector3d::zero;
}

CCharTest::~CCharTest ( void )
{

	delete pRigidBody;
	delete pCollision;

	delete pModel;
}

void CCharTest::OnInteract ( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		pLookAtCharacter = (CCharacter*)interactingActor;

		Ray eyeRay = pLookAtCharacter->GetEyeRay();

		vFacingPos = eyeRay.pos;	// Turn and look at the interactor (THAT'S A PUN GET IT interACTOR)
		vLookatPos = vFacingPos;

		fGlanceAwayTimer = 5.0f;	// Don't glance away from character for a time
		vGlanceAwayAngle = Vector3d::zero;

		fLookAwayTimer = 3.0f; // And please please turn to face the player

		bTalkingAtCharacter = true;
	}
}

void CCharTest::UpdateActive ( void )
{
	if ( !pRigidBody )
	{
		pRigidBody = new CRigidBody ( pCollision, this, 30 );
		pRigidBody->SetRotationEnabled( false );
		pRigidBody->SetQualityType( HK_COLLIDABLE_QUALITY_CHARACTER );
		pRigidBody->SetRestitution( 0.01f );
		pRigidBody->SetCollisionLayer( Layers::PHYS_CHARACTER, 1 );
	}
	if ( pModel ) {
		pModel->transform.Get( this->transform );
		pModel->GetAnimation()->Play( "Idle Short" );
	}
	if ( pCharModel ) {
		//pCharModel->SetMoveAnimation( "walk" );
		pCharModel->SetAnimationMotion( pRigidBody->GetVelocity() );

		if ( !pLookAtCharacter ) {
			pLookAtCharacter = (CCharacter*)this->currentActivePlayer;
		}

		//bool bCharacterInView = false;
		bCharacterInView = false;
		bCharacterInRange = false;
		if ( pLookAtCharacter ) {
			Ray eyeRay = GetEyeRay();
			Ray targetEyeRay = pLookAtCharacter->GetEyeRay();
			if ( (targetEyeRay.pos-eyeRay.pos).sqrMagnitude() < sqr(fAttentionDistance) ) {
				bCharacterInRange = true;
				if ( eyeRay.dir.dot( (targetEyeRay.pos-eyeRay.pos).normal() ) > 0.25f ) {
					bCharacterInView = true;
				}
			}
			//DebugD::DrawLine( eyeRay.pos, eyeRay.pos + eyeRay.dir );
		}
		
		if ( !bCharacterInRange ) {	// Seriously, if the character is far enough
			bTalkingAtCharacter = false;	// THEN DON'T GIVE A FUCCCKKKKK
		}

		// look at follow code
		if ( bLookingAtCharacter ) {
			if ( pLookAtCharacter ) {
				vLookatPos = pLookAtCharacter->GetEyeRay().pos;
			}

			// Check if person not in view
			if ( !bCharacterInView ) {
				if ( bTalkingAtCharacter ) {
					// Turn to face the character
					if ( pLookAtCharacter ) {
						vFacingPos = pLookAtCharacter->GetEyeRay().pos;
					}
				}
				else {
					bLookingAtCharacter = false;
					fLookAwayTimer = 2.0f;
				}
			}
		}
		else {
			// Look at our things
			Ray eyeRay = GetEyeRay();
			// Look back forwards when time has passed
			fLookAwayTimer -= Time::deltaTime;
			if ( fLookAwayTimer <= 0 ) {
				// Face forward
				vFacingPos = transform.position + transform.rotation * Vector3d::forward * 24 + Vector3d( 0,0,3 );
				vLookatPos = eyeRay.pos + eyeRay.dir*8;
			}

			// Look at character 
			if ( bCharacterInView ) {
				fGlanceAwayTimer = 5.0f;	// Don't glance away from character for a time
				vGlanceAwayAngle = Vector3d::zero;
				bLookingAtCharacter = true;
			}
		}
		
		// Do the glance effect
		fGlanceAwayTimer -= Time::deltaTime;
		if ( fGlanceAwayTimer < 0 ) {
			vGlanceAwayAngle += Vector3d( 0,random_range(-20.0f,20.0f),random_range(-20.0f,20.0f) );
			if ( vGlanceAwayAngle.magnitude() > 40 ) {
				vGlanceAwayAngle = vGlanceAwayAngle.normal() * 40;
			}
			if ( vGlanceAwayAngle.z < -10 ) {
				vGlanceAwayAngle.z = -10;
			}
			if ( vGlanceAwayAngle.z > 20 ) {
				vGlanceAwayAngle.z = 20;
			}
			fGlanceAwayTimer = random_range( 0.2f, 3.0f );
		}
		else {
			vGlanceAwayAngle += Vector3d( 0,random_range(-4.0f,4.0f),random_range(-4.0f,4.0f) ) * Time::deltaTime;
		}
		vCurrentGlanceAwayAngle = vCurrentGlanceAwayAngle.lerp( vGlanceAwayAngle, Time::TrainerFactor( 0.3f ) );

		// Set the model angles
		pCharModel->SetEyeRotation( Rotator( vCurrentGlanceAwayAngle ) );

		// Set body rotation
		{	// Set default rotation
			Ray eyeRay = GetEyeRay();
			Quaternion rot;
			//rot.RotationTo( eyeRay.dir, (vCurrentFacingPos-eyeRay.pos).normal() );
			//rot.RotationTo( eyeRay.dir, (vCurrentFacingPos-eyeRay.pos).normal() );
			//rot.RotationTo( transform.rotation * Vector3d::left, (vCurrentFacingPos-eyeRay.pos).normal() );
			//rot.RotationTo( transform.rotation * Vector3d::left, (vCurrentFacingPos-eyeRay.pos).normal() );
			//rot.RotationTo( transform.rotation * Vector3d::left, (vCurrentFacingPos-transform.position).normal() );
			rot.RotationTo( Vector3d::forward, (vCurrentFacingPos-transform.position).normal() );
			//DebugD::DrawLine( eyeRay.pos, eyeRay.pos+eyeRay.dir );
			Vector3d euler = Rotator(rot).getEulerAngles();
			transform.rotation = Rotator( Vector3d( 0,0,-euler.z ) );
			//transform.SetDirty();
		}
		//DebugD::DrawLine( transform.position, transform.position + transform.rotation * Vector3d::forward * 2 );
		vCurrentFacingPos = vCurrentFacingPos.lerp( vFacingPos, Time::TrainerFactor( 0.08f ) ) + ((vCurrentFacingPos+vFacingPos)*0.5f - transform.position).normal()*0.4f;//+ (transform.rotation * Vector3d::forward);
		pCharModel->SetFaceAtPosition( vCurrentFacingPos );

		// Then head rotation
		vCurrentLookatPos = vCurrentLookatPos.lerp( vLookatPos, Time::TrainerFactor( 0.15f ) );
		pCharModel->SetLookAtPosition( vCurrentLookatPos );

		// Then eyes last
	}

	MoveUnstuck();
}
void CCharTest::UpdateInactive ( void )
{
	if ( pRigidBody )
	{
		delete pRigidBody;
		pRigidBody = NULL;
	}
}


//#include "unused/CVoxelTerrain.h"
// Move Unstuck
void CCharTest::MoveUnstuck ( void )
{
	ushort block = TerrainAccess.GetBlockAtPosition(transform.position).block;
	if (( block == Terrain::EB_NONE )||( block == Terrain::EB_WATER ))
	{

	}
	else
	{
		//transform.position += Vector3d( 0,0,2 );
		transform.position.z += 2 - fmodf( transform.position.z, 2 );
		if ( pRigidBody ) {
			transform.SetDirty();
			pRigidBody->SetVelocity( pRigidBody->GetVelocity() * 0.5f );
		}
	}
}


Ray		CCharTest::GetEyeRay ( void )
{
	Ray newRay;
	XTransform transCamPos;
	pCharModel->GetEyecamTransform( transCamPos );

	newRay.pos = transCamPos.position;
	//newRay.dir = transCamPos.rotation * Vector3d::left;
	newRay.dir = (vCurrentFacingPos-transCamPos.position).normal();

	return newRay;
};

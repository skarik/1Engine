
#include "FaunaDesertBird.h"

#include "core/time/time.h"

#include "engine-common/entities/CPlayer.h"

#include "renderer/logic/model/CModel.h"

#include "after/types/terrain/TerrainTypes.h"

using namespace NPC;

REGISTER_ZCC(FaunaDesertBird);

FaunaDesertBird::CONSTRUCTOR_ZCC_V2(FaunaDesertBird)
{	ZCC_AddInstance();
	pModel = new CModel ( "models/fauna/bird_beta.FBX" );
}

FaunaDesertBird::~FaunaDesertBird ( void )
{
	if ( pModel )
		delete pModel;
	pModel = NULL;
}

void FaunaDesertBird::UpdateMovement ( void )
{
	if ( this->terraType == Terrain::TER_DESERT ) {
		targetPosition = currentActivePlayer->transform.position;
		targetPosition += Vector3d(
			sin( Time::currentTime*0.5f + GetId()*0.9f )*20*(sin( Time::currentTime*0.06f - GetId()*0.9f )+2.0f),
			cos( Time::currentTime*0.5f + GetId()*0.9f )*20*(sin( Time::currentTime*0.06f - GetId()*0.9f )+2.0f),
			90+sin( Time::currentTime*0.3f + GetId() )*15 );
	}
	else {
		/*targetPosition = currentActivePlayer->transform.position + targetVelocity;
		targetPosition += Vector3d( 
			sin( Time::currentTime*0.5f + GetId()*0.9f )*20*(sin( Time::currentTime*0.06f - GetId()*0.9f )+2.0f),
			cos( Time::currentTime*0.5f + GetId()*0.9f )*20*(sin( Time::currentTime*0.06f - GetId()*0.9f )+2.0f),
			90+sin( Time::currentTime*0.3f + GetId() )*15 );*/
		targetPosition += Vector3d(
			velocity.x + random_range( -2.0f, 2.0f ),
			velocity.y + random_range( -2.0f, 2.0f ),
			velocity.z + random_range( -1.0f, 1.0f ) ) * Time::deltaTime * 5.0f;
		targetPosition += Vector3d( sin( GetId()*0.9f ), cos( GetId()*0.9f ), 0 ) * 16.0f * Time::deltaTime;
	}

	Vector3d targetVelocity;
	targetVelocity = targetPosition - transform.position;
	if ( targetVelocity.magnitude() > 6 ) {
		targetVelocity = targetVelocity.normal() * 6;
	}
	acceleration = ( targetVelocity - velocity ) * 0.1f;
	if ( acceleration.magnitude() < 0.4f ) {
		acceleration = acceleration.normal() * 0.4f;
	}
	if ( acceleration.magnitude() > 2.7f ) {
		acceleration = acceleration.normal() * 2.7f;
	}
	velocity += acceleration * Time::deltaTime;


	Quaternion moveDir;
	moveDir.RotationTo( Vector3d( 0,1,0 ), velocity.normal() );
	transform.rotation = Rotator( moveDir );
	transform.position += velocity;

	if ( pModel )
		pModel->transform.Get( this->transform );
}

void FaunaDesertBird::UpdateActive ( void )
{
	UpdateMovement();
}
void FaunaDesertBird::UpdateInactive ( void )
{
	UpdateMovement();
	/*if ( pRigidBody )
	{
		delete pRigidBody;
		pRigidBody = NULL;
	}*/
}
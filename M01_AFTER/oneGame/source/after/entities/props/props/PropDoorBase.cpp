
#include "PropDoorBase.h"
#include "core/math/Math.h"
#include "after/entities/character/CCharacter.h"

// Constructor
PropDoorBase::PropDoorBase ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	m_model = new CModel( "models/props/door0frame.FBX" );
	//pModel->GetAnimation()->Stop("open");

	mCollidesWithPlayer = true;
	mUseConvexCollision = false;

	/*mLastHitTime = 0.0f;
	mHealth = 35.0f;

	mInventory = new CInventory( this, 4,6 ); */

	//strcpy( charName, "Wooden Chest" );

	doorModel = new CModel( "models/props/door0.FBX" );
	doorCollision = new CBoxCollider( doorModel->GetBoundingBox(), Vector3d(0,0,0) );
	doorBody = new CRigidBody( doorCollision, this );

	// Change the body to kinematic
	doorBody->SetMotionType( physMotion::MOTION_KEYFRAMED );
	// Set collision 
	doorBody->SetCollisionLayer( Layers::PHYS_DYNAMIC );
	// Set body's target to the model transform
	doorBody->SetTargetTransform( &doorModel->transform );

	// Mark positions as dirty to force physics side update
	transform.SetDirty();
	doorModel->transform.SetDirty();

	// Set initial angles
	mDoorAngle = 0;
	mDoorAngularVelocity = 0;

}

// Destructor
PropDoorBase::~PropDoorBase ( void )
{
	delete doorBody;
	delete doorCollision;
	delete doorModel;
}


// Update performs opening and closing fun times
void PropDoorBase::Update ( void )
{
	CTerrainProp::Update();

	// Slow door movement down
	ftype tDoorAcceleration = Time::deltaTime * Math.sgn( mDoorAngularVelocity ) * 45;
	if ( fabs(mDoorAngularVelocity) > fabs(tDoorAcceleration) ) {
		mDoorAngularVelocity -= tDoorAcceleration;
	}
	else {
		mDoorAngularVelocity = 0;
	}

	ftype tDoorAngularVelocity = mDoorAngularVelocity * Time::deltaTime;

	// Close door
	if ( fabs(tDoorAngularVelocity + mDoorAngle) < fabs(tDoorAngularVelocity) )
	{
		mDoorAngularVelocity = 0;
		mDoorAngle = 0;
	}
	// Open door
	if ( fabs(tDoorAngularVelocity + mDoorAngle) > 100 )
	{
		mDoorAngle = Math.sgn( mDoorAngle ) * 100.0f;
		mDoorAngularVelocity = -mDoorAngularVelocity * 0.4f;
	}
	// Move door
	mDoorAngle += mDoorAngularVelocity * Time::deltaTime; // doesn't use tDoorAngularVelocity in case velocity changed from the checks earlier

	// Set door rotation and position
	doorModel->transform.rotation = Quaternion::CreateAxisAngle(Vector3d::up,mDoorAngle) * transform.rotation;
	doorModel->transform.position = transform.position + transform.rotation*(Vector3d(-1.8f,0,0) + Quaternion::CreateAxisAngle(Vector3d::up,mDoorAngle)*Vector3d(1.8f,0,3.3f));
}


// Terrain placement. Override the set scale and rotation
void PropDoorBase::OnTerrainPlacement ( void )
{
	//transform.scale = Vector3d( 1.5f,1.5f,1.5f ) * Random.Range( 0.9f, 1.1f );
	//transform.rotation = Vector3d( Random.Range(-15.0f,15.0f),Random.Range(-15.0f,15.0f),Random.Range(0,360.0f) );
	// Mark position as dirty to force physics side update
	transform.SetDirty();
}

// Interact function
void PropDoorBase::OnInteract ( CActor* interactingActor )
{
	/*if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		switch ( Random.Next()%4 ) {
			case 0: cout << "OH YES, USE ME!" << endl;
				break;
			case 1: cout << "Wooden CHEST" << endl;
				break;
			case 2: cout << "Sexy Chest of Unknowing" << endl;
				break;
			case 3: cout << "Box box box box box" << endl;
				break;
		}

		((CMccPlayer*)interactingActor)->OpenInventoryForSwapping(this->mInventory);
	}*/
	/*if ( mDoorAngularVelocity > 1 ) {
		mDoorAngularVelocity = 0;
	}
	else {
		mDoorAngularVelocity = 35;
	}*/
	Vector3d inputMotion;
	if ( interactingActor->layer & Layers::Character )
	{
		inputMotion = ((CCharacter*)(interactingActor))->GetMotionVelocity();
		inputMotion.z = 0;
	}

	if ( mDoorAngle > 10 ) {
		mDoorAngularVelocity = -130;
	}
	else if ( mDoorAngle < -10 ) {
		mDoorAngularVelocity = 130;
	}
	else {
		Vector3d playerDirpos = doorModel->transform.position - interactingActor->transform.position;
		if ( (Quaternion::CreateAxisAngle(Vector3d::up,mDoorAngle) * Vector3d::left).dot( playerDirpos.normal() ) > 0 ) {
			mDoorAngularVelocity = 100 + inputMotion.sqrMagnitude() * 0.7f; //+ interactingActor->Get;
		}
		else {
			mDoorAngularVelocity = -100 - inputMotion.sqrMagnitude() * 0.7f;
		}
	}
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void PropDoorBase::OnPunched ( RaycastHit const& hitInfo )
{
	/*ItemFloraSnowLily* pNewItem = new ItemFloraSnowLily();
	pNewItem->transform.Get( transform );

	DeleteObject( this );*/
	//pModel->GetAnimation()->Play("open");
	//if ( true )//mInventory->IsEmpty() )
	/*if ( mInventory->IsEmpty() )
	{
		mLastHitTime = 2.0f;
		mHealth -= 10.0f;
		if ( mHealth < 0.0f ) {
			DeleteObject( this );
		}
	}*/
}
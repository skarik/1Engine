

#include "CWeaponItem.h"

// Prototyped classes
#include "engine-common/entities/CActor.h"
#include "after/entities/character/CCharacter.h"
//#include "../CCharacter.h"

#include "core/time/time.h"
//#include "../CBoxCollider.h"
#include "engine/physics/collider/types/CBoxCollider.h"

//#include "../CVoxelTerrain.h"	// for CBoob
//#include "../COctreeTerrain.h"
//#include "../Zones.h"	// for utility

#include "engine/physics/motion/CRigidbody.h"
#include "renderer/logic/model/CModel.h"

#include "after/terrain/Zones.h"

using std::cout;
using std::endl;

// = Static Variables =
//bool CWeaponItem::bCheckCurrentSector = false;

// Constructor for initializing default values
CWeaponItem::CWeaponItem ( const WItemData & newData )
	: CItemBase(), pOwner( NULL )//, pCurrentSector( NULL )
{
	//vCurrentSector = RangeVector( 0,0,0 );

	layer = Layers::WeaponItem;

	for ( uint i = 0; i < 9; ++i ) {
		fCooldown[i] = 0;
	}

	iBeltIndex = 0;

	mHand = 0;

	fCollisionTimer = 0;

	pBody		= NULL;
	pCollider	= NULL;
	pModel		= NULL;

	weaponItemData = newData;
	weaponItemState.iCurrentStack = 1;
	weaponItemState.sItemName = weaponItemData.sInfo;
	// Check factory for a valid index
	/*short targetId = WeaponItem::WeaponItemFactory.witem_hash[typeid(*this).name()];
	if ( weaponItemData.iItemId != targetId ) {
		cout << "Mismatch item ID's: " << weaponItemData.iItemId << " not match table " << targetId << endl;
		if ( targetId != 0 ) {
			cout << "  setting item to ID " << targetId << endl;
			weaponItemData.iItemId = targetId;
		}
		else {
			// TODO: idea, put on update or on factory callback
			//Debug::Console->PrintError( "Could not find item ID in the table! (Did you forget to add it to the list?)\n" );
		}
	}*/
	// Do a check for invalid item ID's
	if ( weaponItemData.iItemId == -1 ) {
		cout << "Bad item ID on new '" << GetTypeName() << "'. item info: " << weaponItemData.sInfo << endl;
	}

	weaponItemState.iCurrentDurability = weaponItemData.iMaxDurability;

	holdType = Item::Default;	// Default value for hold type

	SetOwner ( NULL );
	SetHoldState ( Item::None );

	bHasToss = false;
	vTossVector = Vector3d( 0,0,0 );

	vHoldingOffset = Vector3d( 0,0,0 );

	mAnimationIdleAction = NPC::ItemAnim::Idle;
	iAnimationSubset = 1;
	mAnimationIdleArgs = 0;

	holsterTargetHoldstate	= Item::Hidden;
	holsterTimer			= 0;
	holsterAway				= true;
	holsterInterrupted		= false;
	holsterActive			= false;

	holsterTime_Away		= 0.50f;
	holsterTime_Equip		= 0.25f;

	RemoveReference(); // TODO: Remove this nonsense!
}
// OnCreate for initializing item ID's to proper values
// This should be called to check for user errors.
void CWeaponItem::OnCreate ( void )
{
	short targetId = WeaponItem::WeaponItemFactory.witem_hash[typeid(*this).name()];
	if ( weaponItemData.iItemId != targetId ) {
		Debug::Console->PrintWarning( "Problem with item ID's!" );
		cout << "Mismatch item ID's: " << weaponItemData.iItemId << " not match table " << targetId << endl;
		if ( targetId != 0 ) {
			cout << "  setting item to ID " << targetId << endl;
			weaponItemData.iItemId = targetId;
		}
		else {
			// TODO: idea, put on update or on factory callback
			Debug::Console->PrintError( "Could not find item ID in the table! (Did you forget to add it to the list?)\n" );
		}
	}
	// Do a check for invalid item ID's
	if ( weaponItemData.iItemId == -1 ) {
		Debug::Console->PrintError( "Critical problem with item ID's!" );
		cout << "Still bad item ID on new '" << GetTypeName() << "'. item info: " << weaponItemData.sInfo << endl;
	}
}

// Copy value
CWeaponItem& CWeaponItem::operator= ( const CWeaponItem * original )
{
	holdType		= original->holdType;
	fCooldown[0]	= original->fCooldown[0];
	fCooldown[1]	= original->fCooldown[1];
	fCooldown[2]	= original->fCooldown[2];
	iBeltIndex		= original->iBeltIndex;
	fCollisionTimer	= original->fCollisionTimer;

	pOwner			= original->pOwner;
	holdState		= original->holdState;

	weaponItemState.iCurrentStack		= original->weaponItemState.iCurrentStack;
	weaponItemState.iCurrentDurability	= original->weaponItemState.iCurrentDurability;
	weaponItemState.sItemName			= original->weaponItemState.sItemName;

	return (*this);
}

// Initialize the physics object
void CWeaponItem::CreatePhysics ( void )
{
	if ( pModel == NULL )
	{
		cout << "Problem in CWeaponItem (" << GetItemName() << ") mem(" << (void*)(this) << ") " << endl;
		cout << "Cannot create a physics presence without a model attached!" << endl;
	}
	else
	{
		// Create a box collider based on the model's bounding box,
		//  but only create it if there's no collider already
		if ( pCollider == NULL )
			pCollider = new CBoxCollider( pModel->GetBoundingBox(), Vector3d(0,0,0) );

		// Create a rigidbody based on the collider
		pBody = new CRigidBody( pCollider, this, weaponItemData.fWeight );

		// Update the body's position
		pBody->SetPosition( transform.position );
		//pBody->SetRotation( transform.rotation );
		transform.SetDirty();

		// Change the body's friction
		pBody->SetFriction( 1.3f );

		// Change the body's motion type
		//pBody->SetQualityType( HK_COLLIDABLE_QUALITY_CRITICAL );
		//pBody->SetQualityType( HK_COLLIDABLE_QUALITY_DEBRIS );
		pBody->SetQualityType( HK_COLLIDABLE_QUALITY_MOVING );

		// Set the body's mass
		pBody->SetMass( weaponItemData.fWeight );

		// Change collision type to item
		pBody->SetCollisionLayer( Layers::PHYS_ITEM );

		// Wait, I think that's it
		// hmmm, this is too simple....
	}
}


ftype defrotx = 0, defroty = 0, defrotz = 0;
#include "core/input/CInput.h"
#include <fstream>
void GetRots ( void )
{
	if ( Input::Keydown( Keys.F8 ) )
	{
		std::ifstream file ( ".res-0/system/weaponitem_rots.txt" );
		file >> defrotx;
		file >> defroty;
		file >> defrotz;
	}
}

/*
	// Request a holstering motion
	void RequestHolster ( HoldState nNextHoldState = OnBelt );
	// Have we put this item away?
	bool IsHolstered ( void );
	// Are we in the process of putting this item away?
	bool IsHolstering ( void );
	// Take out the item
	void Draw ( HoldState nNextHoldState = Holding );
	*/


// CWeaponItem's LateUpdate function.
void CWeaponItem::LateUpdate ( void )
{
	//GetRots();
	//vHoldingOffset = Vector3d( defrotx,defroty,defrotz );

	// Work on the cooldowns
	for ( char i = 0; i < 9; i += 1 )
	{
		if ( fCooldown[i] >= -1.0f )
			fCooldown[i] -= Time::deltaTime;
	}

	// Work on the holstering
	if ( holsterActive )
	{
		if ( holsterAway )
		{
			// Work up to putting away
			holsterTimer += Time::deltaTime;
			if ( holsterTimer >= holsterTime_Away )
			{
				holsterActive = false;
				SetHoldState( holsterTargetHoldstate );
			}
		}
		else
		{
			// Work down to taking out
			holsterTimer -= Time::deltaTime;
			if ( holsterTimer <= 0.0f )
			{
				holsterActive = false;
				SetHoldState( holsterTargetHoldstate );
			}
		}
		holsterInterrupted = false; // Reset interrupt state
	}

	// Depending on the hold state, toggle the physics and collision
	// Also do checks for a non-existent owner actor
	switch ( holdState )
	{
		// This hold state removes any sort of physics on the object
		// The reason to remove this collision is because often the weapon will intersect with the player
		case Item::Holding:
		case Item::OnBelt:
		case Item::Equipping:
		case Item::Unequipping:
		case Item::UserSet:
			/*if ( pBody != NULL )
			{
				// Remove the body
				delete pBody;
				pBody = NULL;
			}*/
			if ( !pBody ) {
				CreatePhysics();
			}
			if ( pBody ) {
				pBody->SetMotionType( physMotion::MOTION_KEYFRAMED );
				pBody->SetCollisionLayer( Layers::PHYS_WEAPON );
			}
			// If owner doesn't exist, then drop the object
			if ( pOwner == NULL )
			{
				SetHoldState();
			}
		
		break;
		case Item::Hidden:
		case Item::SystemHidden:
			if ( pBody != NULL )
			{
				// Remove the body
				pBody->RemoveReference();
				delete pBody;
				pBody = NULL;
			}
		break;
		// This hold state creates a physics body on the object if it doesn't exist
		case Item::None:
			if ( pBody == NULL )
			{
				CreatePhysics();
			}
			else
			{
				// Make sure it's not kinmeatic
				pBody->SetMotionType( physMotion::MOTION_DYNAMIC ); // Probably should try to remove this call so often
				pBody->SetCollisionLayer( Layers::PHYS_ITEM );
			}
		break;
		// This hold state creates a kinematic physics body on the object.
		// This is a useful state for, example, books on a bookshelf or weapons on a rack.
		case Item::Hover:
			if ( pBody == NULL )
			{
				CreatePhysics();
			}
			else
			{
				// Make it kinematic
				pBody->SetMotionType( physMotion::MOTION_KEYFRAMED );
				pBody->SetCollisionLayer( Layers::PHYS_ITEM );
				//pBody->SetVelocity( Vector3d(0,0,0) );
				pBody->SetVelocity( pBody->GetVelocity() * Time::TrainerFactor( 1.0f ) );
				transform.SetDirty();
			}
		break;
	}

	// Depending on the hold state, toggle the visibility of the model
	bool modelVisible = false;
	if ( pModel )
	{
		switch ( holdState )
		{
			case Item::Hidden:
			case Item::OnBelt:
			case Item::SystemHidden:
				modelVisible = false;
				pModel->SetVisibility( false );
			break;
			default:
				modelVisible = true;
				pModel->SetVisibility( true );
			break;
		}
	}

	// Increment the countdown to change the motion quality
	if ( pBody != NULL )
	{
		fCollisionTimer += Time::deltaTime;
		if ( fCollisionTimer > 0.7f )
		{
			//pBody->SetQualityType( HK_COLLIDABLE_QUALITY_CRITICAL );
			pBody->SetQualityType( HK_COLLIDABLE_QUALITY_MOVING );
		}
	}
	else
	{
		fCollisionTimer = 0.0f;
	}

	// Depending on the hold state, move the weapon item
	bool bDoTransform = false;
	//Matrix4x4 mWantedTransform;
	XTransform mWantedTransform;
	Vector3d mWantedTranslation;
	Quaternion mWantedRotation;
	switch ( holdState )
	{
		case Item::Holding:
			//mWantedTransform = pOwner->GetHoldTransform();
			//mWantedTransform = GetHoldTransform();
			GetHoldTransform( mWantedTranslation, mWantedRotation );
			bDoTransform = true;
		break;
		case Item::OnBelt:
			mWantedTransform	= pOwner->GetBeltTransform( iBeltIndex );
			mWantedTranslation	= mWantedTransform.position;//mWantedTransform.getTranslation();
			mWantedRotation		= mWantedTransform.rotation;//mWantedTransform.getEulerAngles();
			bDoTransform = true;
		break;
		/*case Hidden:
		case Equipping:
		case Unequipping:*/	
	}
	// Change the rotation if is being equipped
	if ( holdState != Item::Hidden )
	{
		if ( holsterActive )
		{
			Real holsterPercent = 0.0f;
			if ( holsterAway ) {
				holsterPercent = holsterTimer / holsterTime_Away;
			}
			else {
				holsterPercent = holsterTimer / holsterTime_Equip;
			}
			mWantedRotation = Quaternion::CreateAxisAngle( Vector3d::left, 90.0f * holsterPercent ) *  mWantedRotation;
		}
	}
	// Update transforms
	if ( bDoTransform )
	{
		//transform.position = mWantedTransform.getTranslation();
		//transform.rotation = mWantedTransform.getEulerAngles();
		transform.position = mWantedTranslation;
		transform.rotation = mWantedRotation;
		//Vector3d eu = mWantedRotation.GetEulerAngles();
		//transform.rotation = Vector3d( eu.y, eu.x, -eu.z );

		//transform.position += Rotator( -transform.rotation.getEulerAngles() ) * Vector3d( 0,0,0.3f );
		//transform.position += transform.rotation.SwitchBasis
		Rotator tempRot = transform.rotation;
		//tempRot.SwitchBasis();
		//transform.position += vHoldingOffset*(!tempRot);
		//transform.position += Rotator( -transform.rotation.getEulerAngles() ) * vHoldingOffset;
		//transform.position += !Quaternion(-transform.rotation.getEulerAngles()) * vHoldingOffset;
		transform.position += transform.rotation * vHoldingOffset;
	}

	// Update model position
	if ( pModel )
	{
		if ( modelVisible ) //works because we JUST SET this
		{
			//pModel->transform.Get( transform );
			pModel->transform.position = this->transform.position;
			pModel->transform.rotation = this->transform.rotation;
			pModel->transform.localPosition = this->transform.localPosition;
			pModel->transform.localRotation = this->transform.localRotation;
		}
	}

	// Do holder's animations
	if ( pOwner && pOwner->IsCharacter() )
	{
		if ( holdState == Item::Holding )
		{
			//pOwner->PlayItemAnimation( "idle" );
			((CCharacter*)pOwner)->PlayItemAnimation( mAnimationIdleAction, iAnimationSubset, mHand, mAnimationIdleArgs );
		}
	}

	// Update tracker information last
	/*if ( bCheckCurrentSector || (holdType == Item::None) )
	{
		vCurrentSector = Zones.PositionToRV( transform.position );
		if ( !Zones.IsActiveArea( transform.position ) ) {
			DeleteObject( this );
		}
	}*/
}

// Change who's "holding" this item. NULL if belongs to world
// Note that setting this object's owner to null WILL force this weapon to belong to the world.
// Inventories must check if their weapons are still on them, as the weapon does not send any message when this happens!
void CWeaponItem::SetOwner ( CActor* pNewOwner )
{
	if ( pOwner && pOwner->IsCharacter() ) {
		if ( holdState == Item::Holding ) {
			((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Holster, iAnimationSubset, mHand, 0 );
		}
	}
	pOwner = pNewOwner;
	if ( pOwner == NULL ) {
		SetInventory( NULL );
	}
}
CActor* CWeaponItem::GetOwner ( void )
{
	return pOwner;
}
// Change the current hold state. Defaults to a world 'hold.'
// Values are in the CWeaponItem::HoldState enumeration.
void CWeaponItem::SetHoldState ( Item::HoldState nHoldState )
{
	if ( holdState != nHoldState )
	{
		if ( pOwner && pOwner->IsCharacter() )
		{
			if ( nHoldState == Item::Holding ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Draw, iAnimationSubset, mHand, 0 );
			}
			else if ( holdState == Item::Holding ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Holster, iAnimationSubset, mHand, 0 );
			}
		}
		holdState = nHoldState;
	}
}
Item::HoldState CWeaponItem::GetHoldState ( void )
{
	return holdState;
}
Item::HoldType CWeaponItem::GetHoldType ( void )
{
	return holdType;
}


// Request a holstering motion
void CWeaponItem::RequestHolster ( Item::HoldState nNextHoldState )
{
	// We taking something out?
	if ( holsterActive && !holsterAway )
	{
		// Interrupt it, set timer
		holsterInterrupted = true;
		holsterTimer = (holsterTimer/holsterTime_Equip) * holsterTime_Away;
	}
	else
	{	// Pull out from scratch
		holsterTimer = 0;
	}
	holsterActive = true;
	holsterAway = true;
	holsterTargetHoldstate = nNextHoldState;
}
// Have we put this item away?
bool CWeaponItem::IsHolstered ( void )
{
	return holsterAway && (holsterTimer > holsterTime_Away) && (holdState == holsterTargetHoldstate);
}
// Are we in the process of putting this item away?
bool CWeaponItem::IsHolstering ( void )
{
	return holsterActive;
}
// Take out the item
void CWeaponItem::Draw ( Item::HoldState nNextHoldState )
{
	// We putting something away?
	if ( holsterActive && holsterAway )
	{
		// Interrupt it, set timer
		holsterInterrupted = true;
		holsterTimer = (holsterTimer/holsterTime_Away) * holsterTime_Equip;
	}
	else
	{	// Pull out from scratch
		holsterTimer = holsterTime_Equip;
	}
	holsterActive = true;
	holsterAway = false;
	holsterTargetHoldstate = nNextHoldState;
	SetHoldState( nNextHoldState );
}


// Change the current inventory. NULL if belongs to world
// This is used for direct inventory queries on certain objects
void CWeaponItem::SetInventory ( CInventory* pNewInventory )
{
	pInventory = pNewInventory;
}

// This calculates the current hold transform, based on the current hold type
//Matrix4x4	CWeaponItem::GetHoldTransform ( void )
void CWeaponItem::GetHoldTransform ( Vector3d & vOutPosition, Quaternion & vOutRotation )
{
	//Matrix4x4 result;
	XTransform result;
	//Matrix4x4 tempRot;
	Matrix4x4 tempTrans;
	Quaternion tempRot;

	Vector3d rots;
	//ftype temp;
	switch ( holdType )
	{
	case Item::DefaultRanged:
		//tempTrans.setTranslation( -0.2f,-0.3f,-0.1f );
		/*tempRot.setRotation( 0,0,90 );
		result = pOwner->GetHoldTransform() * tempRot * tempTrans;
		break;*/
		result = pOwner->GetHoldTransform(mHand);
		vOutPosition = result.position;
		vOutRotation = result.rotation * Quaternion(Vector3d(180,0,90));
		/*temp = vOutRotation.x;
		vOutRotation.x = -vOutRotation.y;
		vOutRotation.y = temp;
		vOutRotation.z -= 90;*/
		break;
	case Item::ChannelMelee:
		result = pOwner->GetHoldTransform(mHand);
		vOutPosition = result.position;
		vOutRotation = result.rotation;
		if ( fCooldown[0] < -0.4f )
		{
			//temp = vOutRotation.x;
			//vOutRotation.x = -vOutRotation.y;
			//vOutRotation.y = temp;
			//vOutRotation.z -= 90;
			/*vOutRotation.y += 75;
			vOutRotation.z -= 10;*/
			tempRot = Vector3d( 0,75,-10 );
			vOutRotation = vOutRotation*tempRot;

			//vOutPosition.z -= 0.6f;
			vOutPosition += (!Quaternion( !vOutRotation )) * Vector3d( 0.65f,-0.3f,0.3f );
		}
		break;
	case Item::LightSourceMelee:
		result = pOwner->GetHoldTransform(mHand);
		//vOutPosition = result.getTranslation() + !Quaternion(-result.getEulerAngles())*Vector3d( 1.0f, 0,0 );
		vOutRotation = result.rotation * Quaternion(Vector3d(-90,0,100));
		vOutPosition = result.position; //+ (!Rotator( vOutRotation ) * Vector3d( 0,0,-0.5f ));
		//vOutRotation.x += 180;
		//tempRot = Vector3d( 180,0,0 );
		//vOutRotation = tempRot*vOutRotation;
		break;
	case Item::Bow:
		{
			/*result = pOwner->GetHoldTransform( 1 );
			vOutPosition = result.position;
			//rots = result.rotation.GetEulerAngles();
			//vOutRotation = Quaternion(Vector3d(0,90,0)) * Quaternion(Vector3d(-rots.z,rots.x,rots.y));
			vOutRotation = result.rotation * Quaternion(Vector3d(0,90,0));
			//* Quaternion(Vector3d(rots.x,0,0)) * Quaternion(Vector3d(0,rots.y,0)) * Quaternion(Vector3d(0,0,rots.z));*/
			XTransform hand1 = pOwner->GetHoldTransform( mHand%2 + 1 );
			XTransform hand2 = pOwner->GetHoldTransform( mHand%2 );
			//vOutRotation = Quaternion::CreateRotationTo( Vector3d::forward, (hand1.position - hand2.position).normal() );

			vOutRotation = hand1.rotation * Quaternion(Vector3d(0,90,0)) * Quaternion(Vector3d(0,0,90));
			if ( iAnimationSubset > 0 ) {
				Quaternion tempNextRot = Quaternion::CreateRotationTo( vOutRotation * Vector3d::forward, (hand1.position - hand2.position).normal() );
				vOutRotation = tempNextRot * vOutRotation;
			}
			vOutPosition = hand1.position;
		}
		break;
	case Item::Default:
		result = pOwner->GetHoldTransform(mHand);
		vOutPosition = result.position;
		vOutRotation = result.rotation;
		break;
	default:
		result = pOwner->GetHoldTransform(mHand);
		vOutPosition = result.position;
		//vOutRotation = result.rotation;
		vOutRotation = result.rotation * Quaternion(Vector3d(180,0,-90));
		//vOutRotation = !((!result.rotation)*Quaternion(Vector3d(-90,0,180)));
		break;
	}
	//return result;
}

// This sets the cooldown for the designated use.
void CWeaponItem::SetCooldown ( int iUse, float fNewCooldown )
{
	//if (( iUse >= 0 )&&( iUse <= 2 ))
	if ( iUse >= 0 && iUse <= 3 ) {
		fCooldown[iUse] = fNewCooldown;
	}
	else if ( iUse >= 10 && iUse <= 13 ) {
		fCooldown[iUse-6] = fNewCooldown;
	}
	else if ( iUse == 24 ) {
		fCooldown[8] = fNewCooldown;
	}
}
// This checks the cooldown for the designated use.
bool CWeaponItem::CanUse ( int iUse ) const
{
	if ( iUse >= 0 && iUse <= 3 ) {
		return ( fCooldown[iUse] <= 0.0f );
	}
	else if ( iUse >= 10 && iUse <= 13 ) {
		return ( fCooldown[iUse-6] <= 0.0f );
	}
	else if ( iUse == 24 ) {
		return ( fCooldown[8] < 0.0f );
	}
	return true;
}
// This gets the cooldown time left for the given use.
ftype	CWeaponItem::GetCooldown ( int iUse ) const
{
	if (( iUse >= 0 )&&( iUse <= 3 )) {
		return fCooldown[iUse];
	}
	else if ( iUse >= 10 && iUse <= 13 ) {
		return fCooldown[iUse-6];
	}
	else if ( iUse == 24 ) {
		return fCooldown[8];
	}
	else if ( iUse <= 8 ) {
		return fCooldown[iUse];
	}
	return 0;
}
// This tosses the item
void CWeaponItem::Toss ( Vector3d force )
{
	if ( holdState == Item::Holding )
	{
		SetOwner( NULL );
		holdState = Item::None;
		pOwner = NULL;
		if ( pBody == NULL )
			CreatePhysics();
		//pBody->ApplyForce( force );
		bHasToss = true;
		vTossVector = force;
	}
}
// This tosses a stack from the item
CWeaponItem* CWeaponItem::TossFromStack ( Vector3d force, short stackCount )
{
	short actualStackCount = stackCount;
	if ( actualStackCount > weaponItemState.iCurrentStack ) {
		actualStackCount = weaponItemState.iCurrentStack;
	}
	AddToStack( -actualStackCount );
	// Dupe this item
	CWeaponItem* tossedStack = Dupe();
	tossedStack->transform.Get( transform );
	tossedStack->SetStackSize( actualStackCount );
	// Toss new item
	tossedStack->Toss( force );
	return tossedStack;
}

// Physics extras
void CWeaponItem::FixedUpdate ( void )
{
	// Apply the force to the rigidbody in the fixed update
	if ( pBody )
	{
		if ( bHasToss )
		{
			pBody->ApplyForce( vTossVector );
			bHasToss = false;
		}
	}
}

// Get the item's current name. For item info and such.
const char* CWeaponItem::GetItemName ( void ) const
{
	return weaponItemState.sItemName;
}
// Returns the item data in a CWeaponItem::WItemData via reference return.
// This holds the information string of the item, if the item is stackable, and the item ID used for save files.
void CWeaponItem::GetItemData ( WItemData& outItemData ) const
{
	outItemData = weaponItemData;
}
const CWeaponItem::WItemData* CWeaponItem::GetItemData ( void ) const
{
	return &weaponItemData;
}

// Returns the item data in a CWeaponItem::WItemState via reference return.
// This holds the name string of the item, the current stack count of the item, and other current data.
void CWeaponItem::GetItemState ( WItemState& outItemState ) const
{
	outItemState = weaponItemState;
}
const CWeaponItem::WItemState* CWeaponItem::GetItemState ( void ) const
{
	return &weaponItemState;
}

//Set the info(name) for the item
void CWeaponItem::SetInfo (arstring<128> & name)
{
	weaponItemData.sInfo = name;
}
//Returns the size of the current stack
short CWeaponItem::GetStackSize (void)
{
	return weaponItemState.iCurrentStack;
}
//Returns the mamximum stack size of the item
short CWeaponItem::GetMaxStack (void)
{
	return weaponItemData.iMaxStack;
}
//Adds to the current stack
void CWeaponItem::AddToStack (short stack)
{
	weaponItemState.iCurrentStack += stack;
	//if ( weaponItemState.iCurrentStack < 0 ) {
	//	weaponItemState.iCurrentStack = 0;
	//}
}
//Sets the current stack size
void CWeaponItem::SetStackSize (short pile)
{
	weaponItemState.iCurrentStack = pile;
}
// Return the durability of the item
short CWeaponItem::GetDurability ( void )
{
	return weaponItemState.iCurrentDurability;
}
// Return the maximum durability of the item
short CWeaponItem::GetMaxDurability ( void )
{
	return weaponItemData.iMaxDurability;
}
// Set the durability of the item
void CWeaponItem::SetDurability (int con)
{
	weaponItemState.iCurrentDurability = con;
}
//Returns the ID of the item
short CWeaponItem::GetID (void)
{
	return weaponItemData.iItemId;
}
//Return if item can stack
bool CWeaponItem::GetCanStack (void)
{
	return weaponItemData.bCanStack;
}
// Return if item has durability
bool CWeaponItem::GetCanDegrade ( void ) const
{
	return weaponItemData.bCanDegrade;
}
//Return if item is a skill
bool CWeaponItem::GetIsSkill (void) const
{
	return weaponItemData.bIsSkill;
}
// Return the Item Type
ItemType CWeaponItem::GetItemType (void)
{
	return weaponItemData.eItemType;
}
// Returns number of needed hands to use this time
int	CWeaponItem::GetHands (void)
{
	return weaponItemData.iHands;
}
// Sets main hand this item is being used by
void CWeaponItem::SetHand ( const int newHand )
{
	mHand = newHand;
}

// Returns the rigidbody of the item
CRigidBody* CWeaponItem::GetRigidbody ( void )
{
	return pBody;
}
// Returns the model of the item
CModel*		CWeaponItem::GetModel ( void )
{
	return pModel;
}

// Sets owner's viewangle offset
void	CWeaponItem::SetViewAngleOffset ( const ftype offset )
{
	if ( holdState == Item::Holding )
	{
		if ( pOwner->GetBaseClassName() == "CActor_Character" ) {
			CCharacter* pOwnerChar = (CCharacter*)pOwner;
			pOwnerChar->SetViewAngleOffset( offset );
		}
	}
}

// Sets if items should update their tracker information about the world.
// Tracker information is used to manage unloading items when out of range.
void	CWeaponItem::SetTrackerState ( const bool update_information )
{
	//bCheckCurrentSector = update_information;
	throw Core::NotYetImplementedException();
}


// Serialize default items
void	CWeaponItem::serialize ( Serializer & ac,  const uint version )
{
	ac & weaponItemState.iCurrentStack;
	ac & weaponItemState.iCurrentDurability;
	ac & weaponItemState.sItemName;
}


//===============================================================================================//
//		CGameBehavior
//
// Object base class for the engine
// All objects that have some sort of behavior inherit from this base class.
//===============================================================================================//
#ifndef _C_GAME_BEHAVIOR_
#define _C_GAME_BEHAVIOR_

//===============================================================================================//
// INCLUDES
//===============================================================================================//
#include <cassert>

#include "core/common.h"	// Common core
#include "core/time.h"		// Time definition
#include "gbbool.h"			// Dirty bools

#include "physical/system/Layers.h"			// Physics layers
#include "engine/types.h"	// Engine integral types
#include "engine/network/GameNetworking.h"	// Networking (includes server.h and client.h)
#include "engine/physics/collider/sCollision.h"	// Collision info

//===============================================================================================//
// MACROS
//===============================================================================================//
#ifndef ClassName
#	define ClassName(_strname) public: virtual string GetTypeName ( void ) { return string(_strname); };
#endif
#ifndef BaseClass
#	define BaseClass(_strname) public: virtual string GetBaseClassName ( void ) { return string(_strname); };
#endif
#ifndef delete_safe
#	define delete_safe(_ptr) { if ( _ptr ) { delete (_ptr); (_ptr) = NULL; } };
#endif
#ifndef delete_safe_decrement
#	define delete_safe_decrement(_ptr) { if ( _ptr ) { (_ptr)->RemoveReference(); delete (_ptr); (_ptr) = NULL; } };
#endif

//===============================================================================================//
// TEMPLATES
//===============================================================================================//
// == GLOBAL INSTANTIATION TEMPLATE ==
// USED FOR CLASS REGISTRATION SYSTEM
template<typename T> CGameBehavior * _instGameBehavior( void ) { return new T; }

//===============================================================================================//
// PROTOTYPES
//===============================================================================================//
class CGameState;
class CGameHandle;

//===============================================================================================//
// CLASS DEFINTION
//===============================================================================================//
class CGameBehavior
{
private:
	// Disable CGameBehavior copying
	CGameBehavior & operator= (const CGameBehavior & other) { assert(0); return(*this); }
	CGameBehavior ( const CGameBehavior& other) { assert(0); }

	// Register classnames
	ClassName( "CGameBehavior" );
	BaseClass( "undefined" );
public:

	ENGINE_API			CGameBehavior ( void );
	ENGINE_API virtual	~CGameBehavior ( void );
	
	//===============================================================================================//
	// Step events
	//===============================================================================================//

	// Game step

	ENGINE_API virtual void Update ( void ) =0;
	ENGINE_API virtual void LateUpdate ( void ) {};
	ENGINE_API virtual void PostFixedUpdate ( void ) {};
	ENGINE_API virtual void PostUpdate ( void ) {};

	// Physics step

	ENGINE_API virtual void FixedUpdate ( void ) {};
	ENGINE_API virtual void RigidbodyUpdate ( void ) {}; // DO NOT OVERLOAD THIS FUNCTION

	//===============================================================================================//
	// General events
	//===============================================================================================//

	ENGINE_API virtual void OnCreate ( void ) {};
	ENGINE_API virtual void OnDestroy ( void ) {};

	ENGINE_API virtual void OnEnable ( void ) {};
	ENGINE_API virtual void OnDisable ( void ) {};

	//===============================================================================================//
	// Collision events
	//===============================================================================================//

	ENGINE_API virtual void OnCollide ( sCollision& ) {};
	ENGINE_API virtual void OnCollisionEnter ( sCollision& ) {};
	ENGINE_API virtual void OnCollisionLeave ( sCollision& ) {};

	//===============================================================================================//
	// Messenger events
	//===============================================================================================//

	ENGINE_API virtual void OnReceiveSignal ( const uint64_t signal ) {};

	//===============================================================================================//
	// Memory Management : Reference counting
	//===============================================================================================//

	ENGINE_API void RemoveReference ( void ) {
		if ( referenceCount > 0 )
			referenceCount--;
		else
			throw Core::InvalidCallException();
	}
	ENGINE_API void AddReference ( void ) { referenceCount++; }
	ENGINE_API bool HasReference ( void ) { return (referenceCount != 0); }

	//===============================================================================================//
	// Object Query/Handling
	//===============================================================================================//

	//	GetId
	// Returns the ID of the behavior
	ENGINE_API gameid_t GetId ( void ) {
		return id;
	}

	//	GetNetId
	// Returns the NetID of the behavior
	ENGINE_API netid_t GetNetId ( void ) {
		return netid;
	}

	//	GetGUID
	// Returns the GUID of the behavior
	ENGINE_API guid32_t GetGUID ( void ) {
		return guid;
	}

	// Returns a pointer to the spot in the array where the object is held.
	// This is very unsafe and can cause memory leaks if not used properly!
	// Avoid use of this function if possible!
	//CGameBehavior**	GetArrayPointer ( void ); // TODO: Replace with managed CGameHandle() that provides the same info
	ENGINE_API CGameHandle	GetHandle ( void );
	
	//	DeleteObject
	// Adds this object to be deleted next frame
	ENGINE_API static void DeleteObject ( CGameBehavior* pObjectToDelete );

	//	DeleteObjectDelayed
	// Sets this object to be deleted after a certain delay.
	// All events are still handled
	ENGINE_API static void DeleteObjectDelayed ( CGameBehavior* pObjectToDelete, float fDeleteTime );

	//	SetPersistence
	// Sets the persistent state of the object.
	// If persistence is enabled, then the object will not be considered for deletion on a world cleaning event.
	ENGINE_API void SetPersistence ( bool nPersistence );

	//===============================================================================================//
	// Public properties
	//===============================================================================================//

	//	layer
	// User-defined layer mask for game logic
	Layers::Layer layer;
	//	active
	// Controls if Update() and other step events are called.
	gbbool active;
	//	name
	// Name of object for human identification
	string name;

protected:
	//===============================================================================================//
	// Protected Methods
	//===============================================================================================//

	//	SetId ()
	// Changes this object's ID. This should NEVER be called manually.
	ENGINE_API void SetId ( const gameid_t );
	//	SetNetId ()
	// Changes this object's Net ID. This should NEVER be called manually.
	ENGINE_API void SetNetId ( const netid_t );

private:
	//===============================================================================================//
	// Private data
	//===============================================================================================//

	//	id
	// Object's current ID
	gameid_t	id;
	//	netid
	// Object's network ID used to sync tables
	netid_t		netid;
	//  guid
	// Object's unique ID used for referencing game behaviors
	guid32_t	guid;
	//	referenceCount
	// used for reference counting
	uint16_t	referenceCount;
	//	persistent
	// used to check if this object is persistent thru scene ending or not
	bool		persistent;

	// Give storage class access to SetId
	friend CGameState;
};

#endif

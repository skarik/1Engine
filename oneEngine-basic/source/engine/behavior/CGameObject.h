
#ifndef _C_GAME_OBJECT_
#define _C_GAME_OBJECT_

// Includes
#include "CGameBehavior.h"
#include "core-ext/transform/Transform.h"

// Class Definition
class CGameObject : public CGameBehavior
{
	ClassName( "CGameObject" );
	BaseClass( "CGameBehavior" );
public:
	// Constructor setting owner behavior
	CGameObject ( void ) : CGameBehavior ()
	{
		//transform.owner = this;
		//transform.ownerType = Transform::TYPE_BEHAVIOR;
		//transform.name = this->name;
	};
	~CGameObject ( void ) {
		;
	}
	
	void Update ( void ) {};
	void LateUpdate ( void ) {};
	void FixedUpdate ( void ) {};

public:
	// Basic transformation
	core::Transform transform;
};

#endif
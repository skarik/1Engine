// The extendable game object is a great class for instanciating those one-time objects and other such stuff


#ifndef _C_EXTENDABLE_GAME_OBJECT_
#define _C_EXTENDABLE_GAME_OBJECT_

// Includes
#include "CGameObject.h"
#include "renderer/logic/CLogicObject.h"
#include "renderer/object/CRenderableObject.h"
#include "engine/physics/collider/CCollider.h"
#include <vector>

// Class object
class CExtendableGameObject : public CGameObject
{
	ClassName( "CExtendableGameObject" );
public:
	// ==Constructor==
	CExtendableGameObject ( void ) : CGameObject()
	{

	}
	// ==Destructor==
	~CExtendableGameObject ( void )
	{
		// Loop through all the components. If they've no owner, remove them.
		for ( std::vector<CGameBehavior*>::iterator it = vpComponents.begin(); it != vpComponents.end(); it++ )
		{
			(*it)->RemoveReference();
			if ( (*it)->HasReference() == false )
			{
				DeleteObject( *it );
			}
		}
		for ( std::vector<CRenderableObject*>::iterator it = vpRComponents.begin(); it != vpRComponents.end(); it++ )
		{
			delete_safe(*it);
		}
		for ( std::vector<CLogicObject*>::iterator it = vpLComponents.begin(); it != vpLComponents.end(); it++ )
		{
			delete_safe (*it);
		}
		for ( std::vector<CCollider*>::iterator it = vpCComponents.begin(); it != vpCComponents.end(); it++ )
		{
			delete_safe (*it);
		}
	}

	// Add new component (game behavior) to track
	CGameBehavior* AddComponent ( CGameBehavior* pNewBehavior )
	{
		// Check if the component is in the list
		for ( std::vector<CGameBehavior*>::iterator it = vpComponents.begin(); it != vpComponents.end(); it++ )
		{
			if ( (*it) == pNewBehavior )
				return pNewBehavior;
		}
		vpComponents.push_back( pNewBehavior );
		return pNewBehavior;
	}
	// Add new component (logic renderable object) to track
	CLogicObject* AddComponent ( CLogicObject* pNewBehavior )
	{
		// Check if the component is in the list
		for ( std::vector<CLogicObject*>::iterator it = vpLComponents.begin(); it != vpLComponents.end(); it++ )
		{
			if ( (*it) == pNewBehavior )
				return pNewBehavior;
		}
		vpLComponents.push_back( pNewBehavior );
		return pNewBehavior;
	}
	// Add new component (renderable object) to track
	CRenderableObject* AddComponent ( CRenderableObject* pNewBehavior )
	{
		// Check if the component is in the list
		for ( std::vector<CRenderableObject*>::iterator it = vpRComponents.begin(); it != vpRComponents.end(); it++ )
		{
			if ( (*it) == pNewBehavior )
				return pNewBehavior;
		}
		vpRComponents.push_back( pNewBehavior );
		return pNewBehavior;
	}
	// Add new component (collider object) to track
	CCollider* AddComponent ( CCollider* pNewBehavior )
	{
		// Check if the component is in the list
		for ( std::vector<CCollider*>::iterator it = vpCComponents.begin(); it != vpCComponents.end(); it++ )
		{
			if ( (*it) == pNewBehavior )
				return pNewBehavior;
		}
		vpCComponents.push_back( pNewBehavior );
		return pNewBehavior;
	}

	// Remove component
	void RemoveComponent ( void* pBehavior )
	{
		// Check all lists for the component
		for ( std::vector<CGameBehavior*>::iterator it = vpComponents.begin(); it != vpComponents.end(); it++ )
		{
			if ( (*it) == pBehavior ) {
				vpComponents.erase(it);
				return;
			}
		}
		for ( std::vector<CRenderableObject*>::iterator it = vpRComponents.begin(); it != vpRComponents.end(); it++ )
		{
			if ( (*it) == pBehavior ) {
				vpRComponents.erase(it);
				return;
			}
		}
		for ( std::vector<CLogicObject*>::iterator it = vpLComponents.begin(); it != vpLComponents.end(); it++ )
		{
			if ( (*it) == pBehavior ) {
				vpLComponents.erase(it);
				return;
			}
		}
		for ( std::vector<CCollider*>::iterator it = vpCComponents.begin(); it != vpCComponents.end(); it++ )
		{
			if ( (*it) == pBehavior ) {
				vpCComponents.erase(it);
				return;
			}
		}
	}

protected:
	// List of the components
	std::vector<CGameBehavior*> vpComponents;
	std::vector<CRenderableObject*> vpRComponents;
	std::vector<CLogicObject*> vpLComponents;
	std::vector<CCollider*> vpCComponents;
};


#endif
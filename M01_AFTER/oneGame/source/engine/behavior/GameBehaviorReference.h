
#ifndef _C_GAME_BEHAVIOR_REFERENCE_H_
#define _C_GAME_BEHAVIOR_REFERENCE_H_

#include "CGameBehavior.h"
#include "../state/CGameState.h"

class GameBehaviorReference
{

public:
	explicit				GameBehaviorReference ( CGameBehavior* object )
		: targetobj(object), objectid(object->GetId()) {;};
							~GameBehaviorReference ( void )
		{;};

	CGameBehavior&				operator*() 
		{
			if ( targetobj == NULL ) throw std::exception;
			if ( CGameState::pActive->GetBehavior(objectid) == targetobj->GetId() ) {
				return *targetobj;
			}
			throw std::exception;
		}
	const CGameBehavior&		operator*() const
		{
			if ( targetobj == NULL ) throw std::exception;
			if ( CGameState::pActive->GetBehavior(objectid) == targetobj->GetId() ) {
				return *targetobj;
			}
			throw std::exception;
		}
	CGameBehavior*				operator->()
		{
			if ( targetobj == NULL ) throw std::exception;
			if ( CGameState::pActive->GetBehavior(objectid) == targetobj->GetId() ) {
				return targetobj;
			}
			throw std::exception;
		}
	const CGameBehavior*		operator->() const
		{
			if ( targetobj == NULL ) return std::exception;
			if ( CGameState::pActive->GetBehavior(objectid) == targetobj->GetId() ) {
				return targetobj;
			}
			throw std::exception;
		}

	operator const CGameBehavior* ( void ) const
		{
			return targetobj;
		}

protected:
	CGameBehavior*		targetobj;
	unsigned int		objectid;
};


#endif//_C_GAME_BEHAVIOR_REFERENCE_H_


#include "CGameHandle.h"
#include "engine/state/CGameState.h"

CGameHandle::CGameHandle ( void )
{
	this->behavior = NULL;
	this->id = 0;
	this->guid = (guid32_t)(-1);
}

CGameHandle::CGameHandle ( CGameBehavior* behavior )
{
	this->behavior = behavior;
	this->id = behavior->GetId();
	this->netid = behavior->GetNetId();
	this->guid = behavior->GetGUID();
}

// Assignemnt
void CGameHandle::operator= ( CGameBehavior* behavior )
{
	this->behavior = behavior;
	this->id = behavior->GetId();
	this->netid = behavior->GetNetId();
	this->guid = behavior->GetGUID();
}

CGameHandle::operator CGameBehavior*() 
{
	if ( CGameState::Active()->GetBehavior(id) == behavior && behavior->GetGUID() == guid ) {
		return behavior;
	}
	else {
		throw core::NullReferenceException();
		return NULL;
	}
}
CGameBehavior* CGameHandle::operator-> ( void )
{
	if ( CGameState::Active()->GetBehavior(id) == behavior && behavior->GetGUID() == guid ) {
		return behavior;
	}
	else {
		throw core::NullReferenceException();
		return NULL;
	}
}
// Check
bool CGameHandle::valid ( void )
{
	if ( CGameState::Active()->GetBehavior(id) == behavior && behavior->GetGUID() == guid ) {
		return true;
	}
	else {
		return false;
	}
}
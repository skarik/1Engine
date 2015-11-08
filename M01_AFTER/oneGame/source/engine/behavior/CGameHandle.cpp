

#include "CGameHandle.h"
#include "engine/state/CGameState.h"



CGameHandle::CGameHandle ( CGameBehavior* behavior )
{
	this->behavior = behavior;
	this->id = behavior->GetId();
	this->netid = behavior->GetNetId();
}

CGameHandle::operator CGameBehavior*() 
{
	if ( CGameState::Active()->GetBehavior(id) == behavior && behavior->GetNetId() == netid ) {
		return behavior;
	}
	else {
		throw Core::NullReferenceException();
		return NULL;
	}
}
CGameBehavior* CGameHandle::operator-> ( void )
{
	if ( CGameState::Active()->GetBehavior(id) == behavior && behavior->GetNetId() == netid ) {
		return behavior;
	}
	else {
		throw Core::NullReferenceException();
		return NULL;
	}
}
// Check
bool CGameHandle::valid ( void )
{
	if ( CGameState::Active()->GetBehavior(id) == behavior && behavior->GetNetId() == netid ) {
		return true;
	}
	else {
		return false;
	}
}
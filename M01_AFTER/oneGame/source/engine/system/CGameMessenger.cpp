
#include "CGameMessenger.h"
#include "engine/state/CGameState.h"

CGameMessenger::CGameMessenger( CGameState* targetGamestate )
	: m_gamestate(targetGamestate)
{

}

void CGameMessenger::Send ( const std::string& target, signal_t signal )
{
	CGameBehavior* pBehavior;
	for ( uint i = 0; i < m_gamestate->iCurrentIndex; i += 1 )
	{
		pBehavior = m_gamestate->pBehaviors[i];
		if ( pBehavior && pBehavior->GetTypeName() == target )
		{
			pBehavior->OnReceiveSignal( signal );
		}
	}
}
void CGameMessenger::SendGlobal ( signal_t signal )
{
	CGameBehavior* pBehavior;
	for ( uint i = 0; i < m_gamestate->iCurrentIndex; i += 1 )
	{
		pBehavior = m_gamestate->pBehaviors[i];
		if ( pBehavior )
		{
			pBehavior->OnReceiveSignal( signal );
		}
	}
}
void CGameMessenger::SendFirstUnique ( const std::string& target, signal_t signal )
{
	CGameBehavior* pBehavior;
	for ( uint i = 0; i < m_gamestate->iCurrentIndex; i += 1 )
	{
		pBehavior = m_gamestate->pBehaviors[i];
		if ( pBehavior && pBehavior->GetTypeName() == target )
		{
			pBehavior->OnReceiveSignal( signal );
			return;
		}
	}
}
void CGameMessenger::SendTo ( const std::string& name, signal_t signal )
{
	CGameBehavior* pBehavior;
	for ( uint i = 0; i < m_gamestate->iCurrentIndex; i += 1 )
	{
		pBehavior = m_gamestate->pBehaviors[i];
		if ( pBehavior && pBehavior->name == name )
		{
			pBehavior->OnReceiveSignal( signal );
			return;
		}
	}
}
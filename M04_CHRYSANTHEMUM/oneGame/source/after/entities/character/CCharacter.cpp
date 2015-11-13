
#include "CCharacter.h"

std::vector<CCharacter*> CCharacter::msList;


// NotifyAttack is used to go through attack notification list and call all the function pointers that have been added.
void	CCharacter::NotifyAttack ( void )
{
	for ( std::vector<cbpObserverInfo>::iterator observer = observersAttack.begin(); observer != observersAttack.end(); ++observer )
	{
		((observer->first)->*(observer->second))(this);
	}
}
// Adds a function to call when this Character performs an attacking action.
void	CCharacter::AddNotifyWhenAttack ( CCallbackObserver* nlink, arCallback_Clb_Ptr ncb )
{
	observersAttack.push_back(cbpObserverInfo(nlink,ncb));
}
// Remove all notifications of attack with matching pointer.
void	CCharacter::RemoveNotiftyWhenAttack ( CCallbackObserver* nlink )
{
	for ( std::vector<cbpObserverInfo>::iterator observer = observersAttack.begin(); observer != observersAttack.end(); )
	{
		if ( observer->first == nlink ) {
			observer = observersAttack.erase( observer );
		}
		else {
			++observer;
		}
	}
}
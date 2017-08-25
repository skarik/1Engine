
#include "Events.h"

using std::vector;

// NotifyAttack is used to go through attack notification list and call all the function pointers that have been added.
void	CbpEvent::notifyObservers ( void* arg )
{
	for ( vector<cbpObserverInfo>::iterator observer = observers.begin(); observer != observers.end(); ++observer )
	{
		((observer->first)->*(observer->second))(arg);
	}
}
// Adds a function to call when this Character performs an attacking action.
void	CbpEvent::addNotify ( cbpObserverInfo ninfo )
{
	observers.push_back( ninfo );
}
// Remove all notifications of attack with matching pointer.
void	CbpEvent::removeNotify ( CCallbackObserver* nlink )
{
	for ( vector<cbpObserverInfo>::iterator observer = observers.begin(); observer != observers.end(); )
	{
		if ( observer->first == nlink ) {
			observer = observers.erase( observer );
		}
		else {
			++observer;
		}
	}
}


// NotifyAttack is used to go through attack notification list and call all the function pointers that have been added.
void	CbvEvent::notifyObservers ( void )
{
	for ( vector<cbvObserverInfo>::iterator observer = observers.begin(); observer != observers.end(); ++observer )
	{
		((observer->first)->*(observer->second))();
	}
}
// Adds a function to call when this Character performs an attacking action.
void	CbvEvent::addNotify ( cbvObserverInfo ninfo )
{
	observers.push_back( ninfo );
}
// Remove all notifications of attack with matching pointer.
void	CbvEvent::removeNotify ( CCallbackObserver* nlink )
{
	for ( vector<cbvObserverInfo>::iterator observer = observers.begin(); observer != observers.end(); )
	{
		if ( observer->first == nlink ) {
			observer = observers.erase( observer );
		}
		else {
			++observer;
		}
	}
}
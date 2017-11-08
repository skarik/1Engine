//===============================================================================================//
//
//		Event.h - Legacy Event System
//
// Legacy event system that was used for callback within AFTER's game code, especially with delays.
// Kept as a possibly useful resource.
//
//===============================================================================================//
#ifndef CORE_1ENGINE_EVENTS_H_
#define CORE_1ENGINE_EVENTS_H_

#include "core/common.h"
#include <vector>

typedef std::pair<CCallbackObserver*,arCallback_Clb_Void> cbvObserverInfo;
typedef std::pair<CCallbackObserver*,arCallback_Clb_Ptr> cbpObserverInfo;

class CbvEvent
{
public:
	void addNotify ( cbvObserverInfo );
	void removeNotify ( CCallbackObserver* );
	void notifyObservers ( void );

private:
	std::vector<cbvObserverInfo>	observers;
};
class CbpEvent
{
public:
	void addNotify ( cbpObserverInfo );
	void removeNotify ( CCallbackObserver* );
	void notifyObservers ( void* arg );

private:
	std::vector<cbpObserverInfo>	observers;
};

#endif//CORE_1ENGINE_EVENTS_H_
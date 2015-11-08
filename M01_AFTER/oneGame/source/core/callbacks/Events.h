
#ifndef _WON_ENGINE_EVENTS_H_
#define _WON_ENGINE_EVENTS_H_

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

#endif//_WON_ENGINE_EVENTS_H_
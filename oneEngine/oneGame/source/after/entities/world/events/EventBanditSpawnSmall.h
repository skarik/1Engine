
#ifndef _EVENT_BANDIT_SPAWN_SMALL_H_
#define _EVENT_BANDIT_SPAWN_SMALL_H_

#include "after/entities/world/events/CEventBase.h"

namespace GameEvents
{

	class EventBanditSpawnSmall : public CEventBase
	{
	public:
		EventBanditSpawnSmall ( void );
		~EventBanditSpawnSmall ( void );

		void Update ( void );

	private:
		uint32_t mainBanditId;
	};

}


#endif//_EVENT_BANDIT_SPAWN_SMALL_H_
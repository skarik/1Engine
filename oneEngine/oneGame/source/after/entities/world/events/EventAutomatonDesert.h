
#ifndef _EVENT_AUTOMATON_SPAWN_H_
#define _EVENT_AUTOMATON_SPAWN_H_

#include "after/entities/world/events/CEventBase.h"

namespace GameEvents
{
	class EventAutomatonDesert : public CEventBase
	{
	public:
		EventAutomatonDesert ( void );
		~EventAutomatonDesert ( void );

		void Update ( void );

	private:
		uint32_t mainBanditId;
	};
}


#endif//_EVENT_AUTOMATON_SPAWN_H_

#ifndef _EVENT_DESERT_STORM_H_
#define _EVENT_DESERT_STORM_H_

#include "after/entities/world/events/CEventBase.h"

class CParticleSystem;

namespace GameEvents
{

	class EventDesertStorm : public CEventBase
	{
	public:
		EventDesertStorm ( void );
		~EventDesertStorm ( void );

		void Update ( void );

	private:
		Real lifetime;
		Real targetLifetime;

		Real centerOffset;

		CParticleSystem*	ps;
	};

}


#endif//_EVENT_DESERT_STORM_H_

#ifndef _C_GAME_EVENT_GENERATOR_H_
#define _C_GAME_EVENT_GENERATOR_H_

#include <unordered_map>
#include <map>
#include <functional>

#include "engine/behavior/CGameBehavior.h"
#include "engine/behavior/CGameObject.h"

namespace World
{
	struct gameEventEntry
	{
		ftype		baseChance;
		uint32_t	target;
		char		active;
		char padding[251];

		gameEventEntry ( void ) : baseChance(1.0f), target(0), active(0) {}
	};
	struct gameEventInstantiator
	{
		//_instantiationFunction	func;
		CGameBehavior*			(*func)(void);
		string					luaname;
		CGameBehavior* operator() ( void );
		bool	valid ( void );
		gameEventInstantiator ( void ) : func(NULL) {}
	};
	class CGameEventGenerator : public CGameBehavior
	{
	public:
		CGameEventGenerator ( void );
		~CGameEventGenerator ( void );

		void Update ( void );

		// Adds an event to the event entry list
		void AddEvent ( const string&, const gameEventEntry& );
		void RemoveEvent ( const string& );

		// Load and save state
		void SaveState ( void );
		void LoadState ( void );
	private:
		void RegisterGameEvents ( void );
		void AddGameEvent ( const string& name, CGameBehavior*(*func)(void) );
		void AddGameEvent ( const string& name, const string& luaName );

		CGameObject*	m_target_player;
		ftype			m_travel_distance;
		ftype			m_event_timer;
		Vector3d		m_start_position;
		
		std::unordered_map<string,gameEventInstantiator>	m_instance_list;
		std::map<string,gameEventEntry>						m_event_list;
	};


	extern CGameEventGenerator*	EventGenerator;
};

#endif//_C_GAME_EVENT_GENERATOR_H_
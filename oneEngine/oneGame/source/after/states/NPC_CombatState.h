
#ifndef _NPC_COMBAT_STATE_H_
#define _NPC_COMBAT_STATE_H_

#include "core/types/float.h"

namespace NPC
{
	struct sCombatInfo
	{
		enum eCombatState {
			RELAXED,
			GUARDED,
			COMBAT
		} state;
		ftype timeInState;

		explicit sCombatInfo ( void );

		void Update ( void );

		void OnSheathe ( void );
		void OnDraw ( void );
		void OnAttack ( void );
		void OnDefend ( void );
		void OnAttacked ( void );
	};
};

#endif//_NPC_AI_STATE_H_

#ifndef _AFTER_TYPES_CHARACTER_TYPE_ENUM_H_
#define _AFTER_TYPES_CHARACTER_TYPE_ENUM_H_

#include "core/types/types.h"

namespace NPC
{
	//	Character Type
	// Type (and possibly class) of character.
	enum eCharacterType : uint8_t
	{
		// CHARACTER_TYPE_GENERAL refers to the base class CCharacter
		// Has very minimal functionality, but can be assured is a CCharacter.
		CHARACTER_TYPE_GENERAL	= 0,

		// CHARACTER_TYPE_ZCC refers to the class CZonedCharacter (ZCC = Zoned Character Carrier)
		// ZCCs are typically NPCs that are specific to a zone.
		CHARACTER_TYPE_ZCC		= 2,

		// CHARACTER_TYPE_PLAYER refers to the class CAfterPlayer.
		// CAfterPlayer is the main gameplay character type meant for player control.
		CHARACTER_TYPE_PLAYER	= 4
	};
}

#endif//_AFTER_TYPES_CHARACTER_TYPE_ENUM_H_
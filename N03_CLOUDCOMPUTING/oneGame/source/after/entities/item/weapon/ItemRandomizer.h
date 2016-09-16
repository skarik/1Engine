
#ifndef _ITEM_RANDOMIZER_H_
#define _ITEM_RANDOMIZER_H_

#include "core/types/types.h"
#include "core/types/float.h"

namespace WeaponItem
{
	extern bool IsPart ( const int type, const char* name );
	extern void ModifyNumber ( Real& inOutParamter, const char* input );
};

#endif//_ITEM_RANDOMIZER_H_
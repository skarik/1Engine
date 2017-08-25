
#include "DebuffList.h"

// All debuff includes
#include "racial/BuffRacialDarkElf.h"
#include "racial/BuffRacialKitten.h"

#include "racial/BuffFluxxorManablood.h"

#include "standard/DebuffBurningFlametoss.h"
#include "standard/DebuffSlowBurnFrostball.h"

#include "standard/DebuffRooted.h"
#include "standard/DebuffSlow.h"
#include "standard/DebuffDisable.h"
#include "standard/DebuffDPS.h"

namespace Debuffs
{
	// Global list instance
	CDebuffList DebuffList;
};

// ===== DEBUFF CLASS+ID LIST =====
// ADD YOUR DEBUFF/BUFF HERE FOR IT TO BE PROPERLY REGISTERED TO THE GAME.
// IF IT IS NOT REGISTERED, VARIOUS ISSUES WILL OCCUR.
void Debuffs::CDebuffList::CreateDebuffList ( void )
{
	RegisterBuff( BuffRacialDarkElf,	20 );
	RegisterBuff( BuffRacialKitten,		21 );

	RegisterBuff( BuffFluxxorManablood,	30 );

	RegisterBuff( DebuffBurningFlametoss,	100 );
	RegisterBuff( DebuffSlowBurnFrostball,	101 );

	RegisterBuff( DebuffRooted, 200);
	RegisterBuff( DebuffSlow, 201);
	RegisterBuff( DebuffDPS, 202);
	RegisterBuff( DebuffDisable, 203);

}

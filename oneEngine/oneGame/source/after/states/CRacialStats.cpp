
#include "core/math/Math.h"
#include "core/math/random/Random.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/serializer.h"
#include "core-ext/system/io/serializer/ISerialBinary.h"
#include "core-ext/system/io/serializer/OSerialBinary.h"

#include "after/entities/character/CCharacter.h"

#include "CRacialStats.h"
#include "player/CPlayerStats.h"
#include "debuffs/racial/BuffRacialDarkElf.h"
#include "debuffs/racial/BuffRacialKitten.h"
#include "debuffs/racial/BuffFluxxorManablood.h"

// Set Default stats
void CRacialStats::SetDefaults ( void )
{
	// ====== PLAYERTYPE STATS ======
	iRace = CRACE_DEFAULT;
	iGender = CGEND_FEMALE;
	iHairstyle = 0;

	iSpecialType = CSPCL_NONE;
	iBodyType = 0;
	iFaceType = 0;
	iMiscType = 0;

	sPlayerName	= "Wanderer";
	sLastName	= "";

	// ====== VANITY STATS ======
	// == Player Colors ==
	cEyeColor	= Color( 0.4f, 0.4f, 0.4f );
	cFocusColor	= Color( 0.8f, 0.2f, 0.2f );

	cHairColor	= Color( 0.4f, 0.4f, 0.4f );
	cSkinColor	= Color( 0.9f, 0.7f, 0.5f );

	//RerollColors();
	//LimitColors();

	// == Visual Equipment for Quick Load ==
	sGearHead		= "";
	sGearNecklace	= "";
	sGearShoulder	= "";
	sGearChest		= "";
	sGearRighthand	= "";
	sGearLefthand	= "";
	sGearLegs		= "";
	sGearRightfoot	= "";
	sGearLeftfoot	= "";

	// == Tattoo and Body marking info ==
	iTattooCount	= 0;

	// ====== GAMEPLAY STATS ======
	// Point+level stats
	stats->ResetStats();

	// ====== PHYSICS STATS ======
	// == Player Hull Sizes ==
	fPlayerRadius		= 0.7f;

	fStandingHeight		= 5.7f;
	fCrouchingHeight	= 3.7f;
	fProneHeight		= 1.6f;

	// == Movement ==
	// Speed and acceleration
	fRunSpeed			= 11.0f;
	fCrouchSpeed		= 6.0f;
	fProneSpeed			= 2.0f;
	fSprintSpeed		= 17.0f;
	fSwimSpeed			= 12.0f;
	fGroundAccelBase	= 55.0f;
	fAirAccelBase		= 10.0f;
	fWaterAccelBase		= 20.0f;
	fJumpVelocity		= 12.0f;
	// Special moves
	bCanAutojump		= true;
	bCanPowerslide		= true;
	bCanSprint			= true;
	bCanCrouchSprint	= false;

	fSlideTime			= 0.7f; // length of powerslide

	iWallRunStepCount	= 2; // Doesn't include initial step
}

// Load stats from file
void CRacialStats::LoadFromFile ( CBinaryFile& file )
{
	{
		// ====== VERSION INFO ======
		file.ReadUInt32();
		file.ReadUInt32();
		file.ReadUInt32();
		file.ReadUInt32();

		// General Player Stats
		ISerialBinary deserializer ( &file );
		deserializer >> (*this);

		// Gameplay Point stats
		deserializer >> (*stats);

		// == Tattoo and Body marking info ==
		iTattooCount	= file.ReadUShort();
		tattooList.clear();
		for ( ushort i = 0; i < iTattooCount; ++i ) {
			tattoo_t newTat;
			file.ReadData( (char*)&newTat, sizeof( tattoo_t ) );
			tattooList.push_back( newTat );
		}
	}
	
	// Now, check for invalid values and thus set defaults
	if ( iMovestyle != CMOVES_RANDOM && (iMovestyle >= CMOVES_MAX || iMovestyle < 0) ) {
		iMovestyle = CMOVES_DEFAULT_INVALID;
	}
	if ( iMovestyle == CMOVES_DEFAULT_INVALID )
	{
		switch ( iRace ) {
		case CRACE_ELF:		iMovestyle = CMOVES_GLOOMY;		break;
		case CRACE_KITTEN:	iMovestyle = CMOVES_JOLLY;		break;
		default:			iMovestyle = CMOVES_DEFAULT;	break;
		}
	}
	if ( iRace == CRACE_FLUXXOR ) {
		// Invalid fluxxor marks + fluid type
		if ( iMiscType%10 > 2 ) {
			iMiscType = 0;
		}
	}
}

// Save stats to file
void CRacialStats::SaveToFile ( CBinaryFile& file )
{
	{
		// ====== VERSION INFO ======
		file.WriteUInt32( 0 );
		file.WriteUInt32( 0 );
		file.WriteUInt32( 0 );
		file.WriteUInt32( 1 );

		// General Player Stats
		OSerialBinary serializer ( &file );
		serializer << (*this);

		// Gameplay Point stats
		serializer << (*stats);

		// == Tattoo and Body marking info ==
		file.WriteUShort( iTattooCount );
		for ( ushort i = 0; i < iTattooCount; ++i ) {
			file.WriteData( (char*)&(tattooList[i]), sizeof( tattoo_t ) );
		}
	}
}

void CRacialStats::serialize ( Serializer & ser, const uint ver )
{	
	// ====== PLAYERTYPE STATS ======
	ser & (uint32_t&)iRace;
	ser & (uint32_t&)iGender;

	ser & (short&)iHairstyle;
	ser & (short&)iMovestyle;
	ser & (short&)iTalkstyle;
	ser & (short&)iMiscstyle;

	ser & sPlayerName;
	ser & sLastName;

	// == Player Special Info ==
	ser & (short&)iSpecialType;
	ser & (short&)iBodyType;
	ser & (short&)iFaceType;
	ser & (short&)iMiscType;

	// ====== VANITY STATS ======
	// == Player Colors ==
	ser & cEyeColor;
	ser & cFocusColor;

	ser & cHairColor;
	ser & cSkinColor;

	// == Visual Equipment for Quick Load ==
	ser & sGearHead;
	ser & sGearNecklace;
	ser & sGearShoulder;
	ser & sGearChest;
	ser & sGearRighthand;
	ser & sGearLefthand;
	ser & sGearLegs;
	ser & sGearRightfoot;
	ser & sGearLeftfoot;

	// ====== GAMEPLAY STATS ======
	// Game stats
	//ser & vPlayerInitSpawnPoint;
	//ser & vPlayerCalcSpawnPoint;

	// Point stats
	/*ser & stats->fHealth;
	ser & stats->fHealthMax;

	ser & stats->fStamina;
	ser & stats->fStaminaMax;

	ser & stats->fMana;
	ser & stats->fManaMax;

	// Primary stats
	ser & (uint32_t)stats->iLevel;
	ser & stats->fExperience;

	ser & (unsigned int)stats->iStrength;
	ser & (unsigned int)stats->iAgility;
	ser & (unsigned int)stats->iIntelligence;

	ser & (unsigned int)stats->iOffsetStrength;
	ser & (unsigned int)stats->iOffsetAgility;
	ser & (unsigned int)stats->iOffsetIntelligence;*/

	// Record stats
	/*ser & bHasPlayed;
	ser & fDeathCount;
	ser & fFeetFallen;
	ser & fFeetTravelled;*/
}

// Update stats per-frame
void CRacialStats::UpdateStats ( void )
{
	bool unique;

	// UPDATE SYSTEM NUMBERS
	iTattooCount = (ushort)tattooList.size();

	// DO RACE CODE HERE
	if ( pOwner ) {
		// Add race specific buffs
		if ( iRace == CRACE_ELF ) {
			//pOwner->AddBuffIfUnique( new BuffRacialDarkElf( pOwner ) );
			//pOwner->AddBuffIfUnique( "BuffRacialDarkElf" );
			pOwner->AddBuffIfUnique<BuffRacialDarkElf>(unique);
			//if ( pOwner->BuffExists( BuffRacialDarkElf::BUFF_ID
		}
		else if ( iRace == CRACE_KITTEN ) {
			//pOwner->AddBuffIfUnique( new BuffRacialKitten( pOwner ) );
			//pOwner->AddBuffIfUnique( "BuffRacialKitten" );
			pOwner->AddBuffIfUnique<BuffRacialKitten>(unique);
		}
		else if ( iRace == CRACE_FLUXXOR ) {
			//pOwner->AddBuffIfUnique( "BuffFluxxorManablood" );
			pOwner->AddBuffIfUnique<BuffFluxxorManablood>(unique);
		}
	}

	
	// DO THE STAT CODE HERE AS WELL

	// NEED TO KNOW THE PLAYER'S OFFSET STATS

	// offset start with 1 straight across
}

// Do Level up
void CRacialStats::LevelUp ( void )
{
	// Give skill and stat points
	//iStatPoints		+= 1;
	//iSkillPoints	+= 1;

	// Max out health, stamina, and mana
	stats->fHealth	= stats->fHealthMax;
	stats->fStamina	= stats->fStaminaMax;
	stats->fMana	= stats->fManaMax;

	// Increase stats
	stats->iStrength		+= stats->iOffsetStrength;
	stats->iAgility			+= stats->iOffsetAgility;
	stats->iIntelligence	+= stats->iOffsetIntelligence;

	//stats->iStrength		+= (int)( 4.0f * log( stats->iStrength / 20.0f ) );
	stats->iStrength		+= (int)(stats->iStrength * 0.01f);
	stats->iAgility			+= (int)(stats->iAgility * 0.01f);
	stats->iIntelligence	+= (int)(stats->iIntelligence * 0.01f);

	// Save the player stats right now
	//SaveToFile();
}



// == Vanity Stats ==

// Rerolls colors, but takes race into account
void CRacialStats::RerollColors ( void )
{
	Color skinHue, hairHue, eyeHue;

	switch ( iRace ) {
	case CRACE_HUMAN:
	case CRACE_KITTEN:
	case CRACE_DWARF:
		skinHue.red		= Random.Range( 20.0f,40.0f );
		skinHue.green	= Random.Range( 0.2f,0.8f );	// Humans are always colorful!
		skinHue.blue	= Random.Range( 0.3f,0.8f );

		hairHue.red		= Random.Range( -20.0f,80.0f );
		if ( hairHue.red < 0 ) { hairHue.red += 360; }
		hairHue.green	= Random.Range( 0.3f,0.8f );
		hairHue.blue	= Random.Range( 0.1f,0.9f );

		eyeHue.red		= Random.Range( 0.0f,270.0f );
		eyeHue.green	= Random.Range( 0.5f,1.0f );
		eyeHue.blue		= Random.Range( 0.3f,0.7f );
		break;
	case CRACE_ELF:
		skinHue.red		= Random.Range( 180.0f, 280.0f ); // Nice blue to purple range
		skinHue.green	= Random.Range( 0.0f, 0.3f );
		skinHue.blue	= Random.Range( 0.2f, 0.5f );

		hairHue.red		= Random.Range( -180.0f,30.0f );
		if ( hairHue.red < 0 ) { hairHue.red += 360; }
		hairHue.green	= Random.Range( 0.1f,0.8f );
		hairHue.blue	= Random.Range( 0.3f,0.9f );

		eyeHue.red		= Random.Range( 0.0f,100.0f );
		eyeHue.green	= Random.Range( 0.1f,0.9f );
		eyeHue.blue		= Random.Range( 0.3f,0.7f );
		break;
	case CRACE_FLUXXOR:
		skinHue.red		= Random.Range( 145.0f, 205.0f );
		skinHue.green	= Random.Range( 0.0f, 0.4f );
		skinHue.blue	= Random.Range( 0.35f, 0.6f ); // Darker skin colors

		hairHue.red		= Random.Range( 90.0f,270.0f );
		hairHue.green	= Random.Range( 0.1f,0.9f );
		hairHue.blue	= Random.Range( 0.4f,0.8f );

		eyeHue.red		= Random.Range( 90.0f,270.0f );
		eyeHue.green	= Random.Range( 0.1f,0.9f );
		eyeHue.blue		= Random.Range( 0.5f,0.9f );
		break;
	}

	cSkinColor.SetHSL( skinHue );
	cHairColor.SetHSL( hairHue );
	cEyeColor.SetHSL( eyeHue );

	LimitColors(); // In case we fudged up the range
}
// Limit colors based on the player race
void CRacialStats::LimitColors ( void )
{
	Color skinHue;
	cSkinColor.GetHSLC(skinHue);

	switch ( iRace ) {
	case CRACE_HUMAN:
	case CRACE_KITTEN:
	case CRACE_DWARF:
		if ( skinHue.red > 175 ) {
			skinHue.red = 30.0f;
			skinHue.green = 0.7f;
			skinHue.blue = 0.7f;
		}
		skinHue.red		= Math.Clamp( skinHue.red, 20.0f, 40.0f );
		skinHue.green	= Math.Clamp( skinHue.green, 0.1f,0.8f );
		skinHue.blue	= Math.Clamp( skinHue.blue, 0.3f, 0.9f );
		break;
	case CRACE_ELF:
		if ( skinHue.red < 45 ) {
			skinHue.red = 240.0f;
			skinHue.green = 0.1f;
			skinHue.blue = 0.4f;
		}
		skinHue.red		= Math.Clamp( skinHue.red, 180.0f, 280.0f );
		skinHue.green	= Math.Clamp( skinHue.green, 0.0f,0.5f );
		skinHue.blue	= Math.Clamp( skinHue.blue, 0.2f, 0.8f );
		break;
	case CRACE_FLUXXOR:
		if ( skinHue.red < 45 ) {
			skinHue.red = 165.0f;
			skinHue.green = 0.3f;
			skinHue.blue = 0.4f;
		}
		skinHue.red		= Math.Clamp( skinHue.red, 145.0f, 205.0f );
		skinHue.green	= Math.Clamp( skinHue.green, 0.0f, 0.4f );
		skinHue.blue	= Math.Clamp( skinHue.blue, 0.35f, 0.6f );
		break;
	}

	cSkinColor.SetHSL( skinHue );


	// Set Color Alpha
	cFocusColor.alpha	= 1;
	cEyeColor.alpha		= 1;
	cSkinColor.alpha	= 1;
	cHairColor.alpha	= 1;
}


// Stats copy
void CRacialStats::CopyFrom ( const CRacialStats* n_copy_source )
{
	iRace				= n_copy_source->iRace;
	iGender				= n_copy_source->iGender;

	iHairstyle			= n_copy_source->iHairstyle;
	iMovestyle			= n_copy_source->iMovestyle;
	iTalkstyle			= n_copy_source->iTalkstyle;
	iMiscstyle			= n_copy_source->iMiscstyle;

	sPlayerName			= n_copy_source->sPlayerName;
	sLastName			= n_copy_source->sLastName;

	iSpecialType		= n_copy_source->iSpecialType;
	iBodyType			= n_copy_source->iBodyType;
	iFaceType			= n_copy_source->iFaceType;
	iMiscType			= n_copy_source->iMiscType;

	cEyeColor			= n_copy_source->cEyeColor;
	cFocusColor			= n_copy_source->cFocusColor;

	cHairColor			= n_copy_source->cHairColor;
	cSkinColor			= n_copy_source->cSkinColor;

	sGearHead			= n_copy_source->sGearHead;
	sGearNecklace		= n_copy_source->sGearNecklace;
	sGearShoulder		= n_copy_source->sGearShoulder;
	sGearChest			= n_copy_source->sGearChest;
	sGearRighthand		= n_copy_source->sGearRighthand;
	sGearLefthand		= n_copy_source->sGearLefthand;
	sGearLegs			= n_copy_source->sGearLegs;
	sGearRightfoot		= n_copy_source->sGearRightfoot;
	sGearLeftfoot		= n_copy_source->sGearLeftfoot;

	iTattooCount		= n_copy_source->iTattooCount;
	tattooList			= n_copy_source->tattooList;

	SetLuaDefaults();
}
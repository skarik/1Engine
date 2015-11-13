
#ifndef _C_ITEM_JUNK_BASE_H_
#define _C_ITEM_JUNK_BASE_H_

#include "after/entities/item/CWeaponItem.h"
#include "core-ext/system/io/serializer/CBaseSerializer.h"

class CItemJunkBase : public CWeaponItem
{
	ClassName( "CItemJunkBase" );
	BaseClass( "CItemJunkBase" );
protected:
	struct tJunkStats;
public:
	explicit CItemJunkBase ( tJunkStats&, const WItemData & wdat );
	~CItemJunkBase ( void );
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Serialization
	virtual void serialize ( Serializer &, const uint );

	// Use function
	bool Use ( int x );

	// Update, for check what's going on when flying through the air
	void Update ( void );

protected:
	// Junk's stats. Very important.
	struct tJunkStats
	{
		bool	breaksOnThrow;
		bool	morbid;
		bool	totallyUseless;
		bool	plotRelevant;
		bool	cursed;
		bool	hasFeelings;
		bool	wantsToKillYou;
		bool	notInaminate;
		// Junk constructor
		explicit tJunkStats ( bool nbreaksOnThrow=false, bool nmorbid=false, bool ntotallyUseless=true, bool nplotRelevant=false,
			bool ncursed=false, bool nhasFeelings=false, bool nwantsToKillYou=false, bool nnotInaminate=false ) :
			breaksOnThrow(nbreaksOnThrow), morbid(nmorbid), totallyUseless(ntotallyUseless), plotRelevant(nplotRelevant),
				cursed(ncursed), hasFeelings(nhasFeelings), wantsToKillYou(nwantsToKillYou), notInaminate(nnotInaminate)
		{	// Nothing to see here
			;
		}
	} junk_stats;

	// Check if tossed
	bool bWasTossed;
};

#endif//_C_ITEM_JUNK_BASE_H_
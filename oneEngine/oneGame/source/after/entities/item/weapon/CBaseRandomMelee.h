
#ifndef _C_BASE_RANDOM_MELEE_H_
#define _C_BASE_RANDOM_MELEE_H_

#include "CBaseRandomItem.h"
//#include "ItemBlockPuncher.h"
#include "after/entities/item/system/ItemBlockPuncher.h"
#include "engine-common/types/ItemProperties.h"

class CParticleSystem;

class CBaseRandomMelee : public CBaseRandomItem
{
	ClassName( "RandomBaseMelee" );
	BaseClass( "RandomBaseMelee" );
public:
	struct tMeleeWeaponProperties;

	explicit CBaseRandomMelee ( tMeleeWeaponProperties&, const WItemData & wdat );
	~CBaseRandomMelee ( void );
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Serialization
	virtual void serialize ( Serializer &, const uint );

	// Properties struct
	struct tMeleeWeaponProperties
	{
		// Length of blade check
		ftype	reach;
		// Base damage info
		ftype	damage;
		int		damagetype;
		// Base combat info
		ftype	stagger_chance;
		
		// Min number of hands needed to wield the weapon (1 to 4)
		int		hands;

		// Governs cooldowns and time of swing
		ftype	recover_time;

		// Charge info
		ftype	charge_full_time;
		ftype	charge_min_damage;
		ftype	charge_max_damage;
		ftype	charge_min_stagger;
		ftype	charge_max_stagger;

		// Durability properties
		bool	useable_when_broken;
		short	max_durability;

		// Constructor for default properties
		tMeleeWeaponProperties ( void ) : reach(3.0f), damage(10.0f), damagetype(DamageType::Slash), stagger_chance(0.05f),
			hands(1), recover_time(1.0f),
			charge_full_time(1.0f), charge_min_damage(0.8f), charge_max_damage(1.2f), charge_min_stagger(0.0f), charge_max_stagger(3.0f),
			useable_when_broken(true), max_durability(1000)
		{}
	};
	tMeleeWeaponProperties weapon_stats;
	tMeleeWeaponProperties base_weapon_stats;

	// needs to have a common use that also piggybacks off the ItemBlockPuncher
	//  so does hittrace damage if on actor
	//  if not, then does block damage if elsewhere
	// Use function
	bool Use ( int x )		override;
	// Activating the item has just started. Weapons take the firing type as the argument.
	void StartUse ( int )	override;
	// Activating the item has ended. Weapons take the firing type as the argument.
	void EndUse ( int )		override;
	// Attack function
	void Attack ( XTransform& )	override;
	// Clang Checking
	void ClangCheck ( void );

	
	void	OnUnequip ( CActor* interactingActor ) override;

	// Melee events.
	void	OnBlockAttack ( Damage& resultantDamage ) override;

	// Return stance of the item. Returns StanceDefense when defending.
	Item::EquipStance GetStance ( void ) override;

	// This is called when the HUD for the current weapon needs to be drawn.
	// For this class, draws the current sword swinging angle.
	void OnDrawHUD ( const WReticleStyle& n_reticleStyle ) override;
	// Randomized weapons get a slightly different tooltip that shows their stats.
	void DrawTooltip ( const Vector2d & drawPos, const WTooltipStyle & style ) override;


	void Update ( void ) override;


	void Generate ( void ) override;

	// Collision Callbacks
	void OnCollide ( sCollision& ) override;

protected:
	// Routines for collision detection
	CActor*	FivePointCollision ( void );

	// Routines for block hitting
	void	InitializeBlockDestruction ( void );
	virtual void SetBlockHardness ( void ) { ; };
	bool	DoBlockDestruction ( int x );

private:
	// Info for block hitting
	typedef ItemBlockPuncher::HitPartInfo HitPartInfo;
	std::list<HitPartInfo>	hitList;
	// Has block hardness been initialized? (Is initialized then 'bHardnessSet' set to true on first read)
	bool	bHardnessSet;

	// Info for attack hitting
	std::vector<CGameBehavior*> m_swingHitlist;

	// General attack stats
	bool	bIsCharging;
	ftype	fChargeTime;
	bool	bIsBlocking;
	ftype	fBlockTime;
	bool	bIsDashing;

	// Is Parrying?
	bool	bIsParrying;

	// Quick string info
	bool	bEnableQuickstrike;
	ftype	fQuickstrikeTimer;

	// Stamina the owner was at when attack started
	ftype	fStartAttackStamina;

	// Holding angle
	ftype	fHoldStation;
	// Update holding angle
	void UpdateHoldStation ( void );

	// Sword trail effect
	bool	bShowTrail;
	CParticleSystem*	m_trail_ps;
	void UpdateParticleSystem ( void );

protected:		
	ftype	GetChargeTime ( void ) {
		return fChargeTime;
	}
	
protected:
	// Hardness of the block
	char	cBlockHardness [1024];
	// Hit multiplier. If it's set to 2, then something normally taking 2 hits to destroy now takes 4 hits.
	// This allows to pace low-cooldown weapons easier.
	char	iHitMutliplier;
	// Cooldowns when using
	float	fCooldownOnHit;
	float	fCooldownOnMiss;
	// Range of tool
	float	fMaxToolRange;


protected:
	short GetHitCount ( char hardness );


};

#endif
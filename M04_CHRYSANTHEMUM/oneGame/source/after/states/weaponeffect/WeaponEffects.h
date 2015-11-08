
#ifndef _C_WEAPON_EFFECT_H_
#define _C_WEAPON_EFFECT_H_

// == Includes ==
#include <vector>

#include "engine-common/types/Damage.h"

// == Class Prototypes ==
class CWeaponItem;
class CProjectile;

namespace Debuffs
{
// == Class Prototypes ==
class CWeaponItemBuff;

// == Debuffs Class ==
class WeaponEffects 
{
public:
	//explicit Debuffs ( CCharacter* pParentCharacter ) : m_character( pParentCharacter ) { ; };
	//void UpdateParent ( CCharacter* pParentCharacter, CharacterStats* pTargetStats ) { m_character = pParentCharacter; m_stats = pTargetStats; };
	void UpdateParent ( CWeaponItem* pParentItem ) { m_item = pParentItem; };

	//void UpdatePreStats ( void );
	void Update ( void );
	void ExplicitUpdate ( void );

	Damage OnCreateDamage ( const Damage& dmg );
	void OnCreateProjectile ( CProjectile* new_projectile );
	void OnUseItem ( int usetype );
	void OnDropItem ( void );
	void OnPickupItem ( void );
	void OnEquipItem ( void );
	void OnUnequipItem ( void );

	void Add ( CWeaponItemBuff* newBuff );

private:
	// Typedefs
	typedef CWeaponItemBuff Buff;
	typedef CWeaponItemBuff Buffs;
	// List of buffs
	std::vector<Buffs*>	v_currentBuffs;
	// Pointer to owning character
	CWeaponItem*	m_item;
	//CharacterStats*	m_stats;
};

#ifndef BUFF_ID
#define BUFF_ID(a) public: virtual int GetID ( void ) { return int(a); };
#endif

// == Character Buffs Class ==
class CWeaponItemBuff
{
	BUFF_ID(0);
public:
	enum eEffectType
	{
		TYPE_PASSIVE,
		TYPE_PASSIVE_PLAYER,
		TYPE_ACTIVE
	};

	explicit	CWeaponItemBuff ( eEffectType inType, CWeaponItem* inTarget );
	
	//virtual void PreStatsUpdate ( void );
	//virtual void PostStatsUpdate ( void );
	virtual void Update ( void ) {;};			// After stats have finished initializing.
	//virtual void ExplicitUpdate ( void ) =0;

	virtual Damage OnCreateDamage ( const Damage& dmg ) { return dmg; };
	virtual void OnCreateProjectile ( CProjectile* new_projectile ) {;};
	virtual void OnUseItem ( int usetype ) {;};
	virtual void OnDropItem ( void ) {;};
	virtual void OnPickupItem ( void ) {;};
	virtual void OnEquipItem ( void ) {;};
	virtual void OnUnequipItem ( void ) {;};

	void Delete ( void );

public:
	// Properties
	//bool	can_stack;
	int		buff_id;

	// Used for deletion stuff
	bool	delete_me;

protected:
	// Target character
	CWeaponItem* pTarget;
	// Effect properties
	eEffectType	effect_type;	// Used to speed up application of the effects
};

};

#endif
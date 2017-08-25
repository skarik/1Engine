
#ifndef _C_WEAPONITEM_GENERATOR_H_
#define _C_WEAPONITEM_GENERATOR_H_

#include "after/entities/item/CWeaponItem.h"

class CBinaryFile;

class CWeaponItemGenerator
{
public:
	// ========================
	// Chance for drops
	// ========================
	bool			ChanceDiggingDrops ( void );
	bool			ChanceDesertTripDrops ( void );
	bool			ChanceGrass ( ushort nGrassType );
public:
	// ========================
	// Grouped/Themed Drops
	// ========================
	CWeaponItem*	DropDigging ( const Vector3d&, ushort nBlockType );
	CWeaponItem*	DropDesertTrip ( const Vector3d& );
	CWeaponItem*	DropGrass	( const Vector3d&, ushort nGrassType );

	CWeaponItem*	DropJunk	( const Vector3d& );
public:
	// ========================
	// Debug/Specific Drops
	// ========================
	
	// == Weapons ==
	CWeaponItem*	MakeShittySword ( const Vector3d& );
	CWeaponItem*	MakePickaxeSuShitty ( const Vector3d& );
	CWeaponItem*	MakeShovelSuShitty ( const Vector3d& );
	CWeaponItem*	MakeHoe ( const Vector3d& );

	CWeaponItem*	MakeTestBow ( const Vector3d& );

	// == Tools ==
	CWeaponItem*	MakeBasicDrill ( const Vector3d& );

	// == Throwables ==
	CWeaponItem*	MakeDynamite ( const Vector3d& );

private:
	struct randomItemPartEntry_t
	{
		arstring<64>	type;
		arstring<64>	component;
	};
	struct randomItemInfoEntry_t
	{
		arstring<64>	key;
		arstring<64>	value;
	};
	std::vector<randomItemPartEntry_t> itemParts;
	std::vector<randomItemInfoEntry_t> itemInfo;
	std::vector<arstring<64>>	requiredComponents;
	std::vector<arstring<64>>	optionalComponents;

	CBinaryFile*	LoadInRandomItemFile ( const char* s_filename );
	int				RandomItemChooseComponent ( const char* s_component, const char* s_partial_match );
	arstring<64>&	GetItemInfoValue ( const char* s_key );
	void			LoadInRandomComponentInfo ( CBinaryFile* file, const randomItemPartEntry_t& n_entry );
};

extern CWeaponItemGenerator ItemGenerator;


#endif
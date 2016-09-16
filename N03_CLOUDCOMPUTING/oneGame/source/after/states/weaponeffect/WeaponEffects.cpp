
#include "WeaponEffects.h"
#include "after/entities/item/CWeaponItem.h"
#include "after/entities/projectile/AfterBaseProjectile.h"

namespace Debuffs
{

	// UpdatePreStats()
	//  Runs before player stats have been reset/updated
	//  This is pretty much useless.
	void WeaponEffects::Update ( void )
	{
		// First go through and delete all the buffs marked for deletion
		auto it = v_currentBuffs.begin();
		while ( it != v_currentBuffs.end() )
		{
			if ( (*it)->delete_me )
			{
				delete (*it);
				it = v_currentBuffs.erase( it );
			}
			else
			{
				it++;
			}
		}
		// Run through all the buffs and update them
		for ( auto it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			(*it)->Update();
		}
	}


	// Add( const Buff & )
	//  Adds a buff to the list, performs checks for non-stacking buffs
	void WeaponEffects::Add ( CWeaponItemBuff* newBuff )
	{
		if ( newBuff )
			v_currentBuffs.push_back( newBuff );
		/*if ( newBuff->can_stack )
		{
			v_currentBuffs.push_back( newBuff );
		}
		else
		{
			// Look for all buffs with the identical id, and remove
			vector<Buffs*>::iterator it = v_currentBuffs.begin();
			while ( it != v_currentBuffs.end() )
			{
				if ( (*it)->buff_id == newBuff->buff_id )
				{
					it = v_currentBuffs.erase( it );
				}
				else
				{
					it++;
				}
			}
			// Add new buff
			v_currentBuffs.push_back( newBuff );
		}*/
	}



	CWeaponItemBuff::CWeaponItemBuff ( eEffectType inType, CWeaponItem* inTarget )
		: effect_type( inType ), pTarget( inTarget )
	{
		buff_id = GetID();
	}


};
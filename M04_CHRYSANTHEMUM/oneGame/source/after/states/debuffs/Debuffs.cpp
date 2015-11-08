
#include "Debuffs.h"
#include "after/entities/character/CCharacter.h"
#include "after/states/CharacterStats.h"

#include "DebuffList.h"

#include "core/debug/CDebugConsole.h"

#include "engine-common/entities/CPlayer.h"

namespace Debuffs
{

// Constructor
CCharacterBuff::CCharacterBuff ( eEffectType inType, CCharacter* inTarget )
	: effect_type( inType ), pTarget( inTarget ), can_stack( false ), delete_me( false ), positive(0)
{
	//buff_id = GetID();
	// Check factory for a valid index
	//short targetId = DebuffList.buff_hash[typeid(*this).name()];
	/*if ( weaponItemData.iItemId != targetId ) {
		cout << "Mismatch item ID's: " << weaponItemData.iItemId << " not match table " << targetId << endl;
		if ( targetId != 0 ) {
			cout << "  setting item to ID " << targetId << endl;
			weaponItemData.iItemId = targetId;
		}
		else {
			Debug::Console->PrintError( "Could not find item ID in the table! (Did you forget to add it to the list?)\n" );
		}
	}*/
	/*if ( targetId != 0 ) {
		buff_id = targetId;
	}
	else {
		// TODO
		//Debug::Console->PrintError( "Could not find buff ID in the table! (Did you forget to add it to the list?)\n" );
	}*/
}

// UpdatePreStats()
//  Runs before player stats have been reset/updated
//  This is pretty much useless.
void Debuffs::UpdatePreStats ( void )
{
	// useful for debuffs like "reduce stat growth"

	// First go through and delete all the buffs marked for deletion
	vector<Buffs*>::iterator it = v_currentBuffs.begin();
	while ( it != v_currentBuffs.end() )
	{
		if ( (*it)->delete_me ) {
			delete (*it);
			it = v_currentBuffs.erase( it );
			//Debug::Console->PrintMessage( "removed buff\n" );
		}
		else {
			(*it)->pTarget = m_character;
			it++;
		}
	}

	// Run through all the buffs and update them
	//if ( m_character == CPlayer::GetActivePlayer() ) {
	if ( m_character->ActorType() == ACTOR_TYPE_PLAYER || m_character->ActorType() == ACTOR_TYPE_PLAYER_SIMPLE ) {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			(*it)->PreStatsUpdate();
		}	
	}
	else {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			if (( (*it)->getEffectType() == BaseEffect::TYPE_PASSIVE_PLAYER )||( (*it)->getEffectType() == BaseEffect::TYPE_ACTIVE_PLAYER )) {
				continue;
			}
			(*it)->PreStatsUpdate();
		}	
	}
}
// Update()
//  Runs after player stats have been reset
void Debuffs::Update ( void )
{
	// Run through all the buffs and update them
	if ( m_character->ActorType() == ACTOR_TYPE_PLAYER || m_character->ActorType() == ACTOR_TYPE_PLAYER_SIMPLE ) {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			(*it)->Update();
		}	
	}
	else {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			if (( (*it)->getEffectType() == BaseEffect::TYPE_PASSIVE_PLAYER )||( (*it)->getEffectType() == BaseEffect::TYPE_ACTIVE_PLAYER )) {
				continue;
			}
			(*it)->Update();
		}	
	}
}
// UpdatePostStats()
//  Runs after player stats have been updated
void Debuffs::UpdatePostStats ( void )
{
	// Run through all the buffs and update them
	if ( m_character->ActorType() == ACTOR_TYPE_PLAYER || m_character->ActorType() == ACTOR_TYPE_PLAYER_SIMPLE ) {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			(*it)->PostStatsUpdate();
		}	
	}
	else {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			if (( (*it)->getEffectType() == BaseEffect::TYPE_PASSIVE_PLAYER )||( (*it)->getEffectType() == BaseEffect::TYPE_ACTIVE_PLAYER )) {
				continue;
			}
			(*it)->PostStatsUpdate();
		}	
	}
}


// Add( const Buff * )
//  Adds a buff to the list, performs checks for non-stacking buffs
//void Debuffs::Add ( CCharacterBuff* newBuff )
/*void Debuffs::Add ( const char* newBuffId )
{
	Add( DebuffList.buff_hash[string(newBuffId)] );
}
void Debuffs::Add ( const short newBuffId )
{
	CCharacterBuff* newBuff = DebuffList.buff_inst[newBuffId](m_character);
	newBuff->buff_id = newBuffId;

	if ( newBuff->can_stack )
	{
		// If the buff uses grouped stacks
		if ( newBuff->group_stacks )
		{
			if ( newBuff->use_new_stack )
			{
				// If using new stack, find all previous stacks
				vector<Buffs*>::iterator it = v_currentBuffs.begin();
				while ( it != v_currentBuffs.end() )
				{
					if ( (*it)->buff_id == newBuff->buff_id ) {
						// Add previous stack size to current stack size
						newBuff->stack_size += (*it)->stack_size;
						delete (*it);	// Delete old buff
						it = v_currentBuffs.erase( it );
					}
					else {
						it++;
					}
				}
				// Add new buff to the list
				v_currentBuffs.push_back( newBuff );
				newBuff->Initialize();
			}
			else
			{
				// If using old stack, find the previous stack
				vector<Buffs*>::iterator it = v_currentBuffs.begin();
				while ( it != v_currentBuffs.end() )
				{
					if ( (*it)->buff_id == newBuff->buff_id ) {
						// Add new stack size to old stack size
						(*it)->stack_size += newBuff->stack_size;
						delete newBuff;	// Delete new buff
					}
					else {
						it++;
					}
				}
			}
		}
		else
		{
			// Add a separate instance
			v_currentBuffs.push_back( newBuff );
			newBuff->Initialize();
		}
	}
	else
	{
		if ( newBuff->use_new_stack )
		{
			// Look for all buffs with the identical id, and remove
			vector<Buffs*>::iterator it = v_currentBuffs.begin();
			while ( it != v_currentBuffs.end() )
			{
				if ( (*it)->buff_id == newBuff->buff_id ) {
					delete (*it);
					it = v_currentBuffs.erase( it );
				}
				else {
					it++;
				}
			}
			// Add new buff
			v_currentBuffs.push_back( newBuff );
			newBuff->Initialize();
		}
		else
		{
			// Look for any buff with the identical id.
			bool canAdd = true;
			vector<Buffs*>::iterator it = v_currentBuffs.begin();
			while ( it != v_currentBuffs.end() )
			{
				if ( (*it)->buff_id == newBuff->buff_id ) {
					canAdd = false;
					break;
				}
				else {
					it++;
				}
			}
			if ( canAdd ) {
				// Add new buff
				v_currentBuffs.push_back( newBuff );
				newBuff->Initialize();
			}
			else {
				// Buff already exists, delete this one
				delete newBuff;
			}
		}
	}
}*/

// AddUnique( const Buff * )
//  OLD: Adds a buff to the list, performs checks for non-stacking buffs, and returns true
//  If the buff already exists, it deletes the new buff and return false.
//  CURRENT: Checks the list for a buff of the same ID. If there isn't one, calls Add()
//  with the target buff.
/*bool Debuffs::AddUnique ( const char* newBuff )
{
	return AddUnique( DebuffList.buff_hash[string(newBuff)] );
}
bool Debuffs::AddUnique ( const short newBuff )
{
	bool canAdd = true;
	// Look for all buffs with the identical id, and remove
	vector<Buffs*>::iterator it = v_currentBuffs.begin();
	while ( it != v_currentBuffs.end() )
	{
		if ( (*it)->buff_id == newBuff ) {
			canAdd = false;
			break;
		}
		++it;
	}
	if ( canAdd )
	{
		// Add new buff
		//v_currentBuffs.push_back( newBuff );
		Add( newBuff );
		return true;
	}
	else
	{
		//delete newBuff;
		return false;
	}
}*/

// HasBuff( const Buff & )
//  If a buff with an identical ID exists, returns true.
bool Debuffs::HasBuff ( CCharacterBuff const& buff )
{
	return HasBuff( buff.buff_id );
}

// HasBuff( const Buff & )
//  If a buff with an identical ID exists, returns true.
bool Debuffs::HasBuff ( const int buff_id )
{
	vector<Buffs*>::iterator it = v_currentBuffs.begin();
	while ( it != v_currentBuffs.end() )
	{
		if ( (*it)->buff_id == buff_id ) {
			return true;
		}
		++it;
	}
	return false;
}

// OnDamaged ( const Damage& )
//  When the owning actor is damaged, the damage is run through here first.
Damage Debuffs::OnDamaged ( const Damage& in_dmg )
{
	Damage result = in_dmg;
	if ( m_character->ActorType() == ACTOR_TYPE_PLAYER || m_character->ActorType() == ACTOR_TYPE_PLAYER_SIMPLE ) {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			result = (*it)->OnDamaged( result );
		}	
	}
	else {
		for ( vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
		{
			if (( (*it)->getEffectType() == BaseEffect::TYPE_PASSIVE_PLAYER )||( (*it)->getEffectType() == BaseEffect::TYPE_ACTIVE_PLAYER )) {
				continue;
			}
			result = (*it)->OnDamaged( result );
		}	
	}
	return result;
}

Experience Debuffs::OnGainExperience ( const Experience& in_exp )
{
	Experience result = in_exp;
	return result;
}

};
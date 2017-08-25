
#ifndef _DEBUFF_LIST_H_
#define _DEBUFF_LIST_H_

#include "Debuffs.h"

/*
#include <typeinfo>
#include <string>
using std::string;

#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif

#include "Debuffs.h"

namespace Debuffs
{

	template <class BuffType, short Value>
	int GetBuffId ( void )
	{
		return Value;
	}

	class CDebuffList
	{
	public:
		CDebuffList ( void ) { CreateDebuffList(); }

		void CreateDebuffList ( void );

		unordered_map<string,short>	buff_hash;
		unordered_map<short,string>	buff_type;
		unordered_map<string,short>	buff_swap;
		//unordered_map<short,CCharacterBuff*(*)(CCharacter*)>	buff_inst;

		// Debuff: on creation, looks at debuff list for ID.
		// On save, looks at debuff list for ID.
		// On load, looks at ID to create debuff.

		#define RegisterBuff(A,ID) \
			buff_hash[#A] = ID ; \
			buff_type[ ID ] = #A; \
			buff_swap[typeid(A).name()] = ID ;
			//template Debuffs::GetBuffId<
			//buff_inst[ ID ] = &_creatDebuff<A>;
		//#end define

	};

	extern CDebuffList DebuffList;

	// ============================================================================================
	// Buff instantiation
	// ============================================================================================
	// Add( )
	//  Adds a buff to the list, performs checks for non-stacking buffs
	// Returns created debuff
	template <class BuffType>
	BaseEffect* Debuffs::Add ( void )
	{
		//CCharacterBuff* newBuff = DebuffList.buff_inst[newBuffId](m_character);
		//CCharacterBuff* newBuff = _creatDebuff<BuffType>( m_character );
		CCharacterBuff* newBuff = new BuffType ( m_character );
		CCharacterBuff* resultBuff = newBuff;
		newBuff->buff_id = DebuffList.buff_swap[typeid(BuffType).name()];

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
							resultBuff = *it; // Return the existing buff
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
		else // Cannot stack?
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
						delete newBuff; // Delete new buff
						resultBuff = *it; // Return the existing buff
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
			}
		}
		// Return result
		return resultBuff;
	}


	// AddUnique( )
	//  Checks the list for a buff of the same ID. If there isn't one, calls Add()
	//  with the target buff. Returns pointer to the new debuff
	template <class BuffType>
	BaseEffect* AddUnique ( bool& unique )
	{
		// Look for all buffs with the identical id. If it has an identical ID, then return that one.
		vector<Buffs*>::iterator it = v_currentBuffs.begin();
		while ( it != v_currentBuffs.end() )
		{
			if ( (*it)->buff_id == newBuff ) {
				// Found the same buff, so return it instead. Not unique.
				unique = false;
				return *it;
			}
			++it;
		}
		// Add new buff
		unique = true;
		return Add<BuffType>();
	}
};
*/
#endif//_DEBUFF_LIST_H_
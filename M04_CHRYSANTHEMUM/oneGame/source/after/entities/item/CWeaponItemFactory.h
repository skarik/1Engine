
#ifndef _C_WEAPON_ITEM_FACTORY_H_
#define _C_WEAPON_ITEM_FACTORY_H_

class CWeaponItem;

#include "core/common.h"
#include "core/types/types.h"

#include "skill/CSkillReference.h"

#include <typeinfo>
#include <string>
using std::type_info;
using std::string;

#include <map>
#include <unordered_map>

namespace WeaponItem
{
	template<typename T> CWeaponItem * _instWeaponItem( void ) { return new T; }

	class CWeaponItemFactory
	{
	public:
		std::map<string,short>	witem_hash;
		std::map<short,string>	witem_list;
		std::map<short,string>	witem_name;
		std::map<string,short>	witem_swap;
		std::map<short,CWeaponItem*(*)(void)>	witem_inst;
		std::map<short,CSkillReferenceBase*>	skill_refs;
	private:
		void RegisterTypes ( void );
		void RegisterSkillTypes ( void );

#define RegisterWItem(A) \
	incrementId(); \
	witem_hash[typeid( A ).name()] = _id; \
	witem_list[_id] = typeid( A ).name(); \
	witem_name[_id] = #A; \
	witem_swap[#A] = _id; \
	witem_inst[_id] = &_instWeaponItem<A>;
#define RegisterWItemEx(A,ID) \
	witem_hash[typeid(A).name()] = ID ; \
	witem_list[ ID ] = typeid(A).name(); \
	witem_name[ ID ] = #A; \
	witem_swap[#A] = ID ; \
	witem_inst[ ID ] = &_instWeaponItem<A>;
#define RegisterSkill(A,ID) \
	witem_hash[typeid(A).name()] = ID ; \
	witem_list[ ID ] = typeid(A).name(); \
	witem_name[ ID ] = #A; \
	witem_swap[#A] = ID ; \
	witem_inst[ ID ] = &_instWeaponItem<A>; \
	skill_refs[ ID ] = new CSkillReference<A,ID>();

		short _id;
		void incrementId() {
			++_id;
			while ( witem_list.find( _id ) != witem_list.end() ) {
				++_id;
			}
		}
	public:
		CWeaponItemFactory ( void )
		{
			_id = 0;
			RegisterTypes();
			RegisterSkillTypes();
		}
		~CWeaponItemFactory ( void )
		{
			
		}
	};

	extern CWeaponItemFactory WeaponItemFactory;
}

#endif//_C_WEAPON_ITEM_FACTORY_H_

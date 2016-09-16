
// ============================================================================================
// Debuff System
// ============================================================================================
#ifndef _C_ACTOR_EFFECT_H_
#define _C_ACTOR_EFFECT_H_

// == Includes ==
#include <vector>

#include "engine-common/types/Damage.h"
#include "after/types/Experience.h"

// == Class Prototypes ==
class CCharacter;
class CharacterStats;
// Include saving and loading
class CBaseSerializer;
typedef CBaseSerializer Serializer;

namespace Debuffs
{
	using std::vector;

	// == Class Prototypes
	class CCharacterBuff;

	// == Debuffs Class ==
	class Debuffs 
	{
	public:
		// Typedefs
		typedef CCharacterBuff Buff;
		typedef CCharacterBuff Buffs;
	public:
		//explicit Debuffs ( CCharacter* pParentCharacter ) : m_character( pParentCharacter ) { ; };
		void UpdateParent ( CCharacter* pParentCharacter, CharacterStats* pTargetStats ) { m_character = pParentCharacter; m_stats = pTargetStats; };

		void UpdatePreStats ( void );
		void Update ( void );
		void UpdatePostStats ( void );
		void ExplicitUpdate ( void );

		Damage OnDamaged ( const Damage& );
		Damage OnDealDamage ( const Damage& );

		Experience OnGainExperience ( const Experience& );

		//void Add ( CCharacterBuff* newBuff );
		//bool AddUnique ( CCharacterBuff* newBuff );
		//void Add ( CCharacterBuff* newBuff );
		//bool AddUnique ( CCharacterBuff* newBuff );
		//void Add ( const char* newBuffId );
		//bool AddUnique ( const char* newBuff );
		//void Add ( const short newBuffId );
		//bool AddUnique ( const short newBuff );
		template <class BuffType>
		Buff* Add ( void );
		template <class BuffType>
		Buff* AddUnique ( bool& unique );


		bool HasBuff ( CCharacterBuff const& buff );
		bool HasBuff ( const int buff_id );

		const vector<Buffs*>* List ( void ) {
			return &v_currentBuffs;
		}
	private:
		// List of buffs
		vector<Buffs*>	v_currentBuffs;
		// Pointer to owning character
		CCharacter*		m_character;
		CharacterStats*	m_stats;

		friend CCharacterBuff;
	};
	/*
	#ifndef BUFF_ID
	#define BUFF_ID(a) public: virtual int GetID ( void ) { return int(a); };
	#endif
	*/
	// == Character Buffs Class ==
	class CCharacterBuff
	{
		friend Debuffs;
		//BUFF_ID( 0 );
	public:
		// TODO: Declare operator "new" and "delete" to use special area of memory

		enum eEffectType
		{
			TYPE_PASSIVE,
			TYPE_PASSIVE_PLAYER, // Only works if target character is the player
			TYPE_ACTIVE,
			TYPE_ACTIVE_PLAYER // Only works if target character is the player
		};

		explicit CCharacterBuff ( eEffectType inType, CCharacter* inTarget );
		virtual ~CCharacterBuff ( void ) {};

		// Use for initialization of nifty
		virtual void Initialize ( void ) {};
		// Use for deinitialization
		//virtual void Deinitialize ( void ) {};
	
		// Before stats have been touched
		virtual void PreStatsUpdate ( void ) {};
		// After stats have finished initializing/reset.
		virtual void Update ( void ) {};		
		// After stats have been updated
		virtual void PostStatsUpdate ( void ) {};	

		// Not sure yet
		virtual void ExplicitUpdate ( void ) {};

		// On dealt damage
		virtual Damage OnDamaged ( const Damage& dmg ) { return dmg; };
		// On dealing damage
		virtual Damage OnDealDamage ( const Damage& dmg ) { return dmg; };

		// Get effect type
		eEffectType getEffectType ( void ) { return effect_type; };

	public:
		// Properties
		int		buff_id;	// Used for ID purposes

		bool	can_stack;		// If buff can stack
		bool	group_stacks;	// If buff should be grouped in one stack
		bool	use_new_stack;	// If buff needs to replace old one, hould buff use new instance or old instance

		int		positive;	// Is this buff a positive or negative buff? 0 for negative, 1 for positive, 2 for neutral

		// State
		int		stack_size;
		bool	delete_me;
	protected:
		// Remove self
		void Delete ( void ) { delete_me = true; }

		// Target character
		CCharacter* pTarget;
		// Effect type
		eEffectType effect_type;
	};

	// Type typedefs.
	typedef CCharacterBuff::eEffectType eEffectType;
	typedef CCharacterBuff BaseEffect;

};

#endif

// ============================================================================================
// Debuff List
// ============================================================================================
#ifndef _DEBUFF_LIST__FF_H_
#define _DEBUFF_LIST__FF_H_

#include <typeinfo>
#include <string>

#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif

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

		unordered_map<std::string,short>	buff_hash;
		unordered_map<short,std::string>	buff_type;
		unordered_map<std::string,short>	buff_swap;
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
	BaseEffect* Debuffs::AddUnique ( bool& unique )
	{
		int newBuff = DebuffList.buff_swap[typeid(BuffType).name()];
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

#endif//_DEBUFF_LIST__FF_H_
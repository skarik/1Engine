
#ifndef _C_PROJEkkCTILE_EFFECT_H_
#define _C_PROJEkkCTILE_EFFECT_H_

// == Includes ==
#include <vector>

#include "core/math/Vector3d.h"
#include "engine-common/types/Damage.h"

// == Class Prototypes ==
class CCharacter;
class CProjectile;

namespace Debuffs
{

	// == Class Prototypes ==
	class CProjectileBuff;

	// == Debuffs Class ==
	class ProjectileEffects 
	{
	public:
				ProjectileEffects ( void );
				~ProjectileEffects ( void );
		//explicit Debuffs ( CCharacter* pParentCharacter ) : m_character( pParentCharacter ) { ; };
		//void UpdateParent ( CCharacter* pParentCharacter, CharacterStats* pTargetStats ) { m_character = pParentCharacter; m_stats = pTargetStats; };
		void UpdateParent ( CProjectile* pParentProjectile ) { m_projectile = pParentProjectile; };

		//void UpdatePreStats ( void );
		void Update ( void );
		void ExplicitUpdate ( void );

		Damage OnDealDamage ( const Damage& );
		bool OnHitWorld ( const Vector3d& hitPosition, const Vector3d& hitNormal );
		bool OnHitEnemy ( CActor* enemy );
		bool OnHitAlly ( CActor* ally );

		void Add ( CProjectileBuff* newBuff );

	private:
		// Typedefs
		typedef CProjectileBuff Buff;
		typedef CProjectileBuff Buffs;
		// List of buffs
		std::vector<Buffs*>	v_currentBuffs;
		// Pointer to owning character
		CProjectile*	m_projectile;
		//CharacterStats*	m_stats;
	};

	#ifndef BUFF_ID
	#define BUFF_ID(a) public: virtual int GetID ( void ) { return int(a); };
	#endif

	// == Character Buffs Class ==
	class CProjectileBuff
	{
		friend ProjectileEffects;
	public:
		explicit CProjectileBuff ( void );
		virtual ~CProjectileBuff ( void ) {}
	
		virtual void Initialize ( void ) {}

		//virtual void PreStatsUpdate ( void );
		//virtual void PostStatsUpdate ( void );
		virtual void Update ( void ) {}		// Before projectile has actually performed movement
		//virtual void ExplicitUpdate ( void ) =0;

		//virtual Damage OnDamaged ( const Damage& dmg ) { return dmg; };
		//virtual Damage OnDealDamage ( const Damage& dmg ) { return dmg; };
		virtual Damage OnDealDamage ( const Damage& dmg ) { return dmg; }
		virtual bool OnHitWorld ( const Vector3d& hitPosition, const Vector3d& hitNormal ) { return true; }
		virtual bool OnHitEnemy ( CActor* enemy ) { return true; }
		virtual bool OnHitAlly ( CActor* ally ) { return true; }

		void Delete ( void );

	public:
		// Properties
		bool	can_stack;
		int		buff_id;	// Only used when can_stack is false

		// Used for deletion stuff
		bool	delete_me;

	protected:
		// Target character
		CProjectile* pTarget;

		//
	};

};

#endif
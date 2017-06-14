
#include "ProjectileEffects.h"
//#include "CCharacter.h"
//#include "engine/game/item/CWeaponItem.h"
//#include "CProjectile.h"


Debuffs::ProjectileEffects::ProjectileEffects ( void )
{
	;
}
Debuffs::ProjectileEffects::~ProjectileEffects ( void )
{
	for ( uint i = 0; i < v_currentBuffs.size(); ++i ) {
		delete v_currentBuffs[i];
	}
}

// UpdatePreStats()
//  Runs before player stats have been reset/updated
//  This is pretty much useless.
void Debuffs::ProjectileEffects::Update ( void )
{
	// First go through and delete all the buffs marked for deletion
	std::vector<Buffs*>::iterator it = v_currentBuffs.begin();
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
	for ( std::vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it )
	{
		(*it)->Update();
	}
}

Damage Debuffs::ProjectileEffects::OnDealDamage ( const Damage& inDamage )
{
	Damage damage = inDamage;
	for ( std::vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it ) {
		damage = (*it)->OnDealDamage( damage );
	}
	return damage;
}
bool Debuffs::ProjectileEffects::OnHitWorld ( const Vector3d& hitPosition, const Vector3d& hitNormal )
{
	bool bDestroy = true;
	for ( std::vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it ) {
		bDestroy = bDestroy && (*it)->OnHitWorld( hitPosition, hitNormal );
	}
	return bDestroy;
}
bool Debuffs::ProjectileEffects::OnHitEnemy ( CActor* enemy )
{
	bool bDestroy = true;
	for ( std::vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it ) {
		bDestroy = bDestroy && (*it)->OnHitEnemy( enemy );
	}
	return bDestroy;
}
bool Debuffs::ProjectileEffects::OnHitAlly ( CActor* ally )
{
	bool bDestroy = true;
	for ( std::vector<Buffs*>::iterator it = v_currentBuffs.begin(); it != v_currentBuffs.end(); ++it ) {
		bDestroy = bDestroy && (*it)->OnHitAlly( ally );
	}
	return bDestroy;
}

// Add( const Buff & )
//  Adds a buff to the list, performs checks for non-stacking buffs
void Debuffs::ProjectileEffects::Add ( CProjectileBuff* newBuff )
{
	if ( newBuff ) {
		newBuff->pTarget = m_projectile;
		v_currentBuffs.push_back( newBuff );
		newBuff->Initialize();
	}
}


// Effect contrustor
Debuffs::CProjectileBuff::CProjectileBuff ( void )
{
	delete_me = false;
	can_stack = false;
	buff_id = 0;
}

#ifndef _PROJECTILE_MAGICAL_BASE_H_
#define _PROJECTILE_MAGICAL_BASE_H_

#include "AfterBaseProjectile.h"
#include <list>

class ProjectileMagicalBase : public AfterBaseProjectile
{
public:
	explicit ProjectileMagicalBase( Ray const& rnInRay, ftype fnInSpeed, ftype fnWidth=0.2f )
		: AfterBaseProjectile( rnInRay, fnInSpeed, fnWidth )
	{
		m_sisters.push_back( this );
	}

	~ProjectileMagicalBase ( void )
	{
		auto sisterPosition = std::find( m_sisters.begin(), m_sisters.end(), this );
		m_sisters.erase( sisterPosition );
	}

protected:
	std::list<ProjectileMagicalBase*>		m_sisters;

};

#endif//_PROJECTILE_MAGICAL_BASE_H_
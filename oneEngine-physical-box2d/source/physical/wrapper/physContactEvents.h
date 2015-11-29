
#ifndef _PHYS_WRAPPER_CONTACT_EVENTS_H_
#define _PHYS_WRAPPER_CONTACT_EVENTS_H_

#include "wrapper_common.h"
#include "physVector4.h"
#include "physMotion.h"

class physContactPointEvent 
{
public:
	class physContactPoint_t
	{
	public:
		physVector4 getPosition ( void )
		{
			return m_position;
		}
		physVector4 getNormal ( void )
		{
			return m_normal;
		}

		physVector4 m_position;
		physVector4 m_normal;
	}* m_contactPoint;

	PHYS_API physRigidBody* getBody ( int index );
	PHYS_API const physRigidBody* getBody ( int index ) const;

	b2Contact* m_contactInfo;
};

class physContactListener
{
public:
	virtual void contactPointCallback ( const physContactPointEvent& m_event ) = 0;
};

#endif//_PHYS_WRAPPER_CONTACT_EVENTS_H_

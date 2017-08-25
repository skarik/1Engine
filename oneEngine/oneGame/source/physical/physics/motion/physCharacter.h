
#ifndef _PHYS_CHARACTER_H_
#define _PHYS_CHARACTER_H_

#include "physRigidbody.h"

#define _FORCE_INLINE_

class physCharacter : public physRigidBody 
{
public:
	_FORCE_INLINE_ PHYS_API explicit		physCharacter( hkpCharacterRigidBodyCinfo* );

	_FORCE_INLINE_ PHYS_API void			setLinearVelocity ( const Vector3d& newVel, const Real32 timestep );
	_FORCE_INLINE_ PHYS_API Vector3d		getLinearVelocity ( void );
protected:
	//hkpCharacterRigidBody* controller;
};


#endif//_PHYS_CHARACTER_H_
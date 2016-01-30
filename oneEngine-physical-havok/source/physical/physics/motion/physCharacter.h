
#ifndef _PHYS_CHARACTER_H_
#define _PHYS_CHARACTER_H_

#include "physRigidbody.h"

class physCharacterRigidBodyInfo;

#define _FORCE_INLINE_

class physCharacter : public physRigidBody 
{
public:
	_FORCE_INLINE_ PHYS_API explicit		physCharacter( physCharacterRigidBodyInfo* );

	_FORCE_INLINE_ PHYS_API void			setLinearVelocity ( const Vector3d& newVel, const Real_32 timestep );
	_FORCE_INLINE_ PHYS_API const Vector3d	getLinearVelocity ( void ) const override;

	_FORCE_INLINE_ PHYS_API void			setPosition ( const Vector3d& ) override;
	_FORCE_INLINE_ PHYS_API const Vector3d	getPosition ( void ) const override;
protected:
	hkpCharacterRigidBody* controller;
};


#endif//_PHYS_CHARACTER_H_
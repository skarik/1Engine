

#ifndef _PL_HUD3D_HURT_01_H_
#define _PL_HUD3D_HURT_01_H_

#include "engine/behavior/CGameBehavior.h"
#include "core/math/Rotator.h"

class CModel;

class Plhud3d_Hurt01 : public CGameBehavior
{
public:
	// Constructor
	explicit		Plhud3d_Hurt01 ( const Real n_hurtValue, const Rotator n_rotation );
	// Destructor
					~Plhud3d_Hurt01 ( void );
					
	// Update for values
	void			Update ( void );
	// Lateupdate for positioning
	void			LateUpdate ( void );

protected:
	Real			m_hurtValue;
	CModel*			m_model;
};

#endif//_PL_HUD3D_HURT_01_H_

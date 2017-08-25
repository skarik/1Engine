
#ifndef _C_CYLINDER_COLLIDER_
#define _C_CYLINDER_COLLIDER_

#include "engine/physics/collider/CCollider.h"

class CCylinderCollider : public CCollider
{
public:
	// Constructor and Destructor
	//CCapsuleCollider ( Vector3d const& size, Real radius );
	CCylinderCollider ( Real height, Real radius, bool centered=false );
	~CCylinderCollider ( void );

	// Setters
	void SetSize ( Real height, Real radius );
	void SetHeight ( Real height );
	// Getters
	Vector3d GetSize ( void );
	Real GetRadius ( void );

protected:
	Vector3d vSize;
	Real fRadius;
};

#endif
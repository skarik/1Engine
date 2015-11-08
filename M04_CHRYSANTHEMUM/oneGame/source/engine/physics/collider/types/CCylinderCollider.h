
#ifndef _C_CYLINDER_COLLIDER_
#define _C_CYLINDER_COLLIDER_

#include "engine/physics/collider/CCollider.h"

class CCylinderCollider : public CCollider
{
public:
	// Constructor and Destructor
	//CCapsuleCollider ( Vector3d const& size, ftype radius );
	CCylinderCollider ( ftype height, ftype radius, bool centered=false );
	~CCylinderCollider ( void );

	// Setters
	void SetSize ( ftype height, ftype radius );
	void SetHeight ( ftype height );
	// Getters
	Vector3d GetSize ( void );
	ftype GetRadius ( void );

protected:
	Vector3d vSize;
	ftype fRadius;
};

#endif
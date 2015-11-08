
#ifndef _C_CAPSULE_COLLIDER_
#define _C_CAPSULE_COLLIDER_

// Includes
#include "engine/physics/collider/CCollider.h"

class CCapsuleCollider : public CCollider
{
public:
	// Constructor and Destructor
	//CCapsuleCollider ( Vector3d const& size, ftype radius );
	ENGINE_API CCapsuleCollider ( ftype height, ftype radius, bool centered=false );
	ENGINE_API ~CCapsuleCollider ( void );

	// Setters
	ENGINE_API void SetSize ( ftype height, ftype radius );
	ENGINE_API void SetHeight ( ftype height );
	ENGINE_API void SetFootOffset ( ftype foot );
	// Getters
	ENGINE_API Vector3d GetSize ( void );
	ENGINE_API ftype GetRadius ( void );
	ENGINE_API ftype GetFootOffset ( void );

protected:
	Vector3d vSize;
	ftype fRadius;
	ftype fFootOffset;
};

#endif
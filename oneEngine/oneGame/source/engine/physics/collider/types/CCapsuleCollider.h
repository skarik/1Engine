
#ifndef _C_CAPSULE_COLLIDER_
#define _C_CAPSULE_COLLIDER_

// Includes
#include "engine/physics/collider/CCollider.h"

class CCapsuleCollider : public CCollider
{
public:
	// Constructor and Destructor
	//CCapsuleCollider ( Vector3d const& size, Real radius );
	ENGINE_API CCapsuleCollider ( Real height, Real radius, bool centered=false );
	ENGINE_API ~CCapsuleCollider ( void );

	// Setters
	ENGINE_API void SetSize ( Real height, Real radius );
	ENGINE_API void SetHeight ( Real height );
	ENGINE_API void SetFootOffset ( Real foot );
	// Getters
	ENGINE_API Vector3d GetSize ( void );
	ENGINE_API Real GetRadius ( void );
	ENGINE_API Real GetFootOffset ( void );

protected:
	Vector3d vSize;
	Real fRadius;
	Real fFootOffset;
};

#endif
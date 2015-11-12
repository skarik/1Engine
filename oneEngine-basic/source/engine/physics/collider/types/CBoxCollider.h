
#ifndef _C_BOX_COLLIDER_H_
#define _C_BOX_COLLIDER_H_

// Includes
#include "core/math/BoundingBox.h"
#include "engine/physics/collider/CCollider.h"

// Class Definition
class CBoxCollider : public CCollider
{
public:
	// Constructor and Destructor
	ENGINE_API CBoxCollider ( Vector3d const& size );
	ENGINE_API CBoxCollider ( BoundingBox const& bbox, Vector3d const& pos );
	ENGINE_API ~CBoxCollider ( void );

	// Setters
	ENGINE_API void SetSize ( Vector3d const& size );
	ENGINE_API void Set ( BoundingBox const& bbox, Vector3d const& pos );
	// Getters
	ENGINE_API Vector3d GetSize ( void );

protected:
	Vector3d vExtents;

};

#endif
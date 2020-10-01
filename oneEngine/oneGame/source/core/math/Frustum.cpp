#include "core/math/Math3d.h"
#include "core/math/BoundingBox.h"
#include "core/math/Frustum.h"

core::math::arShapeCheckResult
core::math::Frustum::PointIsInside ( const Vector3f& point )
{
	// various distances
	float fDistance;

	// calculate our distances to each of the planes
	for ( char i = 0; i < 6; i += 1 )
	{
		// find the distance to this plane
		fDistance = plane[i].n.dot( point ) + plane[i].d;

		// if this distance is < -sphere.radius, we are outside
		if ( fDistance < 0 )
			return kShapeCheckResultOutside;
	}

	return kShapeCheckResultInside;
}

core::math::arShapeCheckResult
core::math::Frustum::SphereIsInside ( const Vector3f& center, const Real radius )
{
	// various distances
	float fDistance;

	// calculate our distances to each of the planes
	for ( int i = 0; i < 6; ++i )
	{
		// find the distance to this plane
		fDistance = plane[i].n.dot( center ) + plane[i].d;

		// if this distance is < -sphere.radius, we are outside
		if ( fDistance < -radius )
			return kShapeCheckResultOutside;

		// else if the distance is between +- radius, then we intersect
		if( (float)fabs(fDistance) < radius )
			return kShapeCheckResultIntersect;
	}

	// otherwise we are fully in view
	return kShapeCheckResultInside;
}

core::math::arShapeCheckResult
core::math::Frustum::BoundingBoxIsInside ( const BoundingBox& box )
{
	bool bboxBehind;
	// calculate our collisions to each of the planes
	for ( int i = 0; i < 6; ++i )
	{
		// find the distance to this plane
		bboxBehind = box.BoxOutsidePlane( plane[i] );

		if ( bboxBehind )
			return kShapeCheckResultOutside;
	}
	return kShapeCheckResultIntersect;
}

core::math::Frustum core::math::Frustum::BuildFromProjectionMatrix ( const Matrix4x4& viewprojMatrix )
{
	const Real* projection = viewprojMatrix.pData;

	Frustum frustum;

	// Left
	frustum.plane[0].n.x = projection[3] + projection[0];
	frustum.plane[0].n.y = projection[7] + projection[4];
	frustum.plane[0].n.z = projection[11] + projection[8];
	frustum.plane[0].d = projection[15] + projection[12];

	// Right
	frustum.plane[1].n.x = projection[3] - projection[0];
	frustum.plane[1].n.y = projection[7] - projection[4];
	frustum.plane[1].n.z = projection[11] - projection[8];
	frustum.plane[1].d = projection[15] - projection[12];

	// Bottom
	frustum.plane[2].n.x = projection[3] + projection[1];
	frustum.plane[2].n.y = projection[7] + projection[5];
	frustum.plane[2].n.z = projection[11] + projection[9];
	frustum.plane[2].d = projection[15] + projection[13];

	// Top
	frustum.plane[3].n.x = projection[3] - projection[1];
	frustum.plane[3].n.y = projection[7] - projection[5];
	frustum.plane[3].n.z = projection[11] - projection[9];
	frustum.plane[3].d = projection[15] - projection[13];

	// Near
	frustum.plane[4].n.x = projection[3] + projection[2];
	frustum.plane[4].n.y = projection[7] + projection[6];
	frustum.plane[4].n.z = projection[11] + projection[10];
	frustum.plane[4].d = projection[15] + projection[14];

	// Far
	frustum.plane[5].n.x = projection[3] - projection[2];
	frustum.plane[5].n.y = projection[7] - projection[6];
	frustum.plane[5].n.z = projection[11] - projection[10];
	frustum.plane[5].d = projection[15] - projection[14];

	return frustum;
}
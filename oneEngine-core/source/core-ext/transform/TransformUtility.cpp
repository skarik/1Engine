#include "TransformUtility.h"

void Core::TransformUtility::WorldToLocal(const Matrix4x4 & parent_inv, const Rotator & parent_rot_inv, const Vector3d & parent_scale, const Vector3d & position, const Rotator & rotation, const Vector3d & scale, Vector3d & local_position, Rotator & local_rotation, Vector3d & local_scale)
{
	// Convert position,rotation,scaling into local coordinates
	{
		// Update local position by moving the translation into local space
		local_position = parent_inv * position;
		// Just take the new rotation in local space
		local_rotation = parent_rot_inv * rotation;
		// Just take the new scale, I guess
		local_scale = scale.divComponents(parent_scale);
	}
}

void Core::TransformUtility::TRSToMatrix4x4(const Vector3d & position, const Rotator & rotation, const Vector3d & scale, Matrix4x4 & transform, Matrix4x4 & transform_rot)
{
	// Generate the component matrices (this can be optimized)
	Matrix4x4 transMatrix;
	transMatrix.setTranslation( position );
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( rotation );
	Matrix4x4 scalMatrix;
	scalMatrix.setScale( scale );
	// Set the local transforms
	transform = transMatrix*((rotMatrix)*scalMatrix);
	transform_rot = rotMatrix;
}

void Core::TransformUtility::LocalToWorld(const int32_t * parents, const XTransform * local, TransformLite * result, const int count)
{
	for ( int i = 0; i < count; ++i )
	{
		result[i].local.position	= local[i].position;
		result[i].local.rotation	= local[i].rotation;
		result[i].local.scale		= local[i].scale;
		if ( parents[i] >= 0 )
			result[i].UpdateWorldFromLocal(&result[i]);
		else
			result[i].UpdateWorldFromLocal(NULL);
	}
}

CORE_API void Core::TransformUtility::WorldToLocal(const int32_t * parents, const XTransform * local, TransformLite * result, const int count)
{
	for ( int i = 0; i < count; ++i )
	{
		result[i].world.position	= local[i].position;
		result[i].world.rotation	= local[i].rotation;
		result[i].world.scale		= local[i].scale;
		if ( parents[i] >= 0 )
			result[i].UpdateLocalFromWorld(&result[i]);
		else
			result[i].UpdateLocalFromWorld(NULL);
	}
}



void Core::TransformLite::UpdateWorldFromLocal(const TransformLite * parent)
{
	if ( parent != NULL )
	{
		// Generate a local transforms
		Core::TransformUtility::TRSToMatrix4x4( local, matxLocal, matxLocalRot );
		// Create the global transforms
		matx	= parent->matx * matxLocal;
		matxRot	= parent->matxRot * matxLocalRot;
		// Get back the values
		world.position	= matx.getTranslation();
		world.scale		= local.scale.mulComponents(parent->world.scale);
		world.rotation	= matxRot.getRotator();
	}
	else
	{
		world = local;
	}
}

void Core::TransformLite::UpdateLocalFromWorld(const TransformLite * parent)
{
	if ( parent != NULL )
	{
		// Call the world to local transformation utility
		Core::TransformUtility::WorldToLocal(
			parent->matx.inverse(), parent->matxRot.inverse().getRotator(), parent->world.scale,
			world.position, world.rotation, world.scale,
			local.position, local.rotation, local.scale );
	}
	else
	{
		local = world;
	}
}

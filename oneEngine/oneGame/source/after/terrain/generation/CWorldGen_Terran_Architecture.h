
#ifndef _C_WORLD_GEN_TERRAIN_ARCHITECTURE_WORLDTYPES_H_
#define _C_WORLD_GEN_TERRAIN_ARCHITECTURE_WORLDTYPES_H_

#include "core/math/Vector3d.h"
#include "core/math/Vector2d.h"
#include "core/math/vect3d_template.h"
#include "core/math/vect2d_template.h"
#include "core/math/noise/BaseNoise.h"
#include "core/types/ModelData.h"
#include "after/types/WorldVector.h"
#include "after/types/terrain/BlockData.h"

namespace GeneratorUtilities
{
	Vector3d NearestPointOnTri(Vector3d pt, Vector3d a, Vector3d b, Vector3d c);
	int GetClosestTriangle ( const CModelData* md, const Vector3d_d& position );
	int GetClosestVertex ( const CModelData* md, const Vector3d_d& point );
	void Barycentric_ ( const Vector3d& p, const Vector3d& a, const Vector3d& b, const Vector3d& c, ftype &u, ftype &v, ftype &w);
	void GetClosestMeshInfo( const CModelData* md, const Vector3d_d& point, Vector3d& o_pos, Vector3d& o_norm );
	Terrain::terra_b SampleModelVolume ( const CModelData* md, const Vector3d_d& point );
}


#endif//_C_WORLD_GEN_TERRAIN_ARCHITECTURE_WORLDTYPES_H_
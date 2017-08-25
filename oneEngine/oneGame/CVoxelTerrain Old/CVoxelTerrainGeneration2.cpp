#include "CVoxelTerrain.h"
#include "CTerrainGenerator.h"

/// == Freaking Pubs ==
char	CVoxelTerrain::TerraGen_pub_GetType	( Vector3d const& blockPosition )
{
	return generator->TerraGen_pub_GetType( blockPosition );
}
char	CVoxelTerrain::TerraGen_pub_GetBiome( Vector3d const& blockPosition )
{
	return generator->TerraGen_pub_GetBiome( blockPosition ); 
}
ftype	CVoxelTerrain::TerraGen_pub_GetElevation( Vector3d const& blockPosition )
{
	return generator->TerraGen_pub_GetElevation( blockPosition ); 
}
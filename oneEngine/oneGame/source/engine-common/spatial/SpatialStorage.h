#ifndef ENGINE_COMMON_SPATIAL_STORAGE_H_
#define ENGINE_COMMON_SPATIAL_STORAGE_H_

#include "core/types/types.h"
#include "core/math/BoundingBox.h"
#include <vector>

namespace engine
{
	struct SpatialEntity
	{
		void* ent = nullptr;
	};

	class SpatialSector
	{
	public:
		// @brief Area that this group represents
		std::vector<core::math::BoundingBox> area;
		// @brief All the contents within the sector
		std::vector<SpatialEntity> contents;
		// TODO: tree structure of all the items within this sector. Tree of subsectors?


		core::math::BoundingBox wide_area; // Generated from input sector's area.

	public:
		void GenerateCachedInfo ( void );
		bool Contains ( const Vector3f& point );
	};

	struct SpatialSectorLevel
	{
		int32_t	lod = 0;
		std::vector<SpatialSector> sectors;
	};

	class SpatialStorage
	{
	public:
		std::vector<SpatialSectorLevel> lods;
	public:
		// @brief Adds given sector to the listing. If the LOD doesnt exist, adds the LOD.
		void AddSector ( int32_t lod, const SpatialSector&& sector );

		// @brief Returns the sector list of the given LOD.
		const SpatialSectorLevel* GetLod ( int32_t lod ) const;
	};
}

#endif//ENGINE_COMMON_SPATIAL_STORAGE_H_
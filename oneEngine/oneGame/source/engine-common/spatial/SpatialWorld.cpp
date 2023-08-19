#include "SpatialStorage.h"
#include "SpatialWorld.h"

#include <algorithm>


void engine::SpatialSector::GenerateCachedInfo ( void )
{
	ARCORE_ASSERT(area.size() > 0);
	Matrix4x4 translationMatrix;
	translationMatrix.setTranslation(area[0].m_M.getTranslation());
	wide_area = core::math::BoundingBox(translationMatrix, Vector3f::zero);
	for (auto& bbox : area)
	{
		wide_area.Expand(bbox);
	}
}

bool engine::SpatialSector::Contains ( const Vector3f& point )
{
	if (wide_area.IsPointInBox(point))
	{
		for (auto& bbox : area)
		{
			if (bbox.IsPointInBox(point))
			{
				return true;
			}
		}
	}
	return false;
}


void engine::SpatialStorage::AddSector ( int32_t lod, const SpatialSector&& sector )
{
	auto existing_list = std::find_if(lods.begin(), lods.end(), [lod](SpatialSectorLevel& level){ return lod == level.lod; });
	if (existing_list == lods.end())
	{
		existing_list = lods.insert(lods.begin(), SpatialSectorLevel());
	}
	existing_list->sectors.push_back(sector);
}

const engine::SpatialSectorLevel* engine::SpatialStorage::GetLod ( int32_t lod ) const
{
	auto existing_list = std::find_if(lods.begin(), lods.end(), [lod](const SpatialSectorLevel& level){ return lod == level.lod; });
	if (existing_list == lods.end())
	{
		return nullptr;
	}
	return &*existing_list;
}


void engine::SpatialWorld::SetPlayerPosition ( Vector3f& nextPlayerPosition )
{
	// If moved more than one unit, do sector checks again.
	if (nextPlayerPosition != playerPosition)
	{
		std::vector<SpatialSector*> previousSectors = currentSectors;

		// Exit sectors first:
		for (auto iterSector = currentSectors.begin(); iterSector != currentSectors.end(); )
		{
			SpatialSector* sector = *iterSector;
			ARCORE_ASSERT(sector != nullptr);
			if (sector != nullptr)
			{
				if (sector->Contains(nextPlayerPosition))
				{
					OnLodExit(*sector);
					iterSector = currentSectors.erase(iterSector);
					continue;
				}
			}
			++iterSector;
		}

		// Enter sectors next:
		for (auto& lod : sectorLods.lods)
		{
			for (SpatialSector& sector : lod.sectors)
			{
				// Sector isn't in the current sectors list?
				if (std::find(previousSectors.begin(), previousSectors.end(), &sector) == previousSectors.end())
				{
					if (sector.Contains(nextPlayerPosition))
					{
						OnLodEnter(sector);
						currentSectors.push_back(&sector);
					}
				}
			}
		}

		// Update the position
		playerPosition = nextPlayerPosition;
	}
}

void engine::SpatialWorld::OnLodEnter ( SpatialSector& sector )
{
	for (SpatialEntity& ent : sector.contents)
	{
		OnLodEnter(ent);
	}
}

void engine::SpatialWorld::OnLodExit ( SpatialSector& sector )
{
	for (SpatialEntity& ent : sector.contents)
	{
		OnLodExit(ent);
	}
}

void engine::SpatialWorld::OnLodEnter ( SpatialEntity& ent )
{
	// @todo
}

void engine::SpatialWorld::OnLodExit ( SpatialEntity& ent )
{
	// @todo
}
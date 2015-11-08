
#include "COctreeIO.h"
#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"
#include "core/system/io/CMappedBinaryFile.h"
#include "core/system/io/CBufferIO.h"

#include "after/types/terrain/IOPropData.h"

#include "after/entities/props/CTerrainProp.h"
#include "after/entities/props/CTerrainPropFactory.h"
#include "after/entities/foliage/CFoliage.h"
#include "after/entities/foliage/CTerraFoliageFactory.h"

#include "after/terrain/VoxelTerrain.h"

/*
#include "COctreeTerrain.h"
#include "TerrainIOStorage.h"
#include "CTerrainQueuedObjectLoader.h"
#include "CTerrainProp.h"
#include "CFoliage.h"
#include "sTerraGrass.h"

#include "CTerrainPropFactory.h"
#include "CTerraFoliageFactory.h"*/

#include <algorithm>

void Terrain::COctreeIO::WriteSectorGameData ( GamePayload* n_sector, CBufferIO* ofile, const RangeVector& n_sector_id )
{
	// First work on simplifing and catching errors
	std::unique( n_sector->grass.begin(), n_sector->grass.end() );

	// Count the amount of data to edit
	uint32_t bigDataCount, smallDataCount;
	bigDataCount = n_sector->props.size() + n_sector->foliage.size() + n_sector->prop_queue.size() + n_sector->foliage_queue.size();
	smallDataCount = n_sector->grass.size();

	// Write BIG data
	ofile->WriteUInt32( bigDataCount );
	if ( bigDataCount > 0 )
	{
		Terrain::propdata_big_t* propdata_big = new Terrain::propdata_big_t[ bigDataCount ];

		uint32_t k = 0;
		for ( unsigned int i = 0; i < n_sector->foliage.size(); i++ )
		{
			propdata_big[k].foliage.type = Terrain::PROPTYPE_FOLIAGE;
			propdata_big[k].foliage.foliage_index = TerraFoliage::GetFoliageType( n_sector->foliage[i] );
			propdata_big[k].foliage.x = n_sector->foliage[i]->transform.position.x;
			propdata_big[k].foliage.y = n_sector->foliage[i]->transform.position.y;
			propdata_big[k].foliage.z = n_sector->foliage[i]->transform.position.z;
			char data [48];
			n_sector->foliage[i]->GetToggle( data );
			memcpy( propdata_big[k].foliage.branch_data, data, 48 );

			++k;
		}
		for ( unsigned int i = 0; i < n_sector->foliage_queue.size(); i++ )
		{
			propdata_big[k].foliage.type = Terrain::PROPTYPE_FOLIAGE;
			propdata_big[k].foliage.foliage_index = n_sector->foliage_queue[i].foliage_index;
			propdata_big[k].foliage.x = n_sector->foliage_queue[i].position.x;
			propdata_big[k].foliage.y = n_sector->foliage_queue[i].position.y;
			propdata_big[k].foliage.z = n_sector->foliage_queue[i].position.z;
			memcpy( propdata_big[k].foliage.branch_data, n_sector->foliage_queue[i].userdata, 48 );

			++k;
		}
		for ( unsigned int i = 0; i < n_sector->props.size(); i++ )
		{
			propdata_big[k].component.type = Terrain::PROPTYPE_COMPONENT;
			propdata_big[k].component.component_index = PropFactory.GetComponentType( n_sector->props[i] );
			propdata_big[k].component.x = n_sector->props[i]->transform.position.x;
			propdata_big[k].component.y = n_sector->props[i]->transform.position.y;
			propdata_big[k].component.z = n_sector->props[i]->transform.position.z;
			Vector3d angles = n_sector->props[i]->transform.rotation.getEulerAngles();
			propdata_big[k].component.xrot = angles.x;
			propdata_big[k].component.yrot = angles.y;
			propdata_big[k].component.zrot = angles.z;
			propdata_big[k].component.xscal = n_sector->props[i]->transform.scale.x;
			propdata_big[k].component.yscal = n_sector->props[i]->transform.scale.y;
			propdata_big[k].component.zscal = n_sector->props[i]->transform.scale.z;
			propdata_big[k].component.userdata = n_sector->props[i]->GetUserdata();
			propdata_big[k].component.block_x = n_sector->props[i]->GetBlockInfo().pos_x;
			propdata_big[k].component.block_y = n_sector->props[i]->GetBlockInfo().pos_y;
			propdata_big[k].component.block_z = n_sector->props[i]->GetBlockInfo().pos_z;
			propdata_big[k].component.block_w = n_sector->props[i]->GetBlockInfo().bindex;

			++k;
		}
		for ( unsigned int i = 0; i < n_sector->prop_queue.size(); i++ )
		{
			propdata_big[k].component.type = Terrain::PROPTYPE_COMPONENT;
			propdata_big[k].component.component_index = n_sector->prop_queue[i].component_index;
			propdata_big[k].component.x = n_sector->prop_queue[i].position.x;
			propdata_big[k].component.y = n_sector->prop_queue[i].position.y;
			propdata_big[k].component.z = n_sector->prop_queue[i].position.z;
			propdata_big[k].component.xrot = n_sector->prop_queue[i].rotation.x;
			propdata_big[k].component.yrot = n_sector->prop_queue[i].rotation.y;
			propdata_big[k].component.zrot = n_sector->prop_queue[i].rotation.z;
			propdata_big[k].component.xscal = n_sector->prop_queue[i].scaling.x;
			propdata_big[k].component.yscal = n_sector->prop_queue[i].scaling.y;
			propdata_big[k].component.zscal = n_sector->prop_queue[i].scaling.z;
			propdata_big[k].component.userdata = n_sector->prop_queue[i].userdata;
			propdata_big[k].component.block_x = n_sector->prop_queue[i].block_pos_x;
			propdata_big[k].component.block_y = n_sector->prop_queue[i].block_pos_y;
			propdata_big[k].component.block_z = n_sector->prop_queue[i].block_pos_z;
			propdata_big[k].component.block_w = 0;

			++k;
		}
		// Write data
		ofile->WriteData( (char*)propdata_big, sizeof(propdata_big_t)*bigDataCount ); 

		delete [] propdata_big;
	}
	
	// Write SML data
	ofile->WriteUInt32( smallDataCount );
	if ( smallDataCount > 0 )
	{
		Terrain::propdata_small_t* propdata_small = new Terrain::propdata_small_t[ smallDataCount ];

		uint32_t k = 0;
		for ( unsigned int i = 0; i < n_sector->grass.size(); i++ )
		{
			propdata_small[k].grass.type = Terrain::PROPTYPE_GRASS;
			propdata_small[k].grass.grass_type = n_sector->grass[i].type;
			propdata_small[k].grass.offsetx = n_sector->grass[i].position.x;
			propdata_small[k].grass.offsety = n_sector->grass[i].position.y;
			propdata_small[k].grass.offsetz = n_sector->grass[i].position.z;
			//throw std::exception();

			++k;
		}
		// Write data
		ofile->WriteData( (char*)propdata_small, sizeof(propdata_small_t)*smallDataCount ); 

		delete [] propdata_small;
	}
}


// Reads subsector prop data
void Terrain::COctreeIO::ReadSectorGameData ( GamePayload* o_sector, CBufferIO* ofile, const uint32_t n_max_subdivide, const RangeVector& n_sector_id )
{
	uint32_t bigDataCount, smallDataCount;
	// Read in big data
	bigDataCount = ofile->ReadUInt32();
	//cout << "bigDataCount: " << bigDataCount << endl;
	if ( bigDataCount > 0 )
	{
		Terrain::propdata_big_t* propdata_big = new Terrain::propdata_big_t[ bigDataCount ];
		// Read into buffer
		ofile->ReadData( (char*)propdata_big, sizeof(propdata_big_t)*bigDataCount ); 

		// Loop through data and add to queue list
		if ( !o_sector->loaded_macro ) // may need to remove this. just have the loader control the big/little loader
		{
			uint32_t lockIndex;
			Terrain::GamePayload* lockedLoad = m_terrain->AquireGamePayload( n_sector_id, lockIndex );

			for ( uint32_t i = 0; i < bigDataCount; ++i )
			{
				/*if ( propdata_big[i].i_type == Terrain::PROPTYPE_FOLIAGE )
				{
					Terrain::queuerequest_Foliage request;
					request.foliage_index = propdata_big[i].foliage.foliage_index;
					request.position.x = propdata_big[i].foliage.x;
					request.position.y = propdata_big[i].foliage.y;
					request.position.z = propdata_big[i].foliage.z;
					memcpy( request.userdata, propdata_big[i].foliage.branch_data, 48 );
					request.target = o_sector;
					request.index = n_sector_id;

					m_terrain->m_queueloader->LoadFoliage( o_sector, n_sector_id, request );
				}
				else if ( propdata_big[i].i_type == Terrain::PROPTYPE_COMPONENT )
				{
					Terrain::queuerequest_Component request;
					//throw std::exception();
					request.component_index = propdata_big[i].component.component_index;
					request.position.x = propdata_big[i].component.x;
					request.position.y = propdata_big[i].component.y;
					request.position.z = propdata_big[i].component.z;
					request.rotation.x = propdata_big[i].component.xrot;
					request.rotation.y = propdata_big[i].component.yrot;
					request.rotation.z = propdata_big[i].component.zrot;
					request.scale.x = propdata_big[i].component.xscal;
					request.scale.y = propdata_big[i].component.yscal;
					request.scale.z = propdata_big[i].component.zscal;
					request.userdata = propdata_big[i].component.userdata;

					request.target = o_sector;
					request.index = n_sector_id;

					request.block_x = propdata_big[i].component.block_x;
					request.block_y = propdata_big[i].component.block_y;
					request.block_z = propdata_big[i].component.block_z;
					request.block_w = propdata_big[i].component.block_w;

					m_terrain->m_queueloader->LoadComponent( o_sector, n_sector_id, request );
				}
				else
				{
					throw std::exception();
				}*/
				//m_terrain->m_queueloader->LoadPropData( o_sector, n_sector_id, propdata_big[i] );
				if ( propdata_big[i].i_type == Terrain::PROPTYPE_FOLIAGE )
				{
					Terrain::foliage_t request;
					request.foliage_index = propdata_big[i].foliage.foliage_index;
					request.position.x = propdata_big[i].foliage.x;
					request.position.y = propdata_big[i].foliage.y;
					request.position.z = propdata_big[i].foliage.z;
					memcpy( request.userdata, propdata_big[i].foliage.branch_data, 48 );

					if ( lockedLoad ) {
						lockedLoad->foliage_queue.push_back( request );
					}
				}
				else if ( propdata_big[i].i_type == Terrain::PROPTYPE_COMPONENT )
				{
					Terrain::prop_t request;
					//throw std::exception();
					request.component_index = propdata_big[i].component.component_index;
					request.position.x = propdata_big[i].component.x;
					request.position.y = propdata_big[i].component.y;
					request.position.z = propdata_big[i].component.z;
					request.rotation.x = propdata_big[i].component.xrot;
					request.rotation.y = propdata_big[i].component.yrot;
					request.rotation.z = propdata_big[i].component.zrot;
					request.scaling.x = propdata_big[i].component.xscal;
					request.scaling.y = propdata_big[i].component.yscal;
					request.scaling.z = propdata_big[i].component.zscal;
					request.userdata = propdata_big[i].component.userdata;

					request.block_pos_x = propdata_big[i].component.block_x;
					request.block_pos_y = propdata_big[i].component.block_y;
					request.block_pos_z = propdata_big[i].component.block_z;
					//request.component_index = propdata_big[i].component.block_w;

					if ( lockedLoad ) {
						lockedLoad->prop_queue.push_back( request );
					}
				}
				else
				{
					//throw std::exception();
				}
			}

			m_terrain->ReleaseGamePayload( lockedLoad, lockIndex );
		}

		delete [] propdata_big;
	}

	o_sector->loaded_macro = true;

	// Don't read in small data if an LoD
	if ( n_max_subdivide > 2 ) {
		return;
	}

	// Read in small data if LoD permits
	smallDataCount = ofile->ReadUInt32();
	//cout << "smallDataCount: " << smallDataCount << endl;
	if ( smallDataCount > 0 )
	{
		Terrain::propdata_small_t* propdata_small = new Terrain::propdata_small_t[ smallDataCount ];
		// Read into buffer
		ofile->ReadData( (char*)propdata_small, sizeof(propdata_small_t)*smallDataCount ); 

		// Loop through data and add to queue list
		if ( !o_sector->loaded_small )
		{
			std::cout << "LOADING: " << (void*)o_sector << " " << n_sector_id.x << "," << n_sector_id.y << "," << n_sector_id.z << std::endl;

			uint32_t lockIndex;
			Terrain::GamePayload* lockedLoad = m_terrain->AquireGamePayload( n_sector_id, lockIndex );

			for ( uint32_t i = 0; i < smallDataCount; ++i )
			{
				/*Terrain::queuerequest_Grass request;
				request.grass_type = propdata_small[i].grass.grass_type;
				request.x = propdata_small[i].grass.offsetx;
				request.y = propdata_small[i].grass.offsety;
				request.z = propdata_small[i].grass.offsetz;
				request.target = o_sector;*/
				Terrain::grass_t request;
				request.type = propdata_small[i].grass.grass_type;
				request.position.x = propdata_small[i].grass.offsetx;
				request.position.y = propdata_small[i].grass.offsety;
				request.position.z = propdata_small[i].grass.offsetz;

				//m_terrain->m_queueloader->LoadGrass( o_sector, request );
				if ( lockedLoad ) {
					lockedLoad->grass.push_back( request );
				}
			}
			m_terrain->ReleaseGamePayload( lockedLoad, lockIndex );
		}

		delete [] propdata_small;
	}

	o_sector->loaded_small = true;
}
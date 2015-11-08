
#include "CRegionGenerator.h"

#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/io/COctreeIO.h"
#include "after/terrain/generation/CWorldGen_Terran.h"

using namespace Terrain;
using namespace World;

void CRegionGenerator::IO_Start ( void )
{
	m_io_next_freeblock = 0;
	m_io_usecount = 0;
	m_io = NULL;

	IO_SaveOpen();
}

void CRegionGenerator::IO_End ( void )
{
	if ( m_io )
	{
		if ( m_io->IsOpen() ) {
			m_io->Close();
		}
		delete m_io;
		m_io = NULL;
	}
}

void CRegionGenerator::IO_SaveOpen ( void )
{
	// Lock the function
	m_io_lock_file.lock();
	// Now in citical section
	if ( m_io == NULL )
	{	// No IO? Create it.
		m_io = new CBinaryFile();
	}
	if ( m_io != NULL )
	{
		char stemp_fn[256];
		sprintf( stemp_fn, "%s.regions/map", m_terrain->IO->GetSavefile() );
		// Open the file
		if ( !m_io->IsOpen() )
		{
			if ( IO::FileExists( stemp_fn ) )
			{	// If the file exists, read in ID of next region.
				m_io->Open( stemp_fn, CBinaryFile::IO_READ | CBinaryFile::IO_WRITE );
				m_gen_nextregion	= m_io->ReadUInt32();
				m_io_next_freeblock	= m_io->ReadUInt32();
			}
			else
			{	// If the file doesn't exist, set next region to 1. Save that value.
				m_io->Open( stemp_fn, CBinaryFile::IO_READ | CBinaryFile::IO_WRITE );
				m_gen_nextregion = 2;
				m_io->WriteUInt32( m_gen_nextregion );
				m_io->WriteUInt32( m_io_next_freeblock );
			}
		}
		// Save the file
		{
			m_io->Close(); // This is how the region file is saved.
			m_io->Open( stemp_fn, CBinaryFile::IO_READ | CBinaryFile::IO_WRITE );
		}
	}
	// Out of critical IO section
	m_io_lock_file.unlock();
}

void CRegionGenerator::IO_Open ( void )
{
	// Lock the function
	m_io_lock_file.lock();
	// Now in citical section
	if ( m_io == NULL )
	{	// No IO? Create it.
		m_io = new CBinaryFile();
	}
	if ( m_io != NULL )
	{
		char stemp_fn[256];
		sprintf( stemp_fn, "%s.regions/map", m_terrain->IO->GetSavefile() );
		// Open the file
		if ( !m_io->IsOpen() )
		{
			if ( IO::FileExists( stemp_fn ) )
			{	// If the file exists, read in ID of next region.
				m_io->Open( stemp_fn, CBinaryFile::IO_READ | CBinaryFile::IO_WRITE );
				m_gen_nextregion	= m_io->ReadUInt32();
				m_io_next_freeblock	= m_io->ReadUInt32();
			}
			else
			{	// If the file doesn't exist, set next region to 1. Save that value.
				m_io->Open( stemp_fn, CBinaryFile::IO_READ | CBinaryFile::IO_WRITE );
				m_gen_nextregion = 2;
				m_io->WriteUInt32( m_gen_nextregion );
				m_io->WriteUInt32( m_io_next_freeblock );
			}
		}
	}
	// Out of critical IO section
	m_io_lock_file.unlock();
}

void CRegionGenerator::IO_Save ( void )
{
	// Lock the function
	m_io_lock_file.lock();
	// Now in citical section
	if ( m_io != NULL )
	{
		char stemp_fn[256];
		sprintf( stemp_fn, "%s.regions/map", m_terrain->IO->GetSavefile() );
		// Save the file
		{
			m_io->Close(); // This is how the region file is saved.
			m_io->Open( stemp_fn, CBinaryFile::IO_READ | CBinaryFile::IO_WRITE );
		}
	}
	// Out of critical IO section
	m_io_lock_file.unlock();
}

void CRegionGenerator::IO_UpdateNext ( void )
{
	// Lock the function
	m_io_lock_file.lock();
	// Now in citical section
	{
		// Update the region index
		fseek( m_io->GetFILE(), 0, SEEK_SET );
		m_io->WriteUInt32( m_gen_nextregion );
		m_io->WriteUInt32( m_io_next_freeblock );
	}
	// Out of critical IO section
	m_io_lock_file.unlock();
}

void CRegionGenerator::IO_SetRegion ( const regioninfo_t& n_index_data )
{
	io_nodeset_t nodeset;
	uint32_t t_currentblock = 0;
	io_intermediate_t t_findResult;
	while ( true )
	{
		IO_ReadBlock( t_currentblock, &nodeset );
		uint i;
		// Loop through the nodes to find the first value that is larger (or equal to) the input
		for ( i = 0; i < nodeset.count; ++i )
		{
			//if ( n_index_data.position.CompareXY(nodeset.data[i].position) ) {
			if ( n_index_data.position < nodeset.data[i].position ) {
				break; // Break on ( n_index_data.position.xyz < nodeset.data[i].position.xyz )
			}
		}
		// Look at that node to see where to jump to
		if ( i > 0 )
		{
			// If the node is equal, then this is the result we want
			if ( n_index_data.position == nodeset.data[i-1].position  )
			{
				// Edit this result and write back the nodeset
				nodeset.data[i-1].region = n_index_data.region;
				IO_WriteBlock( t_currentblock, &nodeset );
				break;
			}
			// Value not found, push in a new values at this position (we're larger than i-1)
			else if ( nodeset.count < 12 )
			{	// Push values back
				for ( int k = 11; k > i; --k )
				{
					nodeset.data[k] = nodeset.data[k-1];
					nodeset.blocks[k+1] = nodeset.blocks[k];
				}
				nodeset.blocks[i+1] = 0;
				nodeset.data[i] = n_index_data;
				nodeset.count += 1;
				IO_WriteBlock( t_currentblock, &nodeset );
				break;
			}
		}
		else if ( i == 0 )
		{
			// Push in a new value at the front
			if ( nodeset.count < 12 )
			{	// Push values back
				for ( int k = 11; k > 0; --k )
				{
					nodeset.data[k] = nodeset.data[k-1];
					nodeset.blocks[k+1] = nodeset.blocks[k];
				}
				nodeset.blocks[i+1] = 0;
				nodeset.data[i] = n_index_data;
				nodeset.count += 1;
				IO_WriteBlock( t_currentblock, &nodeset );
				break;
			}
		}
		// Otherwise, either the input value is larger than everything in this block or there wasn't a match

		// If we can't follow the pointer, then add a new data block
		if ( nodeset.blocks[i] == 0 )
		{
			// Create empty data set
			io_nodeset_t empty_nodeset;
			memset( &empty_nodeset, 0, sizeof(io_nodeset_t) );
			// Increment data block
			m_io_next_freeblock += 1;
			nodeset.blocks[i] = m_io_next_freeblock;
			IO_WriteBlock( t_currentblock, &nodeset );
			// Write the empty data set
			IO_WriteBlock( nodeset.blocks[i], &empty_nodeset );
		}

		// Follow the pointer
		t_currentblock = nodeset.blocks[i];
		continue;
	}
}

void CRegionGenerator::IO_FindIndex ( const RangeVector& n_index, io_intermediate_t& o_findResult )
{
	io_nodeset_t nodeset;
	uint32_t t_currentblock = 0;
	while ( true )
	{
		IO_ReadBlock( t_currentblock, &nodeset );
		uint i;
		// Loop through the nodes to find the first value that is larger (or equal to) the input
		for ( i = 0; i < nodeset.count; ++i )
		{
			if ( n_index.CompareXY(nodeset.data[i].position) ) {
				break; // Break on ( n_index.xy < nodeset.data[i].position.xy )
			}
		}
		// Look at that node to see where to jump to
		if ( i > 0 )
		{
			// If the node is equal, then this is the result we want
			if ( n_index.x == nodeset.data[i-1].position.x && n_index.y == nodeset.data[i-1].position.y )
			{
				o_findResult.data.region = nodeset.data[i-1].region;
				o_findResult.block = t_currentblock;
				o_findResult.block_indexer = i-1;
				break;
			}
		}
		// Otherwise, either the input value is larger than everything in this block or there wasn't a match

		// Follow the pointer if we can
		if ( nodeset.blocks[i] != 0 )
		{
			t_currentblock = nodeset.blocks[i];
			continue;
		}
		// If we can't follow the pointer, then return cannot find
		else
		{
			o_findResult.data.region = REGION_NONE;
			o_findResult.block = t_currentblock;
			o_findResult.block_indexer = i;
			break;
		}
	}
}


void CRegionGenerator::IO_ReadBlock ( const uint32_t n_blockindex, io_nodeset_t* o_data )
{
	m_io_lock_file.lock();
	uint64_t t_seekpos = 8 + 256*n_blockindex;
	fseek( m_io->GetFILE(), t_seekpos, SEEK_SET );
	uint64_t t_actual_seekpos = ftell( m_io->GetFILE() );
	if ( t_seekpos != t_actual_seekpos )
	{
		memset( o_data, 0, sizeof(io_nodeset_t) );
	}
	else
	{
		memset( o_data, 0, sizeof(io_nodeset_t) );
		fread( o_data, sizeof(io_nodeset_t), 1, m_io->GetFILE() );
	}
	m_io_lock_file.unlock();
}

void CRegionGenerator::IO_WriteBlock( const uint32_t n_blockindex, const io_nodeset_t* n_data )
{
	m_io_lock_file.lock();
	uint64_t t_seekpos = 8 + 256*n_blockindex;
	fseek( m_io->GetFILE(), t_seekpos, SEEK_SET );
	uint64_t t_actual_seekpos = ftell( m_io->GetFILE() );
	if ( t_seekpos != t_actual_seekpos )
	{
		fseek( m_io->GetFILE(), 0, SEEK_END );
		t_actual_seekpos = ftell( m_io->GetFILE() );
		for ( uint64_t i = 0; i < t_seekpos - t_actual_seekpos; ++i ) {
			fputc( 0, m_io->GetFILE() );
		}
	}
	fwrite( n_data, sizeof(io_nodeset_t), 1, m_io->GetFILE() );
	for ( uint64_t i = 0; i < 256 - sizeof(io_nodeset_t); ++i ) {
		fputc( 0, m_io->GetFILE() );
	}
	m_io_lock_file.unlock();
}


void CRegionGenerator::IO_RegionSaveInfo ( const uint32_t n_region, const regionproperties_t* n_data )
{
	CBinaryFile region_file;
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.regions/region_%d", m_terrain->IO->GetSavefile(), n_region );
	region_file.OpenWait( stemp_fn, CBinaryFile::IO_WRITE | CBinaryFile::IO_READ );
	region_file.WriteData( (char*)n_data, sizeof(regionproperties_t) );
	region_file.Close();
}

void CRegionGenerator::IO_RegionLoadInfo ( const uint32_t n_region, regionproperties_t* n_data )
{
	CBinaryFile region_file;
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.regions/region_%d", m_terrain->IO->GetSavefile(), n_region );
	region_file.OpenWait( stemp_fn, CBinaryFile::IO_READ );
	region_file.ReadData( (char*)n_data, sizeof(regionproperties_t) );
	region_file.Close();
}
bool CRegionGenerator::IO_RegionGetSetHasTowns ( const uint32_t n_region )
{
	CBinaryFile region_file;
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.regions/region_%d", m_terrain->IO->GetSavefile(), n_region );
	region_file.OpenWait( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
	{
		regionproperties_t region;
		region_file.ReadData( (char*)&region, sizeof(regionproperties_t) );
		// Do a locked read/write
		if ( region.flags.has_towns == 0 )
		{
			// Has no towns? Mark has towns.
			region.flags.has_towns = 1;
			fseek( region_file.GetFILE(), 0, SEEK_SET );
			// Write out new data.
			region_file.WriteData( (char*)&region, sizeof(regionproperties_t) );
			region_file.Close();
			return true; // Return that it was able to set
		}
	}
	region_file.Close();
	return false;
}

void	CRegionGenerator::IO_RegionSaveSectors ( const uint32_t n_region, const std::vector<RangeVector>& n_sectors )
{
	CBinaryFile region_file;
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.regions/region_%d", m_terrain->IO->GetSavefile(), n_region );
	region_file.OpenWait( stemp_fn, CBinaryFile::IO_WRITE | CBinaryFile::IO_READ );

	// First seek to the 1024 point
	fseek( region_file.GetFILE(), 1024, SEEK_SET );
	// If the seek didn't work right
	if ( ftell( region_file.GetFILE() ) != 1024 )
	{	// Pad out so the fucking seek works
		fseek( region_file.GetFILE(), SEEK_END, 0 );
		long currentpos = ftell( region_file.GetFILE() );
		for ( int i = 0; i < 1028 - currentpos; ++i )
		{
			putc( 0, region_file.GetFILE() );
		}
		fseek( region_file.GetFILE(), 1024, SEEK_SET );
	}

	// Write out sector count
	region_file.WriteUInt32( n_sectors.size() );

	// Write out sectors
	for ( uint i = 0; i < n_sectors.size(); ++i )
	{
		region_file.WriteInt32( n_sectors[i].x );
		region_file.WriteInt32( n_sectors[i].y );
		region_file.WriteInt32( n_sectors[i].z );
	}

	region_file.Close();
}

void	CRegionGenerator::IO_RegionLoadSectors ( const uint32_t n_region, std::vector<RangeVector>& n_sectors )
{
	CBinaryFile region_file;
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.regions/region_%d", m_terrain->IO->GetSavefile(), n_region );
	region_file.OpenWait( stemp_fn, CBinaryFile::IO_READ );

	// First seek to the 1024 point
	fseek( region_file.GetFILE(), 1024, SEEK_SET );
	// If the seek didn't work right
	if ( ftell( region_file.GetFILE() ) != 1024 )
	{	// Pad out so the fucking seek works
		fseek( region_file.GetFILE(), SEEK_END, 0 );
		long currentpos = ftell( region_file.GetFILE() );
		for ( int i = 0; i < 1028 - currentpos; ++i )
		{
			putc( 0, region_file.GetFILE() );
		}
		fseek( region_file.GetFILE(), 1024, SEEK_SET );
	}

	// Read in sector count
	uint32_t t_sectorCount = region_file.ReadUInt32();

	// Read in sectors
	n_sectors.clear();
	RangeVector t_nextSet;
	for ( uint32_t i = 0; i < t_sectorCount; ++i )
	{
		t_nextSet.x = region_file.ReadInt32();
		t_nextSet.y = region_file.ReadInt32();
		t_nextSet.z = region_file.ReadInt32();
		n_sectors.push_back( t_nextSet );
	}

	region_file.Close();
}
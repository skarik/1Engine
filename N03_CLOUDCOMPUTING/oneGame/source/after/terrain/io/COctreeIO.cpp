
#include "COctreeIO.h"
#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"
#include "core/system/io/CMappedBinaryFile.h"
#include "core/system/io/CBufferIO.h"

#include "core/settings/CGameSettings.h"

#include "after/types/terrain/BlockType.h"
#include "after/terrain/VoxelTerrain.h"

#define LOADER_MODULE 1
//#define LOADER_SIZE 524288 //512 KB
#define LOADER_SIZE 589824 //576 KB

Terrain::COctreeIO::COctreeIO ( const char* n_savefile, VoxelTerrain* n_terrain )
	: m_savefile( n_savefile ), m_terrain( n_terrain )
{
	// The save file given is the full world pass, minus the final /

	if ( n_savefile == NULL )
	{
		// If no save file was passed in, then use the default current savefiles which default to debug
		CGameSettings::Active()->SetRealmSaveTarget( CGameSettings::Active()->GetRealmTargetName().c_str() );
		CGameSettings::Active()->SetWorldSaveTarget( CGameSettings::Active()->GetWorldTargetName().c_str() );
		m_savefile = CGameSettings::Active()->MakeWorldSaveDirectory().c_str();
	}

#	if LOADER_MODULE==1
	FileMapInitialize();
#	elif LOADER_MODULE==2

#	endif
}

Terrain::COctreeIO::~COctreeIO ( void )
{
#	if LOADER_MODULE==1
	FileMapFree();
#	elif LOADER_MODULE==2

#	endif
}


bool Terrain::COctreeIO::LoadSector ( Payload* o_sector, GamePayload* o_sector_gamedata, const RangeVector& n_sector_id, const uint32_t n_max_subdivide )
{
	//boost::mutex::scoped_lock rwLock ( m_wr_lock );
#if LOADER_MODULE==0

	// Open up file under save file
	char filename [256];
	GetSectorFilename( filename, n_sector_id );
	CBinaryFile m_file;
	m_file.Open( filename, CBinaryFile::IO_READ );

	if ( m_file.IsOpen() ) {
		ReadSubsectorSpecial( o_sector, 64, &m_file, n_max_subdivide );
		//ReadSubsector( o_sector, 64, &m_file );
		return true;
	}
	else if ( m_file.Exists( filename ) ) {
		// Huh?
		throw std::exception("Couldn't open an existing file. Is there a bad lock?");
	}
	else {
		return false;
	}

#elif LOADER_MODULE==1

	int f_id = GetSectorFilemapIndex( n_sector_id );
	std::lock_guard<std::mutex> rwLock ( m_writestate[f_id].m_lock );

	// First get sector for it
	BTree_Start( f_id );
	void* tDataPosition = BTree_GetSectorData ( n_sector_id, f_id );
	if ( tDataPosition == NULL ) {
		return false;
	}
	// Move to the data position
	m_writestate[f_id].m_file->SeekTo( (uint32_t)tDataPosition );

	// Now read data manually
	//char readbuffer [524288];
	//char side_buffer [524288];
	char* readbuffer = new char [LOADER_SIZE];
	char* side_buffer = new char [LOADER_SIZE];
	
	CBinaryFile m_file ( m_writestate[f_id].m_file->GetStream() );
	uint32_t sectorSize = m_file.ReadUInt32();
	m_file.ReadData( readbuffer, sectorSize ); // Read in buffer
	// Decompress data
	sectorSize = ExpandBuffer( readbuffer, sectorSize, side_buffer, LOADER_SIZE );
	
	// Convert buffer data into actual game data
	CBufferIO mBufferIO (readbuffer,sectorSize);
	//ReadSubsectorSpecial( o_sector, 64, &mBufferIO, n_max_subdivide );
	ReadSector( o_sector, &mBufferIO, n_max_subdivide );
	ReadSectorGameData( o_sector_gamedata, &mBufferIO, n_max_subdivide, n_sector_id );
	mBufferIO.CheckPosition();

	delete [] readbuffer;
	delete [] side_buffer;
	
	return true;

#elif LOADER_MODULE==2

#endif

}

// Reads subsector to specified density
uint64_t Terrain::COctreeIO::ReadSector ( Payload* o_sector, CBufferIO* ifile, const uint32_t n_max_subdivide )
{
	if ( n_max_subdivide == 5 )
	{	// Simply read in the entire block
		ifile->ReadData( (char*)(o_sector->data), sizeof(terra_b) * BLOCK_COUNT );
	}
	else
	{	// Read in with skipping
		throw Core::NotYetImplementedException();
	}
}

//void Terrain::COctreeIO::ReadSubsector ( Sector* o_sector, const uint32_t n_current_subdivide, CBinaryFile* ifile )
//{
//	throw std::exception("");
//	/*for ( int i = 0; i < 8; ++i )
//	{
//		o_sector->lock.lock();
//		// Read if data or pointer
//		bool nextIsData = ifile->ReadChar();
//		// If data, then read new data to sector
//		if ( nextIsData )
//		{
//			// Delete old area
//			if ( o_sector->data[i] == false ) {
//				// Delete recursively
//				TrackInfo tTracker;
//				tTracker.bindex = i;
//				tTracker.sector = o_sector;
//					o_sector->lock.unlock();
//				m_terrain->SectorDelete( tTracker );
//					o_sector->lock.lock();
//			}
//			o_sector->data[i] = true;
//			// Read data
//			o_sector->info[i].raw = ifile->ReadUInt64();
//			o_sector->lock.unlock();
//		}
//		else
//		{
//			// Create new data
//			if ( o_sector->data[i] == true ) {
//				// Create new sector
//				TrackInfo tTracker;
//				tTracker.bindex = i;
//				tTracker.sector = o_sector;
//					o_sector->lock.unlock();
//				m_terrain->SectorSubdivide( tTracker );
//					o_sector->lock.lock();
//			}
//			o_sector->data[i] = false;
//			// Read new sector in
//			ReadSubsector( o_sector->leafs[i], n_current_subdivide/2, ifile );
//			o_sector->lock.unlock();
//		}
//	}*/
//}
/*uint64_t Terrain::COctreeIO::ReadSubsectorSpecial ( Sector* o_sector, const uint32_t n_current_subdivide, CBufferIO* ifile, const uint32_t n_max_subdivide )
{
	if ( n_current_subdivide == 0 ) {
		throw std::invalid_argument( "Invalid file being read" );
	}

	ifile->CheckPosition();

	if ( n_current_subdivide > n_max_subdivide )
	{
		for ( int i = 0; i < 8; ++i )
		{
			o_sector->BeginWrite();
			// Read if data or pointer
			bool nextIsData = ifile->ReadChar();
			// If data, then read new data to sector
			if ( nextIsData )
			{
				// Delete old area
				if ( o_sector->data[i] == false ) {
					// Delete recursively
					TrackInfo tTracker;
					tTracker.bindex = i;
					tTracker.sector = o_sector;
					m_terrain->SectorDelete( tTracker );
				}
				o_sector->data[i] = true;
				// Read data
				o_sector->info[i].raw = ifile->ReadUInt64();
				o_sector->EndWrite();
			}
			else
			{
				// If will be populating this next subdivide, then subdivide
				if ( n_current_subdivide/2 > n_max_subdivide )
				{
					// Create new data
					if ( o_sector->data[i] == true ) {
						// Create new sector
						TrackInfo tTracker;
						tTracker.bindex = i;
						tTracker.sector = o_sector;
						m_terrain->SectorSubdivide( tTracker );
					}
					o_sector->data[i] = false;
					// Read new sector in
					ReadSubsectorSpecial( o_sector->leafs[i], n_current_subdivide/2, ifile, n_max_subdivide );
					o_sector->EndWrite();
				}
				// Otherwise, we won't be populating it
				else
				{
					// Set data instead
					if ( o_sector->data[i] == false ) {
						// Delete recursively
						TrackInfo tTracker;
						tTracker.bindex = i;
						tTracker.sector = o_sector;
						m_terrain->SectorDelete( tTracker );
					}
					o_sector->data[i] = true;
					// Read new sector in
					o_sector->info[i].raw = ReadSubsectorSpecial( o_sector->leafs[i], n_current_subdivide/2, ifile, n_max_subdivide );
					o_sector->EndWrite();
				}
			}
		}
		return 0;
	}
	else
	{
		// Above the limit. Need to read everything in anyways.
		int average_normal_x = 0;
		int average_normal_y = 0;
		int average_normal_z = 0;
		float average_normal_xw = 0;
		float average_normal_yw = 0;
		float average_normal_zw = 0;
		int smooth_count = 0;

		terra_b lastBlock;
		lastBlock.raw = 0;
		for ( int i = 0; i < 8; ++i )
		{
			// Read if data or pointer
			bool nextIsData = ifile->ReadChar();
			// If data, then read new data to sector
			terra_b nextBlock;
			if ( nextIsData )
			{
				nextBlock.raw = ifile->ReadUInt64();
			}
			else
			{
				// Read new sector in
				nextBlock.raw = ReadSubsectorSpecial( NULL, n_current_subdivide/2, ifile, n_max_subdivide );
			}
			// Read in more dense data
			if ( nextBlock.block > lastBlock.block ) {
				lastBlock.block = nextBlock.block;
			}
			// Sample all the normals and average them
			average_normal_x += nextBlock.normal_y_x;
			average_normal_x += nextBlock.normal_z_x;
			average_normal_y += nextBlock.normal_x_y;
			average_normal_y += nextBlock.normal_z_y;
			average_normal_z += nextBlock.normal_x_z;
			average_normal_z += nextBlock.normal_y_z;

			if ( Terrain::Checker::BlocktypeOpaque( nextBlock.block ) ) {
				average_normal_xw += Terrain::_depth_bias(nextBlock.normal_x_w)*0.5f + ( ((i%2)==0)? 0.0f : 0.5f );
				average_normal_yw += Terrain::_depth_bias(nextBlock.normal_y_w)*0.5f + ( (((i/2)%2)==0)? 0.0f : 0.5f );
				average_normal_zw += Terrain::_depth_bias(nextBlock.normal_z_w)*0.5f + ( ((i/4)==0)? 0.0f : 0.5f );
			}

			smooth_count += nextBlock.smooth_normal;
		}
		lastBlock.normal_x_w = Terrain::_depth_unbias(average_normal_xw/8);
		lastBlock.normal_y_w = Terrain::_depth_unbias(average_normal_yw/8);
		lastBlock.normal_z_w = Terrain::_depth_unbias(average_normal_zw/8);

		lastBlock.normal_y_x = average_normal_x/16;
		lastBlock.normal_z_x = lastBlock.normal_y_x;
		lastBlock.normal_x_y = average_normal_y/16;
		lastBlock.normal_z_y = lastBlock.normal_x_y;
		lastBlock.normal_x_z = average_normal_z/16;
		lastBlock.normal_y_z = lastBlock.normal_x_z;

		lastBlock.smooth_normal = (smooth_count>=2) ? 1 : 0;

		return lastBlock.raw;
	}
}*/

// Do not call this on low LOD sectors!
bool Terrain::COctreeIO::SaveSector ( Payload* n_sector, GamePayload* n_sector_gamedata, const RangeVector& n_sector_id )
{
	//boost::mutex::scoped_lock rwLock ( m_wr_lock );
#if LOADER_MODULE==0
	
	char filename [256];
	GetSectorFilename( filename, n_sector_id );
	CBinaryFile m_file;
	m_file.Open( filename, CBinaryFile::IO_WRITE );
	if ( m_file.IsOpen() ) {
		WriteSubsector( n_sector, &m_file );
		return true;
	}
	else {
		return false;
	}
	
#elif LOADER_MODULE==1

	if ( n_sector_gamedata == NULL ) {
		throw std::invalid_argument( "n_sector_data can not be NULL" );
	}

	int f_id = GetSectorFilemapIndex( n_sector_id );
	std::lock_guard<std::mutex> rwLock ( m_writestate[f_id].m_lock );

	// Write data manually into a side-buffer
	//char writebuffer [524288];//[294912];
	//char side_buffer [524288];
	char* writebuffer = new char [LOADER_SIZE];
	char* side_buffer = new char [LOADER_SIZE];
	CBufferIO mBufferIO (writebuffer,LOADER_SIZE);
	WriteSector( n_sector, &mBufferIO );
	WriteSectorGameData( n_sector_gamedata, &mBufferIO, n_sector_id );
	mBufferIO.CheckPosition();

	// Compress the data
	uint32_t bufferSize = mBufferIO.TellPosition();
	bufferSize = CompressBuffer( writebuffer, bufferSize, side_buffer );
	//printf( "SIZE: %lf", bufferSize/320000.0 );

	// Now get sector for it
	BTree_Start( f_id );
	void* tDataPosition = BTree_GetSectorData ( n_sector_id, f_id );
	if ( tDataPosition != NULL ) {
		// Read in the size of the data. If we can fit our new data in there, then no job needed.
		m_writestate[f_id].m_file->SeekTo( (uint32_t)tDataPosition );
		CBinaryFile m_file ( m_writestate[f_id].m_file->GetStream() );
		uint32_t oldBufferSize = m_file.ReadUInt32();
		// Otherwise, need to free up the data sectors.
		if ( oldBufferSize < bufferSize ) {
			// Go to the 
			//BTree_FreeSectorData ( 
			tDataPosition = NULL; // For now, just go to new area
		}
	}
	if ( tDataPosition == NULL ) {
		TreeNodeInfo newLeaf;
		// First allocate area for the terrain data
		//newLeaf.data_ptr = New_BTreeData( (uint32_t)ceil(320000/2048.0), f_id );
		newLeaf.data_ptr = New_BTreeData( (uint32_t)(ceil(bufferSize/2048.0)+1), f_id );
		newLeaf.index = n_sector_id;
		// Reset BTree finder
		BTree_Start( f_id );
		// Add new leaf to the BTree
		BTree_AddSectorData ( newLeaf, f_id );
		// Set data
		tDataPosition = newLeaf.data_ptr;
	}
	// Move to the data position
	m_writestate[f_id].m_file->SeekTo( (uint32_t)tDataPosition );

	// Write the sector data to the file
	CBinaryFile m_file ( m_writestate[f_id].m_file->GetStream() );
	m_file.WriteUInt32( bufferSize );
	m_file.WriteData( writebuffer, bufferSize );
	//printf( " WRITING %lf %% of BYTES\n", bufferSize/320000.0 );

	delete [] writebuffer;
	delete [] side_buffer;

	return true;

#elif LOADER_MODULE==2


#endif
}
//void Terrain::COctreeIO::WriteSubsector ( Payload* n_sector, CBufferIO* ofile )
//{
//	ofile->CheckPosition();
//
//	//n_sector->lock.lock_shared();
//	n_sector->BeginRead();
//	// Loop through sectors
//	for ( int i = 0; i < 8; ++i )
//	{
//		// Write if data or pointer
//		ofile->WriteChar( (n_sector->data[i])?1:0 );
//		// Write either data, or next sector information
//		if ( n_sector->data[i] ) {
//			ofile->WriteUInt64( n_sector->info[i].raw );
//		}
//		else {
//			WriteSubsector( n_sector->leafs[i], ofile );
//		}
//	}
//	n_sector->EndRead();
//	//n_sector->lock.unlock_shared();
//}

void Terrain::COctreeIO::WriteSector ( Payload* n_sector, CBufferIO* ofile )
{
	ofile->CheckPosition();
	ofile->WriteData( (char*)(n_sector->data), sizeof(terra_b) * BLOCK_COUNT );
}


inline void Terrain::COctreeIO::GetSectorFilename ( char* string, const RangeVector& n_sector_id )
{
	sprintf( string, "%s/%d_%d_%d", m_savefile.c_str(), n_sector_id.x, n_sector_id.y, n_sector_id.z );
}
inline int Terrain::COctreeIO::GetSectorFilemapIndex ( const RangeVector& n_sector_id )
{
	int result = (n_sector_id.x+n_sector_id.y+n_sector_id.z) % 4;
	if ( result < 0 ) {
		result += 4;
	}
	return result;
}


#undef LOADER_MODULE
#undef LOADER_SIZE
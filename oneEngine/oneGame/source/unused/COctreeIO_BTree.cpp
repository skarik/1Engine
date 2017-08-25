
#include "COctreeIO.h"
#include "EngineIO.h"
#include "CMappedBinaryFile.h"

#include "COctreeTerrain.h"

inline void Terrain::COctreeIO::GetSectorFilemapFilename ( char* string, const int n_filemap_index )
{
	sprintf( string, "%s/sectormap.%d.0.0.0.ten", m_savefile.c_str(), n_filemap_index );
}

#include "CMappedBinaryFile.h"

void Terrain::COctreeIO::FileMapInitialize ( void )
{
	/*for ( uint i = 0; i < 4; ++i ) 
	{
		char filename [256];
		GetSectorFilemapFilename( filename, i );

		m_writestate[i].m_file = CreateFile( filename, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL );

		char eventName [256];
		sprintf( eventName, "after_terra%d", i );
		//m_writestate[i].m_file_mapping = CreateFileMapping( m_writestate[i].m_file, NULL, PAGE_READWRITE, 0,1073741824, eventName );//40000000
		//m_writestate[i].m_file_mapping = CreateFileMapping( m_writestate[i].m_file, NULL, PAGE_READWRITE, 0,0x40000000, eventName );
		m_writestate[i].m_file_mapping = CreateFileMapping( m_writestate[i].m_file, NULL, PAGE_READWRITE, 0,0x20000000, eventName );
	}*/
	for ( uint i = 0; i < 4; ++i )
	{
		char filename [256];
		GetSectorFilemapFilename( filename, i );

		m_writestate[i].m_file = new CMappedBinaryFile( filename );
	}
	// Leave if no treenode!
	if ( sizeof(TreeNode) != 64 ) {
		throw std::exception( "Bad treenode size!" );
		exit(0);
	}
	std::cout << "Size of IO map tree node: " << sizeof(TreeNode) << std::endl;
}
void Terrain::COctreeIO::FileMapFree ( void )
{
	for ( uint i = 0; i < 4; ++i ) {
		delete m_writestate[i].m_file;
		m_writestate[i].m_file = NULL;
	}
}

void Terrain::COctreeIO::FileMapSync ( void )
{
	for ( uint i = 0; i < 4; ++i ) {
		m_writestate[i].m_file->SyncToDisk();
	}
}

bool Terrain::COctreeIO::BTree_FindSector ( const RangeVector& n_position )
{
	int id = GetSectorFilemapIndex(n_position);
	bool foundSector = false;

	while ( !foundSector )
	{
		// Read in sector at current position
		TreeNode node;
		m_writestate[id].m_file->ReadBuffer( &node, sizeof(TreeNode) );

		// Loop through the node data
		uint i;
		for ( i = 0; i < 3; ++i )
		{
			if ( node.data[i].data_ptr != NULL )
			{
				// If target is equal to this node
				if ( n_position == node.data[i].index )
				{
					return true;
				}
				// If target is smaller than this node
				else if ( n_position < node.data[i].index )
				{
					// Go to the node to the left
					if ( node.ptrs[i] == NULL ) {
						return false; // No node there
					}
					if ( ((uint32_t)node.ptrs[i]) % 16 != 0 ) {
						throw std::exception( "Misalignment in savedata" );
					}
					m_writestate[id].m_file->SeekTo( (uint32_t) node.ptrs[i] );
					break; // Don't loop over next
				}
			}
			else
			{
				// Node not in tree
				return false;
			}
		}
		if ( i == 3 )
		{
			// And if down here, then means the position is larger than all 3, so go to the right
			if ( node.ptrs[3] == NULL ) {
				return false; // No node there
			}
			if ( ((uint32_t)node.ptrs[3]) % 16 != 0 ) {
				throw std::exception( "Misalignment in savedata" );
			}
			m_writestate[id].m_file->SeekTo( (uint32_t) node.ptrs[3] );
		}
	}

	throw std::exception( "Error in searching for sector" );
	return NULL;
}


// Looks for the range vector in the sector file. Returns NULL if data doesn't exist.
void*	Terrain::COctreeIO::BTree_GetSectorData ( const RangeVector& n_position, const int n_filemap_index )
{
	const int id = n_filemap_index;
	bool foundSector = false;
	int repeats = 0;
	while ( !foundSector )
	{
		// Read in sector at current position
		TreeNode node;
		m_writestate[id].m_file->ReadBuffer( &node, sizeof(TreeNode) );
		++repeats;

		// Loop through the node data
		uint i;
		for ( i = 0; i < 3; ++i )
		{
			if ( node.data[i].data_ptr != NULL )
			{
				// If target is equal to this node
				if ( n_position == node.data[i].index )
				{
					return node.data[i].data_ptr;
				}
				// If target is smaller than this node
				else if ( n_position < node.data[i].index )
				{
					// Go to the node to the left
					if ( node.ptrs[i] == NULL ) {
						return NULL; // No node there
					}
					if ( ((uint32_t)node.ptrs[i]) % 16 != 0 ) {
						throw std::exception( "Misalignment in savedata" );
					}
					m_writestate[id].m_file->SeekTo( (uint32_t) node.ptrs[i] );
					break; // Don't loop over next
				}
			}
			else
			{
				// Node not in tree
				return NULL;
			}
		}
		if ( i == 3 )
		{
			// And if down here, then means the position is larger than all 3, so go to the right
			if ( node.ptrs[3] == NULL ) {
				return NULL; // No node there
			}
			if ( ((uint32_t)node.ptrs[3]) % 16 != 0 ) {
				throw std::exception( "Misalignment in savedata" );
			}
			m_writestate[id].m_file->SeekTo( (uint32_t) node.ptrs[3] );
		}
	}

	return NULL;
}
// Adds the given TreeNodeInfo to the B-tree, making allocations as needed.
void	Terrain::COctreeIO::BTree_AddSectorData ( const TreeNodeInfo& n_data, const int n_filemap_index )
{
	const int id = n_filemap_index;
	bool placedSector = false;
	uint32_t	tFilemapPos = 2048; // Root is always at this position

	// Target is to find the spot we can add new data
	while ( !placedSector )
	{
		// Read in sector at current position
		TreeNode node;	memset( &node, 0, sizeof(TreeNode) );
		m_writestate[id].m_file->ReadBuffer( &node, sizeof(TreeNode) );

		// Loop through the node data
		uint i;
		for ( i = 0; i < 3; ++i )
		{
			// If the target has data
			if ( node.data[i].data_ptr != NULL )
			{
				// If target is smaller than this node
				if ( n_data.index < node.data[i].index )
				{
					// Go to the node to the left
					if ( node.ptrs[i] == NULL ) {
						node.ptrs[i] = New_BTreeNode(id);
						m_writestate[id].m_file->SeekTo( tFilemapPos );
						m_writestate[id].m_file->WriteBuffer( &node, sizeof(TreeNode) );
					}
					tFilemapPos = (uint32_t) node.ptrs[i];
					if ( tFilemapPos % 16 != 0 ) {
						throw std::exception( "Misalignment in savedata" );
					}
					m_writestate[id].m_file->SeekTo( tFilemapPos );
					break; // Don't loop over next
				}
			}
			else // Hit an empty spot. Put self into it
			{
				node.data[i] = n_data;
				m_writestate[id].m_file->SeekTo( tFilemapPos );
				m_writestate[id].m_file->WriteBuffer( &node, sizeof(TreeNode) );
				return;
			}
		}
		// And if down here, then means the position is larger than all 3, so go to the right
		if ( i == 3 )
		{
			if ( node.ptrs[3] == NULL ) {
				node.ptrs[3] = New_BTreeNode(id);
				m_writestate[id].m_file->SeekTo( tFilemapPos );
				m_writestate[id].m_file->WriteBuffer( &node, sizeof(TreeNode) );
			}
			tFilemapPos = (uint32_t) node.ptrs[3];
			if ( tFilemapPos % 16 != 0 ) {
				throw std::exception( "Misalignment in savedata" );
			}
			m_writestate[id].m_file->SeekTo( tFilemapPos );
		}
	}
}

// Set file index to start
void	Terrain::COctreeIO::BTree_Start ( const int n_filemap_index )
{
	m_writestate[n_filemap_index].m_file->SeekTo( 2048 );
}


// Allocates a new B-tree node
void*	Terrain::COctreeIO::New_BTreeNode ( const int n_filemap_index )
{
	const int id = n_filemap_index;
	uint32_t tSector = 0; // First sector always has the first table
	uint32_t tIndexCounter = 0;

	char tBuffer [2048];

	while ( true )
	{
		// Go to new sector
		m_writestate[id].m_file->SeekTo(tSector*2048);
		// Read in the data list
		m_writestate[id].m_file->ReadBuffer( tBuffer, 2048 );

		for ( uint i = 0; i < 2047; ++i ) // go from 0 to 2046. 2047 is the next list.
		{
			if ( i == 0 ) {
				// Increment the index counter
				tIndexCounter += 1;
				continue; // Skip 0, as 0 always refers to self
			}
			// Check if area is empty
			if ( tBuffer[i] & 0x01 ) {
				// Is used, but check for a nonfull area (a 0b0001 as opposed to 0b0101)
				if ( !(tBuffer[i] & 0x02) && !(tBuffer[i] & 0x04) ) {
					// Isn't full, let's use this sector!
					bool filled = false;
					void* result = New_BTreeNode_SearchArea( tIndexCounter, filled, id );
					if ( filled ) {
						tBuffer[i] |= 0x04; // Mark as 0b0101
						// Seek backwards
						m_writestate[id].m_file->SeekTo(tSector*2048);
						// Overwrite the buffer
						m_writestate[id].m_file->WriteBuffer( tBuffer, 2048 );
					}
					return result;
				}
			}
			else {
				// Area is empty, let's use it.
				tBuffer[i] = 0x01; // Marked as 0b0001 for tree info
				// Now, grab result
				bool filled = false;
				void* result = New_BTreeNode_SearchArea( tIndexCounter, filled, id );
				// Is impossible for it to be filled, so ignore return result
				// Just the same, seek backwards
				m_writestate[id].m_file->SeekTo(tSector*2048);
				// Overwrite the buffer
				m_writestate[id].m_file->WriteBuffer( tBuffer, 2048 );
				return result;
			}
			// Increment the index counter
			tIndexCounter += 1;
		}
		if ( tIndexCounter%2047 != 0 ) {
			throw std::exception ( "TABLE MISMATCH!" );
		}
		if ( tBuffer[2047] == 0 ) {
			tBuffer[2047] = 0x01; // Mark area as used. Move to the new area.
			// Seek backwards
			m_writestate[id].m_file->SeekTo(tSector*2048);
			// Overwrite the buffer
			m_writestate[id].m_file->WriteBuffer( tBuffer, 2048 );
		}

		// Mark sector position as the last area (thus seeking to the next list)
		tSector = tIndexCounter;
	}
}
// Submethod of New_BTreeNode.
void*	Terrain::COctreeIO::New_BTreeNode_SearchArea ( const uint32_t n_filemap_sector, bool& o_full, const int n_filemap_index )
{
	const int id = n_filemap_index;
	char tBuffer [2048];
	TreeNode* tNodes = (TreeNode*) tBuffer;

	// Go to new sector
	m_writestate[id].m_file->SeekTo(n_filemap_sector*2048);
	// Read in the data list
	uint32_t readcount = m_writestate[id].m_file->ReadBuffer( tBuffer, 2048 );

	/*if ( tBuffer[1] == 3 ) {
		cout << ftell(m_writestate[id].m_file->GetStream()) << endl;
		m_writestate[id].m_file->SeekTo(n_filemap_sector*2048);
		throw std::exception( "Problem with area seek" );
	}*/

	// Loop through the sector for an empty slot
	for ( uint i = 0; i < 32; ++i ) {
		if ( abs(tNodes[i].data[0].index.x) > 26000 || abs(tNodes[i].data[0].index.y) > 26000 || abs(tNodes[i].data[0].index.z) > 13500 ) {
			throw std::exception( "Error in searching area" );
		}
		if ( tNodes[i].data[0].data_ptr == NULL ) {
			if ( i == 31 ) {
				o_full = true;
			}
			else {
				o_full = false;
			}
			return (void*)( n_filemap_sector*2048 + i*64 );
		}
	}

	throw std::exception( "Error in searching area" );
	return NULL;
}

// Allocates a new sector with the given size
void*	Terrain::COctreeIO::New_BTreeData ( const uint32_t n_minimum_size, const int n_filemap_index )
{
	if ( n_minimum_size > 2046 ) {
		throw std::out_of_range( "Cannot allocate areas larger than 4092 KB" );
	}

	const int id = n_filemap_index;
	uint32_t tSector = 0; // First sector always has the first table
	uint32_t tIndexCounter = 0;

	char tBuffer [2048];

	while ( true )
	{
		// Go to new sector
		m_writestate[id].m_file->SeekTo(tSector*2048);
		// Read in the data list
		m_writestate[id].m_file->ReadBuffer( tBuffer, 2048 );

		for ( uint i = 0; i < 2047; ++i ) // go from 0 to 2046. 2047 is the next list.
		{
			if ( i == 0 ) {
				// Increment the index counter
				tIndexCounter += 1;
				continue; // Skip 0, as 0 always refers to self
			}
			if ( tSector == 0 && i == 1 ) {
				// Increment the index counter
				tIndexCounter += 1;
				continue; // Skip 1, as 1 always refers to first table
			}
			// Check if area is empty
			if ( tBuffer[i] & 0x01 ) {
				// Is not empty. So screw this.
			}
			else {
				// Area is empty. Now, need to seek forward and find a large enough area.
				bool bAreaSizeValid = false;
				uint j;
				for ( j = i; j < 2047; ++j )
				{
					if ( tBuffer[j] & 0x01 ) {
						break; // Area has data, so can't use range.
					}
					if ( (j-i+1) >= n_minimum_size ) {
						bAreaSizeValid = true;
						break; // Got to here without running into data conflicts, so area is proper
					}
				}
				// Area is big enough, so let's use it.
				if ( bAreaSizeValid ) {
					for ( j = i; j < i+n_minimum_size; ++j ) {
						tBuffer[j] = 0x03; // Marked as 0b0011 for sector info
					}
					// Seek backwards
					m_writestate[id].m_file->SeekTo(tSector*2048);
					// Overwrite the buffer
					m_writestate[id].m_file->WriteBuffer( tBuffer, 2048 );
					// Return this buffer
					return (void*)( tIndexCounter*2048 ); // Start of this buffer is proper
				}
				else {
					// Area was not large enough. Skip to where j was.
					uint32_t nexti = std::max<uint>( i,j-1 ); // j will always be at least 1
					tIndexCounter += nexti-i;
					i = nexti;
				}
			}
			// Increment the index counter
			tIndexCounter += 1; // if i was 2046, tIndexCounter will now be 2047
		}
		if ( tIndexCounter%2047 != 0 ) {
			throw std::exception ( "TABLE MISMATCH!" );
		}
		if ( tBuffer[2047] == 0 ) {
			tBuffer[2047] = 0x01; // Mark area as used. Move to the new area.
			// Seek backwards
			m_writestate[id].m_file->SeekTo(tSector*2048);
			// Overwrite the buffer
			m_writestate[id].m_file->WriteBuffer( tBuffer, 2048 );
		}
		// Mark sector position as the last area (thus seeking to the next list)
		tSector = tIndexCounter;
	}
	throw std::exception( "BAD INPUT" );
}
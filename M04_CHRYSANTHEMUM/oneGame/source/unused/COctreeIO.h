
#ifndef _C_TERRAIN_OCTREE_IO_H_
#define _C_TERRAIN_OCTREE_IO_H_

#include "TerrainSector.h"
#include "TerrainGameSector.h"
#include "RangeVector.h"
#include "arstring.h"

class CBinaryFile;
class CMappedBinaryFile;
class COctreeTerrain;
class CBufferIO;

namespace Terrain
{
	class COctreeIO
	{
	public:
		// The save file given is the full world pass, minus the final /
		explicit COctreeIO ( const char* n_savefile, COctreeTerrain* n_terrain );
				~COctreeIO ( void );

		// Load sector from file. Returns true if file existed.
		// Returns false if sector's file doesn't exist.
		// Assumes a subdivide that starts at 64.
		// Will skip game data if o_sector_data is NULL.
		bool LoadSector ( Sector* o_sector, SectorGameData* o_sector_data, const RangeVector& n_sector_id, const uint32_t n_max_subdivide );

		// Save sector to file. Best to only use on fully subdivided areas.
		// Returns true on success, false on failure to open file.
		// Will not work if n_sector_data is NULL.
		bool SaveSector ( Sector* n_sector, SectorGameData* n_sector_data, const RangeVector& n_sector_id );

		// Return save file. Returns a constant C-string pointer.
		// Use for IO instead of CGameSettings
		const char*	GetSavefile ( void ) const {
			return m_savefile.c_str();
		}

	private:
		inline void GetSectorFilename ( char* string, const RangeVector& n_sector_id );

		// Reads subsector to full density
		void		ReadSubsector ( Sector* o_sector, const uint32_t n_current_subdivide, CBinaryFile* ifile );
		// Reads subsector to desired density
		uint64_t	ReadSubsectorSpecial ( Sector* o_sector, const uint32_t n_current_subdivide, CBufferIO* ifile, const uint32_t n_max_subdivide );
		// Reads subsector prop data
		void		ReadSectorGameData ( SectorGameData* o_sector, CBufferIO* ofile, const uint32_t n_max_subdivide, const RangeVector& n_sector_id );

		// Writes subsector out to full density
		void WriteSubsector ( Sector* n_sector, CBufferIO* ofile );
		// Writes subsector prop data
		void WriteSectorGameData ( SectorGameData* n_sector, CBufferIO* ofile, const RangeVector& n_sector_id );

	private:
		unsigned long	CompressBuffer ( char* n_buffer, unsigned long n_original_size, char* n_side_buffer );
		unsigned long	ExpandBuffer ( char* n_buffer, unsigned long n_original_size, char* n_side_buffer, unsigned long n_max_size, unsigned long n_match_size=0 );

	private:
		inline int GetSectorFilemapIndex ( const RangeVector& n_sector_id );
		inline void GetSectorFilemapFilename ( char* string, const int n_filemap_index );

		inline void GetSectorFilelutFilename ( char* string, const int n_filemap_index, const RangeVector& n_sector_id );

	private:
		boost::mutex	m_wr_lock;
		arstring<256>	m_savefile;
		COctreeTerrain*	m_terrain;

	private:
		struct WriteState
		{
			boost::mutex		m_lock;
			CMappedBinaryFile*	m_file;
		};
		WriteState		m_writestate [4];

		struct TreeNodeInfo {
			RangeVector	index;
			void*		data_ptr;
		};
		struct TreeNode
		{
			void*			ptrs [4];
			TreeNodeInfo	data [3];
		};

	private:
		// File mapping
		void FileMapInitialize ( void );
		void FileMapFree ( void );

		void FileMapSync ( void );

		// BTree file
		bool BTree_FindSector ( const RangeVector& );

		// Looks for the range vector in the sector file. Returns NULL if data doesn't exist.
		void*	BTree_GetSectorData ( const RangeVector& n_position, const int n_filemap_index );
		// Adds the given TreeNodeInfo to the B-tree, making allocations as needed.
		void	BTree_AddSectorData ( const TreeNodeInfo& n_data, const int n_filemap_index );

		// Allocates a new B-tree node
		void*	New_BTreeNode ( const int n_filemap_index );
			// Submethod of New_BTreeNode.
			void*	New_BTreeNode_SearchArea ( const uint32_t n_filemap_sector, bool& o_full, const int n_filemap_index );
		// Allocates a new sector with the given size
		void*	New_BTreeData ( const uint32_t n_minimum_size, const int n_filemap_index );

		// Deallocates given B-tree node
		void	Free_BTreeNode ( void* );
		// Deallocates given area
		void	Free_BTreeData ( void* );

		// Set file index to start
		void	BTree_Start ( const int n_filemap_index );

	private:
		// LUT
	};
}

#endif//_C_TERRAIN_OCTREE_IO_H_
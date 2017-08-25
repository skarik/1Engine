//===============================================================================================//
//	COctreeIO_LUT.cpp
//
// Shorthand for building consistent filenames. Not actually a LUT.
//===============================================================================================//

#include "COctreeIO.h"

inline void Terrain::COctreeIO::GetSectorFilelutFilename ( char* string, const int n_filemap_index, const RangeVector& n_sector_id )
{
	sprintf( string, "%s/lut.%d.%d.%d.%d.lut.ten", m_savefile.c_str(), n_filemap_index, n_sector_id.x/64, n_sector_id.y/64, n_sector_id.z/64 );
}

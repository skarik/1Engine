
#include "core/settings/CGameSettings.h"
#include "CZonedCharacterIO.h"

#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/serializer/ISerialBinary.h"
#include "core-ext/system/io/serializer/OSerialBinary.h"

using namespace NPC;

//  CreateCharacterFile
// All values in n_charinfo are required.
void CZonedCharacterIO::CreateCharacterFile ( uint64_t nid, characterFile_t n_charinfo )
{
	arstring<256>	s_charfile_fn;
	FILE*			filep;
	CBinaryFile		charfile;

	sprintf( s_charfile_fn, "%s/chars/%lld.entry", CGameSettings::Active()->GetWorldSaveDir().c_str(), nid );

	if ( IO::FileExists( s_charfile_fn ) )
	{
		// Just save new data
		SaveCharacterFile( nid, n_charinfo );
	}
	else
	{
		// Open dat shit
		charfile.Open( s_charfile_fn, CBinaryFile::IO_WRITE );
		filep = charfile.GetFILE();

		// Clear 4k of data
		char empty [4096] = {0};
		fwrite( empty,4096,1, filep );

		// Close file to commit changes
		charfile.Close();

		// Save new data
		SaveCharacterFile( nid, n_charinfo );
	}
}
//  LoadCharacterFile
// NULL values will be skipped
int CZonedCharacterIO::LoadCharacterFile ( uint64_t nid, characterFile_t n_charinfo ) 
{
	arstring<256>	s_charfile_fn;
	FILE*			filep;
	CBinaryFile		charfile;

	// Load file
	sprintf( s_charfile_fn, "%s/chars/%lld.entry", CGameSettings::Active()->GetWorldSaveDir().c_str(), nid );
	charfile.Open( s_charfile_fn, CBinaryFile::IO_READ );
	filep = charfile.GetFILE();

	if ( filep && charfile.IsOpen() ) {
		// Now, write char info where valid
		if ( n_charinfo.worldstate ) {		// sWorldState
			fseek( filep, 0, SEEK_SET );
			fread( n_charinfo.worldstate, sizeof(sWorldState), 1, filep );
		}
		if ( n_charinfo.prefs ) {			// sPreferences
			fseek( filep, 1024, SEEK_SET );
			fread( n_charinfo.prefs, sizeof(sPreferences), 1, filep );

		}
		if ( n_charinfo.opinions ) {		// sOpinions
			fseek( filep, 2048, SEEK_SET );
			fread( n_charinfo.opinions, sizeof(sOpinions), 1, filep );

		}
		if ( n_charinfo.rstats ) {			// CRacialStats
			fseek( filep, 3072, SEEK_SET );
			//ISerialBinary serializer (&charfile);
			//serializer >> n_charinfo.rstats;
			n_charinfo.rstats->LoadFromFile(charfile);
		}

		return 0;
	}
	return -1;
}
//  SaveCharacterFile
// NULL values will be skipped
void CZonedCharacterIO::SaveCharacterFile ( uint64_t nid, characterFile_t n_charinfo )
{
	arstring<256>	s_charfile_fn;
	FILE*			filep;
	CBinaryFile		charfile;

	// Load file
	sprintf( s_charfile_fn, "%s/chars/%lld.entry", CGameSettings::Active()->GetWorldSaveDir().c_str(), nid );
	charfile.Open( s_charfile_fn, CBinaryFile::IO_WRITE|CBinaryFile::IO_READ );
	filep = charfile.GetFILE();

	// Now, write char info where valid
	if ( n_charinfo.worldstate ) {		// sWorldState
		fseek( filep, 0, SEEK_SET );
		fwrite( n_charinfo.worldstate, sizeof(sWorldState), 1, filep );
	}
	if ( n_charinfo.prefs ) {			// sPreferences
		fseek( filep, 1024, SEEK_SET );
		fwrite( n_charinfo.prefs, sizeof(sPreferences), 1, filep );

	}
	if ( n_charinfo.opinions ) {		// sOpinions
		fseek( filep, 2048, SEEK_SET );
		fwrite( n_charinfo.opinions, sizeof(sOpinions), 1, filep );

	}
	if ( n_charinfo.rstats ) {			// CRacialStats
		fseek( filep, 3072, SEEK_SET );
		//OSerialBinary serializer (&charfile);
		//serializer << n_charinfo.rstats;
		n_charinfo.rstats->SaveToFile(charfile);
	}
}



//	SectorAddNPC
// Opens up file "$SAVEFOLDER/chars/$X_$Y_$Z_terra" and adds the NPC to it
// If the NPC is already in there, doesn't edit the file, but returns -1.
int CZonedCharacterIO::SectorAddNPC ( const RangeVector& nli_pos, const uint64_t nid )
{
	arstring<256>	s_sectorfile_fn;
	CBinaryFile		sectorfile;
	FILE*			filep;
	uint64_t		s_readbuffer [128];

	// Load file
	sprintf( s_sectorfile_fn, "%s/chars/%d_%d_%d_terra", CGameSettings::Active()->GetWorldSaveDir().c_str(), nli_pos.x, nli_pos.y, nli_pos.z );
	sectorfile.Open( s_sectorfile_fn, CBinaryFile::IO_WRITE|CBinaryFile::IO_READ );
	filep = sectorfile.GetFILE();

	// Read in the buffer
	char readCount;
	do {
		readCount = fread( s_readbuffer, 8, 128, filep );
		// Search for the NPC
		for ( char i = 0; i < readCount; ++i ) {
			if ( s_readbuffer[i] == nid ) {
				return -1;	// Already exists, return -1
			}
		}
	}
	while ( readCount == 128 );

	// At this point, add the NPC to the list
	fseek( filep, 0, SEEK_END );
	fwrite( &nid, 8, 1, filep );
	
	return 0;
}
//  SectorRemoveNPC
// Removes NPC from the sector. If the NPC or sector doesn't exist, returns -1.
int CZonedCharacterIO::SectorRemoveNPC ( const RangeVector& nli_pos, const uint64_t nid )
{
	arstring<256>	s_sectorfile_fn;
	CBinaryFile		sectorfile;
	FILE*			filep;
	std::vector<uint64_t>	npcList;

	// Load file
	sprintf( s_sectorfile_fn, "%s/chars/%d_%d_%d_terra", CGameSettings::Active()->GetWorldSaveDir().c_str(), nli_pos.x, nli_pos.y, nli_pos.z );
	sectorfile.Open( s_sectorfile_fn, CBinaryFile::IO_READ );
	if ( sectorfile.IsOpen() )
	{
		filep = sectorfile.GetFILE();

		// Get the list
		GetList( npcList,filep );
		// Edit the list
		bool hadChange = false;
		for ( auto it = npcList.begin(); it != npcList.end(); )
		{
			if ( (*it) == nid ) {
				hadChange = true;
				npcList.erase(it);
				break;
			}
			else {
				++it;
			}
		}
		// No change, return NPC didn't exist
		if ( !hadChange ) {
			return -1;
		}
		// Set the list
		sectorfile.Close();
		SetList( npcList,s_sectorfile_fn );

		return 0;
	}
	// No file, return
	return -1;
}
//  SectorGetNPCList
// Fills up the vector with a list of characters. Returns -1 if the area did not exist.
int CZonedCharacterIO::SectorGetNPCList ( const RangeVector& nli_pos, std::vector<uint64_t> & nvect )
{
	arstring<256>	s_sectorfile_fn;
	CBinaryFile		sectorfile;
	FILE*			filep;

	// Load file
	sprintf( s_sectorfile_fn, "%s/chars/%d_%d_%d_terra", CGameSettings::Active()->GetWorldSaveDir().c_str(), nli_pos.x, nli_pos.y, nli_pos.z );
	sectorfile.Open( s_sectorfile_fn, CBinaryFile::IO_READ );
	if ( sectorfile.IsOpen() ) {
		filep = sectorfile.GetFILE();
		// Get list
		GetList( nvect, filep );
		return 0;
	}
	else {
		return -1;
	}
}


void CZonedCharacterIO::GetList ( std::vector<uint64_t> & nvect, FILE* nfilep )
{
	uint64_t		s_readbuffer [128];
	char			readsize;

	do {
		readsize = fread( s_readbuffer, 8, 128, nfilep );
		for ( char i = 0; i < readsize; ++i ) {
			if ( s_readbuffer[i] != 0 ) {
				nvect.push_back( s_readbuffer[i] );
			}
		}
	}
	while ( readsize == 128 );
}

void CZonedCharacterIO::SetList ( std::vector<uint64_t> & nvect, const arstring<256>& ns_filename )
{
	FILE* filep = fopen( ns_filename, "wb" );
	if ( nvect.empty() ) {
		uint64_t a = 0;
		fwrite( &a, 8, 1, filep );
	}
	else {
		fwrite( &(nvect[0]), 8, nvect.size(), filep );
	}
	fclose( filep );
}
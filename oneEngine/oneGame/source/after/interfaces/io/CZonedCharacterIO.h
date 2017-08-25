
#ifndef _C_ZONED_CHARACTER_IO_H_
#define _C_ZONED_CHARACTER_IO_H_

#include <vector>

#include "after/states/CRacialStats.h"
#include "after/types/character/NPC_WorldState.h"
#include "after/types/WorldVector.h"

namespace NPC
{
	class CZonedCharacterIO
	{
	public:
		//  CreateCharacterFile
		// All values in n_charinfo are required.
		void CreateCharacterFile ( uint64_t nid, characterFile_t n_charinfo );
		//  LoadCharacterFile
		// NULL values will be skipped
		int  LoadCharacterFile ( uint64_t nid, characterFile_t n_charinfo );
		//  SaveCharacterFile
		// NULL values will be skipped
		void SaveCharacterFile ( uint64_t nid, characterFile_t n_charinfo );

		//	SectorAddNPC
		// Opens up file "$SAVEFOLDER/chars/$X_$Y_$Z_terra" and adds the NPC to it
		// If the NPC is already in there, doesn't edit the file, but returns -1.
		int SectorAddNPC ( const RangeVector& nli_pos, const uint64_t nid );
		//  SectorRemoveNPC
		// Removes NPC from the sector. If the NPC doesn't exist, returns -1.
		int SectorRemoveNPC ( const RangeVector& nli_pos, const uint64_t nid );
		//  SectorGetNPCList
		// Fills up the vector with a list of characters. Returns -1 if the area did not exist.
		int SectorGetNPCList ( const RangeVector& nli_pos, std::vector<uint64_t> & nvect );

	private:
		void GetList ( std::vector<uint64_t> & nvect, FILE* nfilep );
		void SetList ( std::vector<uint64_t> & nvect, const arstring<256>& ns_filename );
	};

}
#endif//_C_ZONED_CHARACTER_IO_H_
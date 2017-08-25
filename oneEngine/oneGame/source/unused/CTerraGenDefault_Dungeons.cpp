
// ======== CTerraGenDefault_Dungeons =======
// This is the definition for the first pass dungeon generation.
// Dungeons, caves, bridges, and excavations are handled in this area.

#include "CTerraGenDefault.h"
#include "CDungeonBase.h"
#include "CDungeonFactory.h"
#include "CGameSettings.h"

#include "CDebugConsole.h"

using std::cout;
using std::endl;
using std::max;
using std::min;

// =======================================
// == Generation Algorithm for Dungeons ==
// =======================================
void CTerraGenDefault::TerraGenDungeonPass ( CBoob * pBoob, RangeVector const& position )
{
	try
	{
		for ( char k = 0; k >= -3; k-- ) { // Generate downwards first
			for ( char i = -3; i <= 3; i++ ) {
				for ( char j = -3; j <= 3; j++ ) {
					TerraGen_Dungeons_Generate( RangeVector( position.x+i,position.y+j,position.z+k ) );
				}
			}
		}
		for ( char k = 0; k <= 3; k++ ) { // Generate upwards
			for ( char i = -3; i <= 3; i++ ) {
				for ( char j = -3; j <= 3; j++ ) {
					TerraGen_Dungeons_Generate( RangeVector( position.x+i,position.y+j,position.z+k ) );
				}
			}
		}
	}
	catch ( std::bad_alloc err )
	{
		cout << "ERROR IN GENERATION: " << err.what() << endl;
	};
	
	if ( dungeonList.size() > 1600 ) {
		//cout << "Dungeon table ran out of room, clearing table." << endl;
		dungeonLocalList.clear();
		for ( unordered_map<RangeVector,CDungeonBase*>::iterator it = dungeonList.begin(); it != dungeonList.end(); ++it )
		{
			if ( it->second ) {
				delete it->second;
				it->second = NULL;
			}
		}
		dungeonList.clear();
		//cout << "Dear heavens, the table was cleared without a memory error!" << endl;
	}

	CDungeonBase::generator = this;
	TerraGen_Dungeons_Excavate( pBoob, position );
}


// == TerraGen_Dungeons_Generate ==
// Generate dungeons is what creates the actual dungeon instances.
// First, a list of possible dungeons for an area are checked.
// This list is compared against a list of nearby dungeons and towns for a proper choice.
// Once a dungeon has been chosen, it is added to the major list and then saved to the disk.
void CTerraGenDefault::TerraGen_Dungeons_Generate ( RangeVector const& position )
{
	RangeVector	vCheckpos;
	CDungeonBase* pTargetDungeon;
	char		tempstring[256];
	CBinaryFile	oCheckfile;
	ftype		elevation;

	// Check if area already had dungeon generated, skip if it has been
	//if ( std::find( dungeonChecklist.begin(), dungeonChecklist.end(), position ) != dungeonChecklist.end() ) {
	unordered_map<RangeVector,CDungeonBase*>::iterator hashFind;
	hashFind = dungeonList.find( position );
	if ( hashFind != dungeonList.end() ) {
		return;
	}
	else {
		// Check if there's an associated dungeon file
		sprintf( tempstring, "%s.dungeons/%d_%d_%d\0",
			CGameSettings::Active()->GetTerrainSaveDir().c_str(),
			position.x,position.y,position.z );
		if ( oCheckfile.Exists( tempstring ) ) { // If it exists, load it and skip again
			//printf( "loading file %s\n", tempstring );

			oCheckfile.Open( tempstring, CBinaryFile::IO_READ );	// Open for reading
			pTargetDungeon = CDungeonBase::load( oCheckfile, this ); // Load dungeon
			oCheckfile.Close();
			if ( pTargetDungeon && (pTargetDungeon->m_centerindex==position) ) {
				//dungeonList.push_back( pTargetDungeon );	// Add to list
				dungeonList[position] = pTargetDungeon; // Add to table
			}
			else {
				Debug::Console->PrintError( "95: Either a dungeon couldn't be loaded or its position didn't match.\n" );
				printf( "  dungeon file %s\n", tempstring );
			}
			return; // Break out of generation routine
		}
		else {
			dungeonList[position] = NULL; // Add empty result to table
		}
	}
	
	// If the dungeon sampler list is empty, then create it
	if ( dungeonSamplerList.empty() )
	{
		// Add no choice
		dungeonSamplerList.push_back( NULL );

		//unordered_map<string,short>::iterator it;
		unordered_map<short,CDungeonBase*(*)(CTerrainGenerator*)>::iterator it;
		for ( it = Dungeon::Factory.dungeon_insts.begin(); it != Dungeon::Factory.dungeon_insts.end(); ++it )
		{
			// Lua dungeons are a special case. The directories *\lua\dungeons\ need to be checked for dungeons that generate
			// For now, Lua dungeons are skipped, and all other dungeons added.
			if ( it->first == 1 ) {
				continue;
			}
			// However, all other dungeons should be instantiated
			dungeonSamplerList.push_back( it->second(this) );
		}
	}

	// Get rid of any far dungeons from the local dungeon list
	{
		vector<CDungeonBase*>::iterator it = dungeonLocalList.begin();
		while ( it != dungeonLocalList.end() )
		{
			if (( abs((*it)->m_centerindex.x - position.x) > 7 )||( abs((*it)->m_centerindex.y - position.y) > 7 )||( abs((*it)->m_centerindex.z - position.z) > 3 )) {
				it = dungeonLocalList.erase( it );
			}
			else {
				++it;
			}
		}
	}
	// Add in any missing dungeons to local dungeon list
	{
		for ( char i = -7; i <= 7; i++ ) {
			vCheckpos.x = position.x + i;
			for ( char j = -7; j <= 7; j++ ) {
				vCheckpos.y = position.y + j;
				for ( char k = -3; k <= 3; k++ ) {
					vCheckpos.z = position.z + k;
					
					hashFind = dungeonList.find( position );
					if ( hashFind != dungeonList.end() )
					{
						if ( hashFind->second != NULL )
						{
							if ( std::find( dungeonLocalList.begin(), dungeonLocalList.end(), hashFind->second ) == dungeonLocalList.end() )
							{
								dungeonLocalList.push_back( hashFind->second );
							}
						}
					}
					else
					{
						// Check if there's an associated dungeon file
						sprintf( tempstring, "%s.dungeons/%d_%d_%d\0",
							CGameSettings::Active()->GetTerrainSaveDir().c_str(),
							vCheckpos.x,vCheckpos.y,vCheckpos.z );
						if ( oCheckfile.Exists( tempstring ) ) { // If it exists, then load it to the list
							//printf( "loading file %s\n", tempstring );
							// Need a loader
							//pTargetDungeon = //loader
							oCheckfile.Open( tempstring, CBinaryFile::IO_READ );
							pTargetDungeon = CDungeonBase::load( oCheckfile, this );
							oCheckfile.Close();
							// Add it to the total list
							if ( pTargetDungeon && (pTargetDungeon->m_centerindex == vCheckpos) ) { // MISMATCHED INDEX HERE.
								dungeonList[vCheckpos] = pTargetDungeon;
								dungeonLocalList.push_back( pTargetDungeon );
							}
							else {
								Debug::Console->PrintError( "177: Either a dungeon couldn't be loaded or its position didn't match.\n" );
								printf( "  dungeon file %s\n", tempstring );
							}
						}
					}
				}
			}
		}
	}
	
	// Get elevation
	elevation = TerraGen_1p_GetElevation( position );
	// Grab the terrain type
	char terType = TerraGen_priv_GetType( Vector3d( (ftype)position.x, (ftype)position.y, (ftype)position.z ) );

	// Call init on all the dungeons to get generation tables
	vector<ftype> weights;
	ftype maxweight = 0.0f;
	ftype weight, countweight;
	for ( uint i = 0; i < dungeonSamplerList.size(); ++i )
	{
		if ( i != 0 ) {
			dungeonSamplerList[i]->Initialize( position );
			// Based on the dungeon's table and the surrounding area, generate a weight
			weight = 1.0f; // Start with 1
			switch ( dungeonSamplerList[i]->m_gentable.element ) {
				case Dungeon::ElementNeutral:
					if ( terType == TER_OCEAN || terType == TER_ISLANDS || terType == TER_SPIRES || terType == TER_THE_EDGE ) {
						weight -= 0.5f;
					}
					break;
				case Dungeon::ElementOcean:
					if ( terType == TER_OCEAN || terType == TER_ISLANDS || terType == TER_SPIRES ) {
						weight += 1.0f;
					}
					break;
				default: break;
			}
			weights.push_back( weight );
			maxweight += weight;
		}
		else {
			// Based on the density of the surrounding area, increase the weight of no dungeon
			weight = (dungeonSamplerList.size()-1)*(	// make chances based on percent of list size
					9.0f 													// base chance
				+ 3.9f*dungeonLocalList.size()								// less dungeons when dungeons nearby
				+ max<ftype>( 0, (ftype)position.z-elevation )				// less dungeons above ground
				+ min<ftype>(3.2f, fabs((ftype)position.z-elevation)*0.7f)	// less dungeons away from surface
				);
			weight *= 1.4f;
			weights.push_back( weight );
			maxweight += weight;
		}
	}

	// Sample noise at current position to choose a random value
	weight = maxweight * ( noise_hhf->Unnormalize(noise_hhf->Get3D( position.z * 0.7f - 1.21f, position.y * 1.2f + 2.33f, position.x * 0.13f + 2.1f )) + 0.5f );
	countweight = 0.0f;
	pTargetDungeon = NULL;
	for ( uint i = 0; (i < dungeonSamplerList.size()) && (countweight < weight); i += 1 )
	{
		countweight += weights[i];
		if ( countweight > weight )
		{
			if ( i != 0 )
			{
				// Grab the dungeon at that point
				pTargetDungeon = Dungeon::Factory.dungeon_insts[dungeonSamplerList[i]->index](this);
				pTargetDungeon->m_gentable = dungeonSamplerList[i]->m_gentable;
				break;
			}
			else
			{	// No dungeon generated here, so skip.
				dungeonList[position] = NULL;
				return;
			}
		}
	}

	// If by act of God (happens about 20% of all boobs) a dungeon hasn't been selected and we're still here
	if ( pTargetDungeon == NULL ) {
		dungeonList[position] = NULL;
		return; // do what He asks of you and take fate into your own hands...exit manually
	}

	// The dungeon selected, set options and call generate
	pTargetDungeon->m_centerindex = position;
	pTargetDungeon->SetElevation( elevation );
	pTargetDungeon->Generate();
	// Save the dungeon
	{
		sprintf( tempstring, "%s.dungeons/%d_%d_%d\0",
			CGameSettings::Active()->GetTerrainSaveDir().c_str(),
			position.x,position.y,position.z );
		oCheckfile.Open( tempstring, CBinaryFile::IO_WRITE );
		pTargetDungeon->save( oCheckfile );
		oCheckfile.Close();
	}
	// Add it to the total dungeon list
	//dungeonList.push_back( pTargetDungeon );
	dungeonList[position] = pTargetDungeon;
}

// TerraGen_Dungeons_Excavate
// Looks at dungeons nearby and attempts to excavate them.
void CTerraGenDefault::TerraGen_Dungeons_Excavate ( CBoob * pBoob, RangeVector const& position )
{
	// Loop through all the stored dungeons
	//for ( vector<CDungeonBase*>::iterator it = dungeonList.begin(); it != dungeonList.end(); ++it )
	for ( unordered_map<RangeVector,CDungeonBase*>::iterator it = dungeonList.begin(); it != dungeonList.end(); ++it )
	{
		//Dungeon::DungeonSystem* system = &((*it)->m_system);
		//(*it)->Excavate( pBoob, position );
		if ( it->second != NULL ) {
			it->second->Excavate( pBoob, position );
		}
	}
}
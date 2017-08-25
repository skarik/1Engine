
// ======== CTerraGenDefault_Rivers =======
// This is the definition for the first pass river generation.
// This stuff is such an asspull.

#include "CTerraGenDefault.h"
#include "CGameSettings.h"
#include "CBinaryFile.h"
#include "FileUtils.h"

using std::cout;
using std::endl;
using std::max;
using std::min;

// =======================================
// == Generation Algorithm for RRIIVERS ==
// =======================================
void CTerraGenDefault::TerraGenRiverPass ( CBoob * pBoob, const RangeVector& position )
{
	// Initial feature state
	/*if ( iFeatureCount == uint32_t(-1) )
	{
		char stemp_fn [256];
		sprintf( stemp_fn, "%s.regions/features.state", CGameSettings::Active()->GetTerrainSaveDir().c_str() );
		//cout << "FILENAME: " << stemp_fn << endl;
		// Load feature count from file
		CBinaryFile featureFile;
		featureFile.Open( stemp_fn, CBinaryFile::IO_READ );
		if ( !featureFile.IsOpen() ) {
			iFeatureCount = 0;
			featureFile.Open( stemp_fn, CBinaryFile::IO_WRITE );
			featureFile.WriteUInt32( iFeatureCount );
		}
		else {
			iFeatureCount = featureFile.ReadUInt32();
		}
		featureFile.Close();
	}*/
	
	for ( char i = -2; i <= 2; i++ ) {
		for ( char j = -2; j <= 2; j++ ) {
			TerraGen_Rivers_Exclusify( RangeVector( (rangeint)floor(position.x/32.0)+i,(rangeint)floor(position.y/32.0)+j,0 ) );
		}
	}

	TerraGen_Rivers_Load( RangeVector( (rangeint)floor(position.x/32.0),(rangeint)floor(position.y/32.0),0 ) );

	TerraGen_Rivers_Excavate( pBoob, position );
}
// Generates features for the surrounding area
void CTerraGenDefault::TerraGen_Rivers_Exclusify ( const RangeVector& position )
{
	bool needsGenerate = true;
	bool fileExists = false;
	char stemp_fn [256];
	CBinaryFile featureFile;

	sprintf( stemp_fn, "%s.regions/features_%d_%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), position.x, position.y );
	// First check for feature file
	featureFile.Open( stemp_fn, CBinaryFile::IO_READ );
	if ( featureFile.IsOpen() )
	{
		fileExists = true;
		// File exists, but must check for first bool signifying 'is generated.'
		if ( featureFile.ReadUInt32() == 1 )
		{
			needsGenerate = false;
		}
		featureFile.Close();
	}

	// Generate all features in a 32x32 area
	if ( needsGenerate )
	{
		rangeint posx = position.x*32;
		rangeint posy = position.y*32;
		rangeint tposx, tposy;
		for ( char i = 0; i < 32; i += 2 ) {
			for ( char j = 0; j < 32; j += 2 ) {
				tposx = posx+i;
				tposy = posy+i;
				TerraGen_Rivers_Generate ( tposx, tposy );

			}
		}
		
		if ( fileExists )
		{
			// Set current place generated to true
			featureFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
			fseek( featureFile.GetFILE(), 0, SEEK_SET );
			featureFile.WriteUInt32( 1 );
			featureFile.Close();
		}
		else
		{
			// Set current place generated to true
			featureFile.Open( stemp_fn, CBinaryFile::IO_WRITE );
			featureFile.WriteUInt32( 1 );
			featureFile.Close();
		}
	}
}
// Generates rivers (or possibly does not.)
void CTerraGenDefault::TerraGen_Rivers_Generate ( const rangeint posx, const rangeint posy )
{
	ftype spawnChance = 0;
	ftype spawnSample;
	ftype terraElevation;
	Vector2d vOffgrid;
	uchar terraTerra;
	uchar terraBiome;

	// Create off-grid position
	vOffgrid = Vector2d(
		posx + noise->Get( 1.2f-posx*3.1f+posy*7.1f, 1.3f-posy*3.2f )*3.0f,
		posy + noise->Get( 1.3f-posy*2.3f, 1.4f-posx*1.3f-posy*8.3f )*3.0f );
	// Grab terrain info at off-grid position
	terraBiome		= TerraGen_priv_GetBiome( Vector3d(vOffgrid) );
	terraElevation	= fCurrentElevation;
	terraTerra		= iCurrentTerraType;

	// Generate spawn chance
	spawnChance = std::min<ftype>( (terraElevation-2.0f)*0.1f, 0.75f );
	switch ( terraTerra ) {
		case TER_MOUNTAINS:	spawnChance += 0.7f; break;
		case TER_HILLLANDS:	spawnChance += 0.3f; break;
		case TER_GLACIER:	spawnChance += 0.2f; break;
		case TER_DESERT:
		case TER_BADLANDS:
		case TER_FLATLANDS: spawnChance -= 0.1f; break;
		case TER_THE_EDGE:
		case TER_ISLANDS:
		case TER_SPIRES:
		case TER_OCEAN:		spawnChance -= 1.0f; break;
	}
	switch ( terraBiome ) {
		case BIO_EVERGREEN:
		case BIO_TEMPERATE:	spawnChance += 0.1f; break;
		case BIO_RAINFOREST:
		case BIO_TROPICAL:
		case BIO_WET_OUTLANDS:
		case BIO_MOIST:		spawnChance += 0.2f; break;
	}
	//spawnChance -= 0.1f;
	spawnChance *= 0.7f;

	// Decrease spawn chance by noise function
	spawnChance *= noise->Unnormalize( noise->Get( vMainSamplerOffset.y+posx*5.21f, posy*4.31f ) )+0.5f;

	// Check for spawning
	spawnSample = noise_hf->Unnormalize( noise_hf->Get( vMainSamplerOffset.x+posy*0.89f, posx*0.79f ) )+0.5f;
	if ( spawnSample < spawnChance )
	{
		// River spawned!
		TerraRiver	river;
		Ray			riverPart;
		bool		makeRiver = true;
		ftype		riverWidth;
		ftype		riverDeltaWidth;
		ftype		riverSegmentLength;
		ftype		riverLife;

		// Give river and index and increment feature count
		river.index = iFeatureCount;
		iFeatureCount += 1;

		// Set initial start position and width
		river.startVector = RangeVector( posx,posy,0 );
		riverPart.pos = Vector3d( vOffgrid, TerraGen_1p_GetElevation( Vector3d(vOffgrid) ) );
		riverWidth = noise->Get( vOffgrid.y, vOffgrid.x )*0.25f + 0.25f;
		riverDeltaWidth = 0.1f;
		riverSegmentLength = 0.7f;
		riverLife = 1.0f;
		
		// Simulate river creation using elevation gradient
		while ( makeRiver )
		{
			// Sample around the area of the current part and make a gradient.
			// 4x low res sample to make approximate slope gradient.
			Vector3d sourcepoint = riverPart.pos;
			
			Vector3d castpoint;
			Vector3d gradient (0,0,0);
			for ( uint i = 0; i < 4; ++i )
			{
				castpoint.x = sourcepoint.x + (ftype)cos( i*PI*0.5 );
				castpoint.y = sourcepoint.y + (ftype)sin( i*PI*0.5 );
				castpoint.z = TerraGen_1p_GetElevation( castpoint );
				castpoint.z = castpoint.z-sourcepoint.z;
				gradient.x -= (castpoint.x-sourcepoint.x)*castpoint.z;
				gradient.y -= (castpoint.y-sourcepoint.y)*castpoint.z;
				gradient.z += fabs(castpoint.z)*0.3f;
			}
			gradient.normalize();

			// Change gradient based on previous
			if ( river.riverSegments.size() > 0 ) {
				ftype dotp = gradient.dot( river.riverSegments[river.riverSegments.size()-1].dir.normal() );
				if ( dotp > 0.4f ) {
					riverSegmentLength += (dotp-0.3f)*0.6f;
				}
				else {
					riverSegmentLength += (dotp-0.5f)*1.1f;
				}
				gradient.x += river.riverSegments[river.riverSegments.size()-1].dir.x*0.6f;
				gradient.y += river.riverSegments[river.riverSegments.size()-1].dir.y*0.6f;
				gradient.normalize();
			}

			// Change gradient length
			riverSegmentLength = std::max<ftype>( std::min<ftype>( riverSegmentLength, 1.2f ), 0.2f );
			gradient *= riverSegmentLength;
			// Change river width
			riverDeltaWidth += noise->Get( riverPart.pos.x, riverPart.pos.y ) * 0.2f;
			riverDeltaWidth = std::max<ftype>( std::min<ftype>( riverDeltaWidth, 0.3f ), -0.3f );
			riverWidth += riverDeltaWidth;
			riverWidth = std::max<ftype>( std::min<ftype>( riverWidth, 0.07f ), 0.6f );

			// Do terrain and biome specific changes
			switch ( terraTerra ) {
				case TER_MOUNTAINS: riverSegmentLength -= 0.03f; break;
				case TER_FLATLANDS: riverSegmentLength += 0.04f; break;
				case TER_ISLANDS:
				case TER_SPIRES:	riverWidth += 0.1f;
									riverSegmentLength += 0.04f; break;
				case TER_OCEAN:		riverWidth += 0.3f;
									makeRiver = false;	break;
			}
			// Do distance changes
			if (( fabs(riverPart.pos.x - vOffgrid.x) > 24 )||( fabs(riverPart.pos.y - vOffgrid.y) > 24 )) {
				if (( fabs(riverPart.pos.x - vOffgrid.x) < 50 )||( fabs(riverPart.pos.y - vOffgrid.y) < 50 )) {
					riverSegmentLength += 0.04f;
					riverWidth += 0.01f;
				}
				else {
					riverSegmentLength -= 0.01f;
					riverWidth -= 0.01f;
				}
			}
			// Do elevation changes
			if ( riverPart.pos.z < 0 ) {
				riverLife -= 0.1f;
				riverWidth -= 0.1f;
				riverSegmentLength += 0.02f;
				if ( riverLife < 0 ) {
					makeRiver = false;
				}
			}

			// Using the gradient, extend the river part
			riverPart.dir = Vector3d( gradient.x, gradient.y, gradient.z );
			// Calculate proper elevation
			ftype trueElevation = std::min<ftype>( riverPart.pos.z - 0.03f, TerraGen_1p_GetElevation( riverPart.pos+riverPart.dir ) ); 
			terraTerra		= iCurrentTerraType;
			// Set proper stream width
			riverPart.dir.z = riverWidth;
			// Add new river part
			river.riverSegments.push_back( riverPart );
			// Add source river size
			river.riverSegmentSources.push_back( RangeVector( rangeint(riverPart.pos.x),rangeint(riverPart.pos.y),0 ) );

			// Set next river part
			riverPart.pos += riverPart.dir;
			riverPart.pos.z = trueElevation;

			// If the next part is a little too far, and we're still going
			//  then we probably want to stop (TODO: make a lake)
			if (( fabs(riverPart.pos.x - vOffgrid.x) > 60 )||( fabs(riverPart.pos.y - vOffgrid.y) > 60 )) {
				makeRiver = false;
			}
		}

		// Loop through all the segment sources and add areas based on that
		for ( uint i = 0; i < river.riverSegmentSources.size(); ++i )
		{
			RangeVector areaIndex ( 
				(rangeint)floor(river.riverSegmentSources[i].x/32.0),
				(rangeint)floor(river.riverSegmentSources[i].y/32.0),0 );
			vector<RangeVector>::iterator findResult = std::find( river.areas.begin(), river.areas.end(), areaIndex );
			if ( findResult == river.areas.end() ) {
				river.areas.push_back( areaIndex );
			}
		}

		// Perform IO on now-generated river.
		char stemp_fn [256];
		CBinaryFile featureFile;
		// Add this last river to the target features
		for ( uint n = 0; n < river.areas.size(); ++n )
		{
			// check if file exists.
			sprintf( stemp_fn, "%s.regions/features_%d_%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), river.areas[n].x, river.areas[n].y );
			featureFile.Open( stemp_fn, CBinaryFile::IO_READ );

			// if it exists, search first for self
			if ( featureFile.IsOpen() )
			{
				bool foundSelf = false;
				// Read in the entries
				fseek( featureFile.GetFILE(), 0, SEEK_END );
				int32_t fileLength = ftell( featureFile.GetFILE() );
				fseek( featureFile.GetFILE(), 0, SEEK_SET );

				featureFile.ReadUInt32();
				TerraRiver tempRiver;
				while ( (!foundSelf) && (ftell( featureFile.GetFILE() ) < fileLength) )
				{
					uint32_t index = featureFile.ReadUInt32();
					if ( index == river.index ) {
						foundSelf = true;
					}
					uint32_t stride = featureFile.ReadUInt32();
					RangeVector sourcePosition;
					featureFile.ReadData( (char*)(&sourcePosition), 12 );
					if ( sourcePosition == river.startVector ) {
						foundSelf = true;
					}
					fseek( featureFile.GetFILE(), stride-(8+12), SEEK_CUR );
				}

				// if cant find self, append to it
				if ( !foundSelf )
				{
					featureFile.Close();

					featureFile.Open( stemp_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
					fseek( featureFile.GetFILE(), 0, SEEK_END );
					// Write self
					TerraGen_Rivers_Write( river, featureFile );
				}
				else
				{
					//cout << "!!! Should never get here. !!!" << endl;
				}
			}
			// if it doesn't exist, create it, set generated bool at front to false, add self
			else
			{
				featureFile.Open( stemp_fn, CBinaryFile::IO_WRITE );
				featureFile.WriteUInt32( 0 );
				// Write self
				TerraGen_Rivers_Write( river, featureFile );
			}
			featureFile.Close();
		}
	}

}
// Loads rivers from the target feature map if not loaded
void CTerraGenDefault::TerraGen_Rivers_Load ( const RangeVector& position )
{
	if ( vCurrentFeatureIndex != position )
	{
		char stemp_fn [256];
		CBinaryFile featureFile;

		sprintf( stemp_fn, "%s.regions/features_%d_%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), position.x, position.y );
		// Load features
		featureFile.Open( stemp_fn, CBinaryFile::IO_READ );

		// First clear feature list
		activeRivers.clear();

		// Set new feature index
		vCurrentFeatureIndex = position;

		// Read rivers
		featureFile.ReadUInt32();
		while ( !feof( featureFile.GetFILE() ) )
		{
			TerraRiver river;
			if ( TerraGen_Rivers_Read ( river, featureFile ) ) {
				activeRivers.push_back( river );
			}
		}
	}
}
// TerraGen_Rivers_Read()
// Reads river entry from file. If can not load valid entry, returns false.
// Returns true on success.
bool CTerraGenDefault::TerraGen_Rivers_Read ( TerraRiver& nRiver, CBinaryFile& nFile )
{
	uint32_t maxsize;

	nRiver.index = nFile.ReadUInt32();
	nFile.ReadUInt32(); // stride

	if ( feof(nFile.GetFILE()) ) return false;

	nFile.ReadData( (char*)(&nRiver.startVector), 12 ); // position

	maxsize = nFile.ReadUInt32();
	for ( uint i = 0; i < maxsize; ++i )
	{
		Ray segment;
		nFile.ReadData( (char*)(&segment), 24 );
		nRiver.riverSegments.push_back( segment );
	}
	maxsize = nFile.ReadUInt32();
	for ( uint i = 0; i < maxsize; ++i )
	{
		RangeVector point;
		nFile.ReadData( (char*)(&point), 12 );
		nRiver.riverSegmentSources.push_back( point );
	}

	if ( feof(nFile.GetFILE()) ) return false;

	maxsize = nFile.ReadUInt32();
	for ( uint i = 0; i < maxsize; ++i )
	{
		RangeVector point;
		nFile.ReadData( (char*)(&point), 12 );
		nRiver.areas.push_back( point );
	}

	return true;
}
void CTerraGenDefault::TerraGen_Rivers_Write ( const TerraRiver& nRiver, CBinaryFile& nFile )
{
	// Calculate river stride
	uint32_t stride = 0;

	stride += 8;
	stride += 12;
	stride += 4 + nRiver.riverSegments.size() * 24;
	stride += 4 + nRiver.riverSegmentSources.size() * 12;
	stride += 4 + nRiver.areas.size() * 12;

	// Write river data
	nFile.WriteUInt32( nRiver.index );
	nFile.WriteUInt32( stride );
	nFile.WriteData( (char*)(&nRiver.startVector), 12 );
	nFile.WriteUInt32( nRiver.riverSegments.size() );
	nFile.WriteData( (char*)(&(nRiver.riverSegments[0])), 24*nRiver.riverSegments.size() );
	nFile.WriteUInt32( nRiver.riverSegmentSources.size() );
	nFile.WriteData( (char*)(&(nRiver.riverSegmentSources[0])), 12*nRiver.riverSegmentSources.size() );
	nFile.WriteUInt32( nRiver.areas.size() );
	nFile.WriteData( (char*)(&(nRiver.areas[0])), 12*nRiver.areas.size() );
}

// Excavates rivers in the area
void CTerraGenDefault::TerraGen_Rivers_Excavate ( CBoob * pBoob, const RangeVector& position )
{
	// At this point, it can be assumed that vector 'activeRivers' has a list of all the active rivers.
	// So, at this point, all the rivers are to be looped through and excavated.
	for ( uint ri = 0; ri < activeRivers.size(); ++ri )
	{
		TerraRiver& river = activeRivers[ri];
		// Loop through the river, and find first river segment that matches position
		for ( uint seg = 0; seg < river.riverSegmentSources.size(); ++seg )
		{
			//if ( position == river.riverSegmentSources[seg] )
			if (( abs( position.x - river.riverSegmentSources[seg].x ) <= 1 )||( abs( position.y - river.riverSegmentSources[seg].y ) <= 1 ))
			{
				// Given that, excavate the 2 previous and 2 next segments, a total of 5 segments.
				uint min_val = (uint)std::max<int>( 0,((int)(seg))-2 );
				uint max_val = std::min<uint>( river.riverSegmentSources.size(), seg+3 );
				for ( uint target_seg = min_val; target_seg < max_val; ++target_seg )
				{
					// Create the ray on the terrain for the river.
					Ray riverRay;
					riverRay.pos = river.riverSegments[target_seg].pos;
					riverRay.dir = river.riverSegments[target_seg].dir;
					riverRay.dir.z = std::min<ftype>( -0.03f, TerraGen_1p_GetElevation( river.riverSegments[target_seg].pos+river.riverSegments[target_seg].dir )-river.riverSegments[target_seg].pos.z );

					char i,j;
					short k;
					Vector3d blockPosition, a_to_p, a_to_b;
					ftype atb2, atp_dot_atb, t, dist_to_river_center;
					Vector3d pos;
					// Loop through the blocks and remove target points
					for ( i = 0; i < 8; i += 1 )
					{
						for ( j = 0; j < 8; j += 1 )
						{
							for ( k = 0; k < 512; k += 1 )
							{
								blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
								blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
								blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

								// Calculate distance to line segment (water)
								a_to_p = blockPosition-(riverRay.pos);
								a_to_b = riverRay.dir;
								atb2 = a_to_b.sqrMagnitude();
								atp_dot_atb = a_to_p.dot( a_to_b );
								t = min<ftype>( 1.0f, max<ftype>( 0.0f, atp_dot_atb / atb2  ) );
								pos = riverRay.pos + a_to_b*t;
								dist_to_river_center = (pos-blockPosition).magnitude();

								// If the distance is close enough, then bam, excavate
								if ( dist_to_river_center <= river.riverSegments[target_seg].dir.z )
								{
									pBoob->data[i].data[j].data[k].block = EB_WATER;
								}
							}
						}
					}
				}

				// Of course, at this success, break out of the seg loop
				seg = (uint)(-10); //evaluates to MAX_INTEGER-9
			}
		}
	}
}
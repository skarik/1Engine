
#include "TownGen_A0.h"

#include "CTownManager.h"
#include "CTerraGenDefault.h"
#include "BlockInfo.h"

#include "Math.h"
#include "Zones.h"

using std::cout;
using std::endl;

void TownGen_A0::Generate ( void )
{
	sPatternLandmark town_center;
	town_center.name = "Town Center";
	town_center.radius = 1.0f;
	town_center.position = Vector3d( areaPosition.x+0.5f,areaPosition.y+0.5f,areaElevation );
	genTown->landmarks.push_back( town_center );
	// Generate roads.
	GenerateRoads();
		cout << "  with " << genTown->roads.size() << " roads" << endl;
	// Generate locations.
	GenerateLocations();
		cout << "  with " << genTown->landmarks.size()+genTown->instances.size() << " locations" << endl;
	// Generate buildings.
	GenerateBuildings();
		cout << "   and " << genTown->buildings.size() << " buildings" << endl;
		cout << "Region is " << genTownEntry->region << endl;
}


// Generation

// 'Squarizes' a circle
void CircleToSquare ( Vector2d& rect, ftype radius )
{
	if ( fabs( rect.x ) > fabs( rect.y ) )
	{	// X is the major axis, set via X
		ftype next_x = radius * Math.sgn<ftype>( rect.x );
		rect.y = fabs(next_x * (rect.y/rect.x)) * Math.sgn<ftype>( rect.y );
		rect.x = next_x;
	}
	else 
	{	// Y is the major axis, set via Y
		ftype next_y = radius * Math.sgn<ftype>( rect.y );
		rect.x = fabs(next_y * (rect.x/rect.y)) * Math.sgn<ftype>( rect.x );
		rect.y = next_y;
	}
}
// Returns a rough estimate of order of a point on a circle
ftype GetCircular ( const Vector2d& vect )
{
	if ( vect.y >= 0 ) {
		return (1-vect.x); // from 0 to 2
	}
	else {
		return (vect.x+3); // from 2 to 4
	}
}
// Returns if a comes before b
bool CircularSort ( Vector3d& vect_a, Vector3d& vect_b )
{ 
	Vector2d a (vect_a.x,vect_a.y);
	Vector2d b (vect_b.x,vect_b.y);
	return (GetCircular(a.normal()) < GetCircular(b.normal()));
}
// Returns the probability chance for the values
ftype GetProbabilityAngleChance ( const Vector2d& vect )
{
	Vector2d temp = vect.normal();
	ftype angle = (ftype)radtodeg( atan2( vect.y, vect.x ) );
	if ( angle < 0 ) {
		angle = -angle;
	}
	angle = fmodf( angle, 90 );
	if ( fmodf( angle,90 ) <= 2.0f || fmodf( angle,90 ) >= 88.0f ) {
		return 2;
	}
	else if ( fmodf( angle,45 ) <= 2.0f || fmodf( angle,45 ) >= 43.0f ) {
		return 1;
	}
	else if ( fmodf( angle,22.5f ) <= 2.0f || fmodf( angle,22.5f ) >= 20.5f ) {
		return 0.5f;
	}
	else if ( fmodf( angle,11.25f ) <= 2.0f || fmodf( angle,11.25f ) >= 9.25f ) {
		return 0.25f;
	}
	else if ( fmodf( angle,5.625f ) <= 1.0f || fmodf( angle,5.625f ) >= 4.625f ) {
		return 0.125f;
	}
	return 0.0f;
}

//	GenerateRoads
// First comes up with road traits. After traits are randomly chosen, will generate road map.
void TownGen_A0::GenerateRoads ( void )
{
	// Get the start position.
	// Loop through rings, starting at town center, going to town outskirts.
		// Loop through paths generated on the previous ring.
			// Create path to current ring.
		// Create paths running between paths of this ring.
		// Save number of paths generated this run.
	// Create final shortcut paths (with random chance against order)

	// Note that the final algorithm doesn't matter too much. What does matter is that the towns are kept fairly small.
	// They need to remain small or the sense of progression in the world is lost.

	// Get the start position.
	Vector3d startPosition = genTown->landmarks[0].position;
	// Get the start elevation and terra
	//uchar startTerra = generator->TerraGen_priv_GetType( startPosition );
	//ftype startElevation = generator->fCurrentElevation;
	ftype startElevation = generator->TerraGen_1p_GetElevation( startPosition );
	uchar startTerra = generator->iCurrentTerraType;

	// Based on starting area and randomizer, generate road traits.
	ftype verticality, density, circularity, sublink_amount, majorlink_amount, deadend_acceptance;
	{
		// Verticality is typically extremely low
		verticality = fabs(generator->noise->Get3D( startPosition.x, startPosition.z, startPosition.y ))*0.4f;
		// However, in certain areas, it must be extremely high to match the terrain
		if (( areaTerra == TER_SPIRES )||( areaTerra == TER_MOUNTAINS )) {
			verticality += 0.94f;
		}
		else if ( areaTerra == TER_ISLANDS ) {
			verticality += 0.3f;
		}

		// Density is a mid-range value
		density = generator->noise_hhf->Get3D( startPosition.y, startPosition.z, startPosition.x ) * 0.4f + 0.4f;
		// But deserts, badlands, and harsh weather lend themselves to more spread towns
		if (( areaTerra == TER_DESERT )||( areaTerra == TER_BADLANDS )) {
			density *= 0.67f;
		}
		else if (( areaBiome == BIO_COLD_DESERT )||( areaBiome == BIO_TUNDRA )) {
			density *= 0.5f;
		}

		// Circularity is a full ranging value that varies slowly by region
		circularity = generator->noise_biome->Get3D( startPosition.z, startPosition.x*0.8f, startPosition.y*0.7f ) + 0.5f;
		// But it has a very low chance to be half way, and the more dense, the less wishywashy.
		circularity += ((circularity < 0.5f) ? -1 : 1) * ( density+0.2f );
		circularity = std::min<ftype>( std::max<ftype>( circularity, 0 ), 1 );
	
		circularity = 1.0f; // temp override

		// Sublink amount is stupid
		sublink_amount = generator->noise_hf->Get3D( startPosition.x*7.3f, startPosition.y*2.4f, startPosition.z*1.5f ) + 0.5f;

		// Majorlink amount is super stupid
		majorlink_amount = generator->noise_hf->Get3D( startPosition.z*4.2f, startPosition.y*1.1f, startPosition.x*4.1f ) + 0.5f;
		
		// Deadend acceptance is another mid-range value. Usually, dead-ends will be okay.
		deadend_acceptance = generator->noise_hf->Get3D( startPosition.y*2.3f, startPosition.z*3.7f, startPosition.x*1.1f ) + 0.5f;
	
		// Save town generation properties
		genTown->properties.circularity = circularity;
		genTown->properties.density		= density;
		genTown->properties.verticality	= verticality;
		genTown->properties.sublink_amount		= sublink_amount;
		genTown->properties.majorlink_amount	= majorlink_amount;
		genTown->properties.deadend_acceptance	= deadend_acceptance;
	}

	// Loop through the rings, starting at the town center and going to town outskirts.
	{
		// Ignore outlyer cases (initial case) for now.
		vector<Vector3d> pointList;
		int ringIndex = 1;
		int previousRoadCount = 1;		// Temp road count, used in conjunction with density.
		int hardPreviousRoadCount = 1;	// Count of all roads that connect with the previous ring.
		ftype ringWidth = 0.57f;
		ftype ringCurrentWidth = 0;
		ftype targetElevation;
		uchar targetTerra;
		// While roads were generated in the last run
		while ( previousRoadCount > 0 )
		{
			// Loop through paths on previous ring
			int previousRoadHard = genTown->roads.size()-hardPreviousRoadCount;
			int previousRoadMin = genTown->roads.size()-previousRoadCount;
			int previousRoadMax = genTown->roads.size();
			
			// Set current ring width
			ringCurrentWidth += ringWidth;

			// Reset road generated count
			previousRoadCount = 0;
			hardPreviousRoadCount = 0;
			// Reset the point list
			pointList.clear();

			if ( ringIndex == 1 )
			{
				// Choose four angles to start at.
				for ( int i = 0; i < 4; ++i )
				{
					ftype angle = generator->noise->Unnormalize(generator->noise_hhf->Get3D(startPosition.x*4.3f-i*0.4f,startPosition.y*(4.3f-i*4.2f),startPosition.z*(i+0.3f)*4.3f))*16;
					angle = ftype(int(angle));
					angle = (ftype)((angle/16)*2*PI);
					Vector2d offset (
						(ftype)sin( angle ),
						(ftype)cos( angle ) );

					Vector3d targetSource = startPosition + Vector3d( offset*ringWidth );

					{
						sPatternRoad newRoad;

						// Set new road places
						newRoad.location.pos = startPosition;
						newRoad.location.dir = Vector3d( offset*ringWidth );
						// A road length that links between rings
						Vector2d dir_ci ( newRoad.location.dir.x, newRoad.location.dir.y );
						Vector2d dir_sq ( dir_ci );
						CircleToSquare( dir_sq, ringWidth );
						Vector2d dir_final = dir_sq.lerp( dir_ci, circularity ); // Change XY based on circularity.
						// Set proper road length
						newRoad.location.dir = Vector3d( dir_final, 0 );
						// Set new road width
						newRoad.size = 6.0f;
						// Set new road importance
						newRoad.importance = 5.0f;
						// Set road terrain info
						newRoad.terra = startTerra;
						newRoad.elevation = startElevation;

						bool unique = true;
						// Check previous offsets
						for ( uint j = 0; j < genTown->roads.size(); ++j ) {
							if ( Vector3d(newRoad.location.dir - genTown->roads[j].location.dir).sqrMagnitude() < 0.001f ) {
								unique = false;
							}
						}
						// Add road only if unique (dupes not allowed!)
						if ( unique )
						{
							// Add new road to list
							genTown->roads.push_back( newRoad );
							// Add endpoint of the road to the point list
							pointList.push_back( newRoad.location.pos + newRoad.location.dir );
							// Increment because it's another road generated
							previousRoadCount += 1;
							hardPreviousRoadCount += 1;
						}
					}
				}
			}
			else
			{
				// Loop through all last ring's roads.
				for ( int i = previousRoadMin; i < previousRoadMax; ++i )
				{	// Create the links to the current ring.
					// Look at the endpoint of the current road. Count the amount of roads that share the endpoint or startpoint.
					int linkCount = 0;
					Vector3d targetSource = genTown->roads[i].location.pos + genTown->roads[i].location.dir;
					for ( int j = previousRoadHard; j < previousRoadMax; ++j ) {
						if (( targetSource == genTown->roads[j].location.pos )
							||( targetSource == (genTown->roads[j].location.pos+genTown->roads[j].location.dir) )) {
							linkCount += 1;
						}
					}

					// Get base chance multiplier
					ftype baseChance = GetProbabilityAngleChance( Vector2d(targetSource.x-startPosition.x,targetSource.y-startPosition.y) );

					// Now do random bullshit math.
					ftype validChance = 0.32f-baseChance*(majorlink_amount);
					validChance -= sqr( std::min<int>(linkCount,6)-1 )*0.2f;
						// Reduce chance based on density
						validChance += ((abs(ringIndex-(density*2.5f))*ringWidth) / density) * 0.3f;
						// Increase chance based on dead ends and randomized acceptance rate
						validChance -= std::max<ftype>(
							(1-linkCount) * (1-deadend_acceptance) * 2.0f *
							generator->noise_hf->Unnormalize( generator->noise_hf->Get3D(targetSource.x,targetSource.z,targetSource.y) ), 0.0f );
						// Reduce chance based on number of roads already made
						validChance += genTown->roads.size() / 400.0f;
						// Reduce chance based on mismatched terrain
						//targetTerra = generator->TerraGen_priv_GetType( targetSource );
						//targetElevation = generator->fCurrentElevation;
						targetElevation = generator->TerraGen_1p_GetElevation( targetSource );
						targetTerra = generator->iCurrentTerraType;
						if ( targetTerra == TER_SPIRES ) { // Quickly modify target elevation
							targetElevation += 1.4f*0.5f;
						}
						validChance += (Zones.TerraCompare( startTerra, targetTerra )-0.2f) * 0.6f;


					// Increase spawn chance to give proper weighting
					validChance *= 0.5f;
					if ( generator->noise_hhf->Get3D(targetSource.x,targetSource.y,targetSource.z) > validChance )
					{
						sPatternRoad newRoad;

						// Set new road places
						newRoad.location.pos = genTown->roads[i].location.pos + genTown->roads[i].location.dir;
						newRoad.location.dir = (newRoad.location.pos - startPosition).normal() * ringWidth;
						// A road length that links between rings
						Vector2d dir_ci ( newRoad.location.dir.x, newRoad.location.dir.y );
						Vector2d dir_sq ( dir_ci );
						CircleToSquare( dir_sq, ringWidth );
						Vector2d dir_final = dir_sq.lerp( dir_ci, circularity ); // Change XY based on circularity.
						// Set the proper road length
						newRoad.location.dir = Vector3d( dir_final, 0 );
						// Add vertical offsets with about 50-70% random base chance against verticality
						if ( generator->noise_hf->Get3D(targetSource.z*22.3f,targetSource.y*22.3f,targetSource.x*22.3f) > -0.05f ) {
							// Add an offset based off of verticality
							newRoad.location.dir.z = verticality*generator->noise->Unnormalize(generator->noise->Get(targetSource.y*-1.1f,targetSource.x*-1.1f))*2.0f;
							if ( fabs(newRoad.location.dir.z) > ringWidth*0.5f ) {	// Limit between 50% and 30% of the ring width
								newRoad.location.dir.z = ringWidth*0.5f*Math.sgn<ftype>(newRoad.location.dir.z);
							}
							else if ( fabs(newRoad.location.dir.z) < ringWidth*0.3f ) {
								newRoad.location.dir.z = ringWidth*0.3f*Math.sgn<ftype>(newRoad.location.dir.z);
							}
						}
						// Set target height
						newRoad.location.dir.z += ((targetElevation - newRoad.location.pos.z) - newRoad.location.dir.z)*std::max<ftype>(0,std::min<ftype>(1,1-verticality));
						// Set road width
						newRoad.size = std::max<ftype>( 3, 6.0f-ringIndex*1.2f );
						// Set road importance
						newRoad.importance = std::max<ftype>( 0, 4.8f-ringIndex*0.9f );
						// Set road terrain info
						newRoad.terra = targetTerra;
						newRoad.elevation = targetElevation;
						
						// Add new road to list
						genTown->roads.push_back( newRoad );
						// Add endpoint of the road to the point list
						pointList.push_back( newRoad.location.pos + newRoad.location.dir );
						// Increment because it's another road generated
						previousRoadCount += 1;
						hardPreviousRoadCount += 1;
					}
				}
			}

			// Loop through all the ring link paths
			previousRoadMin = genTown->roads.size()-previousRoadCount;
			previousRoadMax = genTown->roads.size();

			pointList.clear();

			// Add some generated points from these endpoints
			/*for ( int i = previousRoadMin; i < previousRoadMax; ++i )
			{
				Vector3d sourcePoint = genTown->roads[i].location.pos + genTown->roads[i].location.dir;

				// Now, offset the shit out of this new point.
				for ( int j = -3; j <= 3; ++j )
				{
					Vector3d currentPoint = sourcePoint;
					Vector2d localPoint ( currentPoint.x-startPosition.x,currentPoint.y-startPosition.y );

					// Rotate point around center
					Matrix2x2 rotation;
					rotation.setRotation( j * 1.40625f * 2.0f );
					localPoint = localPoint*rotation;

					// Set new point
					currentPoint = Vector3d( localPoint+Vector2d(startPosition.x,startPosition.y), currentPoint.z );

					// Get the chance of this point being used
					ftype baseChance = GetProbabilityAngleChance( localPoint ); 
					// Change change based on the major link amount
					ftype validChance = 0.28f-baseChance*(majorlink_amount)*2.0f;
						// Reduce chance based on density
						validChance += ((abs(ringIndex-(density*2.5f))*ringWidth) / density) * 0.2f;
						// Reduce chance based on number of roads already made
						validChance += genTown->roads.size() / 700.0f;
					// Scale chance downwards
					validChance *= 0.5f;
					if ( generator->noise_hhf->Get3D(currentPoint.y*3.7f,currentPoint.z*3.7f,currentPoint.x*3.7f) > validChance )
					{
						// Put the point at the correct position
						Vector2d pos_ci ( currentPoint.x, currentPoint.y );
						Vector2d pos_sq ( pos_ci );
						CircleToSquare( pos_sq, ringCurrentWidth );
						Vector2d pos_final = pos_sq.lerp( pos_ci, circularity ); // Change XY based on circularity.
						// Set point proper X+Y
						currentPoint = Vector3d( pos_final, currentPoint.z );

						// Set point proper Z (with non-proper based on verticality)
						targetElevation = generator->TerraGen_1p_GetElevation( currentPoint );
						currentPoint.z += (targetElevation - currentPoint.z)*std::max<ftype>(0,std::min<ftype>(1,1-verticality));

						// Do Z offset with 20% base chance offset by verticality
						if ( generator->noise_hf->Get3D(currentPoint.z*22.3f,currentPoint.y*22.3f,currentPoint.x*22.3f) > 0.12f ) {
							// With offset value influenced by verticality
							currentPoint.z += verticality * generator->noise->Get(currentPoint.y*-1.1f,currentPoint.x*-1.1f) * 1.1f;
						}
						
						// Add the new point
						pointList.push_back( currentPoint );
					}
				}
			}

			// Sort the endpoints
			sort( pointList.begin(), pointList.end(), CircularSort );

			// Reset road generated count
			previousRoadCount = 0;
			bool lastSegmentGenerated = false;
			// Loop through all generated points and start attaching roads
			for ( int i = 0; i < pointList.size(); ++i )
			{
				// Look at point before and point next.
				Vector3d roadPointPos = pointList[i];
				Vector3d roadPointDir;
				if ( i < pointList.size()- 1 ) {
					roadPointDir = pointList[i+1];
				}
				else {
					roadPointDir = pointList[0];
				}
				roadPointDir = roadPointDir - roadPointPos;

				// Skip if almost the same point
				if ( roadPointDir.sqrMagnitude() <= 0.004f ) {
					continue;
				}

				// Look at the start and end of the current road. Count the number of main roads that touch this road.
				int linkCount = 0;
				Vector3d roadPointPosB = roadPointPos+roadPointDir;
				for ( int j = previousRoadMin; j < previousRoadMax; ++j ) {
					if (( roadPointPos == (genTown->roads[j].location.pos+genTown->roads[j].location.dir) )
						||( roadPointPosB == (genTown->roads[j].location.pos+genTown->roads[j].location.dir) )) {
						linkCount += 1;
					}
				}

				// Do bullshit math for chance
				// Get base chance multiplier
				ftype baseChanceS = GetProbabilityAngleChance( Vector2d(roadPointPos.x-startPosition.x,roadPointPos.y-startPosition.y) );
				ftype baseChanceE = GetProbabilityAngleChance( Vector2d(roadPointPos.x+roadPointDir.x-startPosition.x,roadPointPos.y+roadPointDir.y-startPosition.y) );

				// Increase spawn chance on major fares.
				ftype validChance = 0.16f-(baseChanceS+baseChanceE)*(majorlink_amount)*0.5f;
					// Increase likeliness to generate a road if Z matches or is close.
					if ( fabs(roadPointDir.z) < 0.1f ) {
						validChance -= 0.0f; // 50% chance
					}
					else {
						validChance -= (verticality*0.1f)-0.4f; // 15 to 30% base chance
					}
					// Increase likeliness when a road has been generated already
					if ( lastSegmentGenerated ) {
						validChance -= (1-density)*0.2f;
					}
					// Decrease likeliness based on current radius and density.
					validChance += ((abs(ringIndex-(density*2.5f))*ringWidth) / density) * 0.22f;
					// Increase chance based on link count and density
					validChance -= sqr( std::min<int>(linkCount,6) )*density;
					// Reduce chance based on number of roads already made
					validChance += genTown->roads.size() / 600.0f;

				// Increase spawn chance to give proper weighting
				validChance *= 0.5f;
				if ( generator->noise_hhf->Get3D(roadPointPos.x*1.7f,roadPointPos.y*1.7f,roadPointPos.z*1.7f) > validChance )
				{
					sPatternRoad newRoad;

					newRoad.location.pos = roadPointPos;
					newRoad.location.dir = roadPointDir;

					// Add new road to list
					genTown->roads.push_back( newRoad );
					// Increment because it's another road generated
					previousRoadCount += 1;
					hardPreviousRoadCount += 1;
					lastSegmentGenerated = true;
				}
				else {
					lastSegmentGenerated = false;
				}
			}*/
			// Now, we've created another sexy ring of roads.

			// From these endpoints, start making roads.
			for ( int i = previousRoadMin; i < previousRoadMax; ++i )
			{
				Vector3d sourcePoint = genTown->roads[i].location.pos + genTown->roads[i].location.dir;
				// Choose a direction to make the road
				int dir = (generator->noise_hhf->Get3D(sourcePoint.y*3.7f,sourcePoint.z*3.7f,sourcePoint.x*3.7f)>0) ? -1 : 1;
				// Choose to move road up or down
				bool vertMove = false;
				// Do Z offset with 20% base chance offset by verticality
				ftype baseVertOffset = 0.0f;
				if ( generator->noise_hf->Get3D(sourcePoint.z*22.3f,sourcePoint.y*22.3f,sourcePoint.x*22.3f) > 0.02f-verticality*0.17f ) {
					vertMove = true;
					baseVertOffset = generator->noise_hf->Unnormalize(generator->noise_hf->Get3D(sourcePoint.y*32.3f,sourcePoint.z*32.3f,sourcePoint.x*32.3f))*(verticality+0.6f);
				}
				
				// Loop down that direction until papa says no
				Vector3d lastPoint = sourcePoint;
				Vector3d currentPoint = sourcePoint;
				bool repeat = true;
				int repeat_count = 0;
				while ( repeat )
				{
					// Save old point
					lastPoint = currentPoint;

					// Take source point and rotate it
					Vector2d localPoint ( sourcePoint.x-startPosition.x,sourcePoint.y-startPosition.y );

					// Rotate point around center
					Matrix2x2 rotation;
					//rotation.setRotation( (repeat_count+1) * 1.40625f * 4.0f );5.625
					rotation.setRotation( (repeat_count+1) * 5.625f );
					localPoint = rotation*localPoint;
					// Modify local point to be properly square
					{
						Vector2d dir_ci ( localPoint );
						Vector2d dir_sq ( dir_ci );
						CircleToSquare( dir_sq, ringCurrentWidth );
						Vector2d dir_final = dir_sq.lerp( dir_ci, circularity ); // Change XY based on circularity.
						localPoint = dir_final;
					}

					// Set current point
					currentPoint = Vector3d( localPoint.x+startPosition.x, localPoint.y+startPosition.y, lastPoint.z );
					if ( vertMove ) {
						// With offset value influenced by verticality
						currentPoint.z += baseVertOffset + (verticality * generator->noise->Get(currentPoint.y*-1.1f*0.7f,currentPoint.x*-1.1f*0.7f) * 1.1f);
					}
					// Sample terrain
					targetElevation = generator->TerraGen_1p_GetElevation( currentPoint );
					targetTerra = generator->iCurrentTerraType;
					if ( targetTerra == TER_SPIRES ) { // Quickly modify target elevation
						targetElevation += 1.4f*0.5f;
					}
					// Change point Z based on terrain
					currentPoint.z += Math.Lerp( std::max<ftype>(0,std::min<ftype>(1,verticality*1.4f)), (targetElevation - currentPoint.z), 0 );
					// Limit z distance
					ftype dist = Vector2d(currentPoint.x-lastPoint.x,currentPoint.y-lastPoint.y).magnitude();
					if ( fabs(currentPoint.z-lastPoint.z) > dist*0.3f ) { 
						currentPoint.z = lastPoint.z + (Math.sgn<ftype>(currentPoint.z-lastPoint.z)*dist*0.3f);
					}

					// Get base chance multiplier
					ftype baseChance = GetProbabilityAngleChance( Vector2d(currentPoint.x-startPosition.x,currentPoint.y-startPosition.y) );
					// Now do random bullshit math.
					ftype validChance = 0.07f-baseChance*(majorlink_amount)*0.1f;
					validChance -= std::max<int>(1-repeat_count,0)*0.2f;
						// Reduce chance based on density
						validChance += ((abs(ringIndex-(density*2.5f))*ringWidth) / density) * 0.18f;
						// Increase chance based on dead ends and randomized acceptance rate
						validChance -= std::max<ftype>(
							std::max<ftype>(1-repeat_count*0.2f,0) * (1-deadend_acceptance) * 2.0f *
							generator->noise_hf->Unnormalize( generator->noise_hf->Get3D(lastPoint.x,lastPoint.z,lastPoint.y) ), 0.0f );
						// Reduce chance based on number of roads already made
						validChance += genTown->roads.size() / 600.0f;
						// Reduce chance based on mismatched terrain
						//targetTerra = generator->TerraGen_priv_GetType( currentPoint );
						//targetElevation = generator->fCurrentElevation;
						validChance += (Zones.TerraCompare( startTerra, targetTerra )-0.2f) * 0.5f;
					// Bias chance
					validChance -= 1.4f;
					// Do the chance to stop moving
					validChance *= 0.5f;
					if ( generator->noise_hhf->Get3D(currentPoint.x,currentPoint.y,currentPoint.z) < validChance ) {
						repeat = false;
					}

					// Make the road anyways
					{
						sPatternRoad newRoad;

						// Set new road places
						newRoad.location.pos = lastPoint;
						newRoad.location.dir = currentPoint - lastPoint;
						// Set target height
						//newRoad.location.dir.z = Math.Lerp( std::max<ftype>(0,std::min<ftype>(1,verticality)), (targetElevation - newRoad.location.pos.z), newRoad.location.dir.z );
						// Set road width
						newRoad.size = std::max<ftype>( 2, 6.0f-ringIndex*1.0f );
						// Set road importance
						newRoad.importance = std::max<ftype>( 0, 5.0f-ringIndex*1.2f );
						// Set road terrain info
						newRoad.terra = targetTerra;
						newRoad.elevation = targetElevation;

						// Add new road to list
						genTown->roads.push_back( newRoad );
						// Add endpoint of the road to the point list
						//pointList.push_back( newRoad.location.pos + newRoad.location.dir );
						// Increment because it's another road generated
						previousRoadCount += 1;
						hardPreviousRoadCount += 1;
					}

					// Check all other points for an indentical endpoint
					for ( uint j = 0; j < pointList.size(); ++j )
					{
						if ( Vector3d(pointList[j].x-currentPoint.x,pointList[j].y-currentPoint.y,(pointList[j].z-currentPoint.z)*0.1f).sqrMagnitude() < 0.001f ) {
							repeat = false;
						}
					}
					// Add current point to point list if still repeating
					if (repeat) pointList.push_back( currentPoint );

					// Increment repeat count
					repeat_count += 1;
					// Quit of going too far (TOO DAMN FAR)
					if ( repeat_count >= 45 ) {
						repeat = false;
					}
				}
			}

			// And BAM, to the next ring.
			ringIndex += 1;
		}
		// END GEN
	}

	// Create the final shortcut paths (with random chance against order)
	{
			
	}

	// Look at the current town layout and set the town sizes as needed
	{
		Vector3d deltaStart, deltaEnd;
		ftype sqrMag;
		for ( uint i = 0; i < genTown->roads.size(); ++i )
		{
			deltaStart = genTown->roads[i].location.pos - startPosition;
			deltaEnd = (genTown->roads[i].location.pos+genTown->roads[i].location.dir) - startPosition;
			// Check differences in height from center to heighten town
			/*if ( ceil(fabs(deltaStart.z)) > genTownEntry->height ) {
				genTownEntry->height = ceil(fabs(deltaStart.z)+1.2f);
			}
			if ( ceil(fabs(deltaEnd.z)) > genTownEntry->height ) {
				genTownEntry->height = ceil(fabs(deltaEnd.z)+1.2f);
			}
			// Check differences in distance from center to widen town
			sqrMag = deltaStart.sqrMagnitude();
			if ( sqrMag > sqr(genTownEntry->radius) ) {
				genTownEntry->radius = sqrt(sqrMag);
			}
			sqrMag = deltaEnd.sqrMagnitude();
			if ( sqrMag > sqr(genTownEntry->radius) ) {
				genTownEntry->radius = sqrt(sqrMag);
			}*/
			if ( deltaStart.x < genTownEntry->bb_min.x ) {
				genTownEntry->bb_min.x = deltaStart.x;
			}
			if ( deltaStart.y < genTownEntry->bb_min.y ) {
				genTownEntry->bb_min.y = deltaStart.y;
			}
			if ( deltaStart.z < genTownEntry->bb_min.z ) {
				genTownEntry->bb_min.z = deltaStart.z;
			}
			if ( deltaEnd.x < genTownEntry->bb_min.x ) {
				genTownEntry->bb_min.x = deltaEnd.x;
			}
			if ( deltaEnd.y < genTownEntry->bb_min.y ) {
				genTownEntry->bb_min.y = deltaEnd.y;
			}
			if ( deltaEnd.z < genTownEntry->bb_min.z ) {
				genTownEntry->bb_min.z = deltaEnd.z;
			}

			if ( deltaStart.x > genTownEntry->bb_max.x ) {
				genTownEntry->bb_max.x = deltaStart.x;
			}
			if ( deltaStart.y > genTownEntry->bb_max.y ) {
				genTownEntry->bb_max.y = deltaStart.y;
			}
			if ( deltaStart.z > genTownEntry->bb_max.z ) {
				genTownEntry->bb_max.z = deltaStart.z;
			}
			if ( deltaEnd.x > genTownEntry->bb_max.x ) {
				genTownEntry->bb_max.x = deltaEnd.x;
			}
			if ( deltaEnd.y > genTownEntry->bb_max.y ) {
				genTownEntry->bb_max.y = deltaEnd.y;
			}
			if ( deltaEnd.z > genTownEntry->bb_max.z ) {
				genTownEntry->bb_max.z = deltaEnd.z;
			}
		}
		//genTownEntry->radius += 1.4f;
		genTownEntry->bb_min += Vector3d(-1,-1,-1);
		genTownEntry->bb_max += Vector3d(+1,+1,+1);
	}
}
//	GenerateLocations
// Generates instances and big one-shot areas in towns that aren't buildings, such as open markets or wells.
void TownGen_A0::GenerateLocations ( void )
{
	// Loop through all the roads.
		// If road enpoint has multiple links counted, then select a busy instance.
		// If road endpoint is close to center and a dead end, then select an quiet instance.
		// If road endpoint is close to a natural resource and not too busy, then select a resource instance.	
}
//	GenerateBuildings
// Places buildings, and generates rooms and layouts based on target building sizes. Will destroy certain roads.
void TownGen_A0::GenerateBuildings ( void )
{
	// Get the start position.
	Vector3d startPosition = genTown->landmarks[0].position;

	// Loop through roads
		// at random road midpoint (.3 to .7 from dist to midpoint) get chance to spawn building
		// set max building size
		// place a building and try to make it as large as possible
		// if building is too small, stop making building

	Vector3d currentRoadPos;
	Vector3d maxBuildingSize;
	Vector3d minBuildingSize;
	Vector3d backwardsExtendDir;
	Vector3d sidewaysExtendDir;
	Maths::Cubic buildingColBox;

	ftype centerDistance;

	// Loop through roads
	for ( uint roadindex = 0; roadindex < genTown->roads.size(); ++roadindex )
	{
		// Get current road
		sPatternRoad& cur_road = genTown->roads[roadindex];

		// Get random road midpoint
		currentRoadPos = cur_road.location.pos + cur_road.location.dir * (generator->noise_hf->Get3D(cur_road.location.pos.y,cur_road.location.pos.z,cur_road.location.pos.x)*0.9f+0.5f);
		centerDistance = (currentRoadPos-startPosition).magnitude();
		// at random road midpoint get chance to spawn building
		ftype validChance = -0.04f;
		validChance += centerDistance * (0.06f/genTown->properties.density); // Decrease chance away from center
			// Reduce chance based on number of roads already made
			validChance += genTown->buildings.size() / 50.0f;
			// And generally increase chance
			validChance -= 0.1f;

		if ( generator->noise_hhf->Get3D(currentRoadPos.z+23.2f,currentRoadPos.x*1.54f+643.2f,currentRoadPos.y*23.2f+1.2f) > validChance )
		{
			// set max and min building size
			maxBuildingSize.x = 10 + generator->noise_hhf->Get(currentRoadPos.x*4.7f,currentRoadPos.y*3.7f)*4.0f + (centerDistance * (0.5f/genTown->properties.density) );
			maxBuildingSize.y = 10 + generator->noise_hhf->Get(currentRoadPos.x*3.3f,currentRoadPos.y*5.3f)*4.0f + (centerDistance * (0.5f/genTown->properties.density) );
			maxBuildingSize.z = std::max<ftype>(6,24-(centerDistance * (2.0f/genTown->properties.density) ));
			minBuildingSize.x = 6 + (centerDistance * (0.5f/genTown->properties.density) );
			minBuildingSize.y = 6 + (centerDistance * (0.5f/genTown->properties.density) );
			minBuildingSize.z = 5.0f;

			maxBuildingSize *= 1/32.0f;
			minBuildingSize *= 1/32.0f;

			// place a building and try to make it as large as possible
			Vector3d startDoorPos = currentRoadPos;
			backwardsExtendDir = cur_road.location.dir.cross(Vector3d::up).normal() * ((generator->noise_hf->Get(312.2f-currentRoadPos.x,432.12f-currentRoadPos.y )>0) ? -1.0f : 1.0f);
				// Choose a random place to start
				startDoorPos += backwardsExtendDir * cur_road.size * (1/32.0f) * 1.5f;

			// Find major axis of backwards extend dir to assist in create building
			backwardsExtendDir.z = 0;
			if ( fabs(backwardsExtendDir.x) > fabs(backwardsExtendDir.y) ) {
				backwardsExtendDir.x = (ftype)Math.sgn<ftype>( backwardsExtendDir.x );
				backwardsExtendDir.y = 0;
			}
			else {
				backwardsExtendDir.y = (ftype)Math.sgn<ftype>( backwardsExtendDir.y );
				backwardsExtendDir.x = 0;
			}
			sidewaysExtendDir = backwardsExtendDir.cross(Vector3d::up);
			sidewaysExtendDir /= 32.0f;
			backwardsExtendDir /= 32.0f;

			// Prepare building
			buildingColBox.position = startDoorPos;
			buildingColBox.size = Vector3d(1/32.0f,1/32.0f,5/32.0f);

			// Start building the building
			bool building = true;
			int buildStep = 0;
			int failureCount = 0;
			Vector3d v_build_start, v_build_end;
			v_build_start = buildingColBox.position;
			v_build_end = buildingColBox.position+buildingColBox.size;
			
			while ( building )
			{
				buildStep %= 3;
				if ( buildStep == 0 ) {
					v_build_start -= sidewaysExtendDir;
				}
				else if ( buildStep == 1 ) {
					v_build_end += sidewaysExtendDir;
				}
				else if ( buildStep == 2 ) {
					v_build_end += backwardsExtendDir;
				}
				buildingColBox = Maths::Cubic::FromPosition( v_build_start,v_build_end );

				// Check for collision
				bool failure = false;
				for ( uint i = 0; i < genTown->roads.size(); ++i ) {
					if ( genTown->roads[i].importance > 2.0f ) {
						if ( (genTown->roads[i].location.pos-buildingColBox.position).sqrMagnitude() < 1.1f ) {
							if ( buildingColBox.LineCollides( Line(genTown->roads[i].location) ) ) {
								failure = true;
								i += genTown->roads.size();
							}
						}
					}
				}
				// Check for collision w/ other buildings
				if ( !failure ) {
					for ( uint i = 0; i < genTown->buildings.size(); ++i ) {
						if ( buildingColBox.CubicCollides( genTown->buildings[i].box ) ) {
							failure = true;
							i += genTown->buildings.size();
						}
					}
				}

				// Do failure cases
				if ( failure ) {
					if ( buildStep == 0 ) {
						v_build_start += sidewaysExtendDir;
					}
					else if ( buildStep == 1 ) {
						v_build_end -= sidewaysExtendDir;
					}
					else if ( buildStep == 2 ) {
						v_build_end -= backwardsExtendDir;
					}
					buildingColBox = Maths::Cubic::FromPosition( v_build_start,v_build_end );

					failureCount += 1;
				}
				buildStep += 1;
				// Stop building if can't expand
				if ( failureCount >= 3 ) {
					building = false;
				}
				// Stop building building if hit max size
				if (( buildingColBox.size.x > maxBuildingSize.x )&&( buildingColBox.size.y > maxBuildingSize.y )) {
					building = false;
				}

				// Limit the size
				if ( fabs(v_build_end.x-v_build_start.x) > maxBuildingSize.x ) {
					//v_build_end.x -= Math.sgn<ftype>( v_build_end.x-v_build_start.x )*(fabs(v_build_end.x-v_build_start.x)-maxBuildingSize.x);
					v_build_end.x = v_build_start.x + Math.sgn<ftype>( v_build_end.x-v_build_start.x )*(maxBuildingSize.x);
				}
				if ( fabs(v_build_end.y-v_build_start.y) > maxBuildingSize.y ) {
					//v_build_end.y -= Math.sgn<ftype>( v_build_end.y-v_build_start.y )*(fabs(v_build_end.y-v_build_start.y)-maxBuildingSize.y);
					v_build_end.y = v_build_start.y + Math.sgn<ftype>( v_build_end.y-v_build_start.y )*(maxBuildingSize.y);
				}
			}

			/*;
			// Randomly create building
			v_build_start += backwardsExtendDir*/

			// Get final size
			buildingColBox = Maths::Cubic::FromPosition( v_build_start,v_build_end );

			// Check that the building is in the bounds
			//if ( (buildingColBox.size.x >= minBuildingSize.x) && (buildingColBox.size.y >= minBuildingSize.y)
			//	&& (buildingColBox.size.x <= maxBuildingSize.x) && (buildingColBox.size.y <= maxBuildingSize.y) ) {
			if ( (buildingColBox.size.x >= minBuildingSize.x) && (buildingColBox.size.y >= minBuildingSize.y) ) {
				// Add it to the building list
				sPatternBuilding newBuilding;
				newBuilding.genstyle = 200;
				newBuilding.box = buildingColBox;
				newBuilding.elevation = generator->TerraGen_1p_GetElevation( newBuilding.box.center() );
				newBuilding.entrance = startDoorPos;
				{	// Modify entrance if in bad spot
					Vector3d normalExtendDir = backwardsExtendDir.normal();
					if ( true /*(newBuilding.entrance-newBuilding.box.center()).dot(Vector3d(fabs(normalExtendDir.y),fabs(normalExtendDir.x),0))
							> Vector3d(buildingColBox.size*0.3f).dot(Vector3d(fabs(normalExtendDir.y),fabs(normalExtendDir.x),0))*/ )
					{
						newBuilding.entrance += Vector3d(
							(newBuilding.box.center().x-newBuilding.entrance.x)*fabs(normalExtendDir.y)*0.9f,
							(newBuilding.box.center().y-newBuilding.entrance.y)*fabs(normalExtendDir.x)*0.9f, 0 );
					}
					newBuilding.entrance.z = newBuilding.box.position.z;
				}

				newBuilding.entrance_dir = backwardsExtendDir.normal() * cur_road.size * (1/32.0f);
				genTown->buildings.push_back( newBuilding );
			}
		}
		// Continue!
	}
}

core_require( "default_races/races.js" );

// Returns a table with the race values
function getRaceValues ( raceIndex )
{
	if ( !raceIndex ) {
		print( "Bad race index!" );
	}
	raceTable = RACES.races[raceIndex];
	return raceTable; // sexy
}
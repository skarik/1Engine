require( "default_races" );

-- Returns a table with the race values
function getRaceValues ( raceIndex )
	if ( raceIndex == nil ) then
		print( "Bad race index!" );
	end
	raceTable = RACES.races[raceIndex];
	return raceTable;	-- BOOM
end
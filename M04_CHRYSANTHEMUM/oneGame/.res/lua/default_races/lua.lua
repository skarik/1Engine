-- RACE TABLE --
-- DO NOT EDIT THESE VALUES (THESE ARE DEFINED IN CharacterStats.h) --
RACES = {};
RACES.default	= 0;
RACES.human		= 1;
RACES.elf		= 2;
RACES.dwarf		= 3;
RACES.kitten	= 4;
RACES.fluxxor	= 5;
RACES.merchant	= 6;

-- Main table --
RACES.races = {};

-- THE VALUES IN THIS FILE ARE HARD-CODED INTO THE GAME
-- MUCKING AROUND WITH THE VALUES IS FINE FOR TEH LuLz
--  BUT DUCKING WITH VARIABLE NAMES IS A NO-NO

--------------------------------------------
-- Table structure for race properties --
--------------------------------------------
RACES.races[0] = {};
RACES.races[0].iRace				= 0;
-- Movement abilities
RACES.races[0].bCanPowerslide 		= true;
RACES.races[0].bCanSprint			= true;
RACES.races[0].bCanCrouchSprint		= false;
-- Misc states
RACES.races[0].iWieldCount			= 2;
-- Movespeeds + Acceleration
RACES.races[0].fRunSpeed			= 11;		-- Upright speed
RACES.races[0].fSprintSpeed			= 17;		-- Sprinting speed
RACES.races[0].fCrouchSpeed			= 6;		-- Crouching speed
RACES.races[0].fProneSpeed			= 2;	
RACES.races[0].fSwimSpeed			= 12;	-- Water base move speed
RACES.races[0].fGroundAccelBase		= 55.0;
RACES.races[0].fAirAccelBase		= 10.0;
RACES.races[0].fWaterAccelBase		= 20.0;
-- Body size and height
RACES.races[0].fStandingHeight		= 5.7;
RACES.races[0].fCrouchingHeight		= 3.7;
RACES.races[0].fProneHeight			= 1.6;
RACES.races[0].fCollisionRadius		= 0.7;
-- Point stats
RACES.races[0].startStrength		= 13;
RACES.races[0].startAgility			= 13;
RACES.races[0].startIntelligence	= 13;
RACES.races[0].startCharisma		= 13;

--------------------------------------------
-- Human race table
--------------------------------------------
RACES.races[1] = table.shallowcopy(RACES.races[0]);
RACES.races[1].iRace				= RACES.human;

RACES.races[1].startStrength		= 15;
RACES.races[1].startAgility			= 15;
RACES.races[1].startIntelligence	= 15;

--------------------------------------------
-- Drow race table
--------------------------------------------
RACES.races[2] = table.shallowcopy(RACES.races[0]);
RACES.races[2].iRace				= RACES.elf;

RACES.races[2].startStrength		= 5;
RACES.races[2].startAgility			= 21;
RACES.races[2].startIntelligence	= 19;
RACES.races[2].startCharisma		= 1;	-- pretty much no charisma lololol

--------------------------------------------
-- Dwarf race table
--------------------------------------------
RACES.races[3] = table.shallowcopy(RACES.races[0]);
RACES.races[3].iRace				= RACES.dwarf;

RACES.races[3].startStrength		= 30;
RACES.races[3].startAgility			= 10;
RACES.races[3].startIntelligence	= 5;

RACES.races[3].fStandingHeight		= 3.7;	-- Dwarves are short
RACES.races[3].fCrouchingHeight		= 2.7;
RACES.races[3].fProneHeight			= 1.5;
RACES.races[3].fCollisionRadius		= 0.7;

--------------------------------------------
-- Catpeople race table
--------------------------------------------
RACES.races[4] = table.shallowcopy(RACES.races[0]);
RACES.races[4].iRace				= RACES.kitten;

RACES.races[4].startStrength		= 5;
RACES.races[4].startAgility			= 35;
RACES.races[4].startIntelligence	= 5;

RACES.races[4].bCanCrouchSprint		= true;

RACES.races[4].fRunSpeed			= 12;	-- Upright speed
RACES.races[4].fSprintSpeed			= 18;	-- Sprinting speed
RACES.races[4].fCrouchSpeed			= 11;	-- Crouching speed
RACES.races[4].fProneSpeed			= 2;	
--RACES.races[4].fSwimSpeed			= 12.0;	-- no change in swimspeed
RACES.races[4].fGroundAccelBase		= 65.0;
RACES.races[4].fAirAccelBase		= 45.0;	-- both high air and movement control
RACES.races[4].fWaterAccelBase		= 14.0;	-- Lower water acceleration

RACES.races[4].fCrouchingHeight		= 2.9; -- ass in the air

--------------------------------------------
-- Fluxxor race table
--------------------------------------------
RACES.races[5] = table.shallowcopy(RACES.races[0]);
RACES.races[5].iRace				= RACES.fluxxor;

RACES.races[5].fRunSpeed			= 9;		-- Upright speed
RACES.races[5].fSprintSpeed			= 16.5;		-- Sprinting speed
RACES.races[5].fCrouchSpeed			= 4.5;		-- Crouching speed

RACES.races[5].fStandingHeight		= 5.2;	-- Fluxxors are kind of short
RACES.races[5].fCrouchingHeight		= 3.1;
RACES.races[5].fProneHeight			= 1.7;

RACES.races[5].startStrength		= 5;
RACES.races[5].startAgility			= 10;
RACES.races[5].startIntelligence	= 30;

--------------------------------------------
-- Merchant race table
--------------------------------------------
RACES.races[6] = table.shallowcopy(RACES.races[0]);
RACES.races[6].iRace				= RACES.merchant;

RACES.races[6].iWieldCount			= 4;	-- Increased max equip count
-- Movespeeds + Acceleration
RACES.races[6].fSwimSpeed			= 15.0;	-- Increased swimming speed
RACES.races[6].fGroundAccelBase		= 45.0; -- Merchants have slightly looser ground control
RACES.races[6].fAirAccelBase		= 25.0; -- Better air control
-- Body size and height
RACES.races[6].fStandingHeight		= 5.8; -- Merchants are slightly taller
RACES.races[6].fCrouchingHeight		= 3.9;
RACES.races[6].fProneHeight			= 1.8; -- should they be able to go prone? (yeah probably)
RACES.races[6].fCollisionRadius		= 0.8; -- Also wider

RACES.races[6].startStrength		= 10;		-- Possibly balance merchants via lower start stats
RACES.races[6].startAgility			= 13;	-- not final
RACES.races[6].startIntelligence	= 15; -- because these guys are pretty friggin rigged 
RACES.races[6].startCharisma		= 20; -- above average charisma
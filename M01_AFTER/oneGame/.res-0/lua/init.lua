-- init.lua
-- Called when engine starts up the Lua system.

io.write("Lua Machine ",_VERSION," was successfully created!\n")
-- Set environment path
package.path = package.path .. ";.res/lua/?" .. ";.res/lua/?/init.lua" .. ";.res/lua/?/lua.lua";

-- Utility functions and 'classes'
dofile( ".res/lua/sys_common.lua" );
-- Race info
--dofile( ".res/lua/races.lua" );

dofile( ".res/lua/game_area_common.lua" );
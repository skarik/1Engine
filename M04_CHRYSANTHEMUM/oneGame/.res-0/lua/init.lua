-- init.lua
-- Called when engine starts up the Lua system.

io.write("Lua Machine ",_VERSION," was successfully created!\n")
-- Set environment path
package.path = package.path .. ";.res-0/lua/?" .. ";.res-0/lua/?.lua" .. ";.res-0/lua/?/init.lua" .. ";.res-0/lua/?/lua.lua";
--package.path = package.path .. ";addons/?/lua/?" .. ";addons/?/lua/?/init.lua" .. ";addons/?/lua/?/lua.lua";
--package.path = package.path .. ";./lua/?" .. ";./lua/?/init.lua" .. ";./lua/?/lua.lua";

-- Utility functions and 'classes'
dofile( path.."sys_common.lua" );
-- Race info
--dofile( "../lua/races.lua" );

dofile( path.."game_area_common.lua" );
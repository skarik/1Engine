
GameBehavior = {}
GameBehavior.__index = GameBehavior;


function GameBehavior.new ( ptr )
	local gm_bev = {}
	setmetatable( gm_bev, GameBehavior )
	
	gm_bev.m_gm_ptr = ptr;
	
	return gm_bev;
end

function GameBehavior:delete ( )
	return _gs_DeleteBehavior( self.m_gm_ptr );
end

io.write( "--Finished gamebehavior.lua\n" );
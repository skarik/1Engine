
--GameObject = {}
--GameObject.__index = GameObject;
GameObject = inheritsFrom( GameBehavior );

function GameObject.new ( ptr )
	--local gm_obj = {}
	--setmetatable( gm_obj, GameObject )
	local gm_obj = GameObject:create();
	
	gm_obj.m_gm_ptr = ptr;
	
	return gm_obj;
end

function GameObject:GetPosition ()
	local x,y,z
	x,y,z = _go_GetPosition( self.m_gm_ptr )
	return Vector3d.new( x,y,z )
end
function GameObject:SetPosition ( position )
	_go_SetPosition( self.m_gm_ptr, position.x, position.y, position.z )
end

function GameBehavior:ToGameObject( obj )
	local gm_obj = GameObject.new( self.m_gm_ptr )
end
function GameObject:ToGameBehavior( obj )
	local gm_bev = GameBehavior.new( self.m_gm_ptr )
end

io.write( "--Finished gameobject.lua\n" );
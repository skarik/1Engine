
Vector3d = {}
Vector3d.__index = Vector3d;

function Vector3d.new(x, y, z)
	local vect = {}
	setmetatable( vect, Vector3d )
	if ( x == nil or y == nil or z == nil ) then
		x = 0; y = 0; z = 0;
		print( "nil values on vector!" );
	end
	vect.x = x
	vect.y = y
	vect.z = z
	return vect
end
function Vector3d.fromTable ( tbl )
	local vect = {}
	setmetatable( vect, Vector3d )
	vect.x = tbl.x
	vect.y = tbl.y
	vect.z = tbl.z
	return vect
end

function Vector3d.add ( v1, v2 )
	local vect = Vector3d.new( v1.x+v2.x, v1.y+v2.y, v1.z+v2.z );
	return vect
end
Vector3d.__add = Vector3d.add
function Vector3d.sub ( v1, v2 )
	local vect = Vector3d.new( v1.x-v2.x, v1.y-v2.y, v1.z-v2.z );
	return vect
end
Vector3d.__sub = Vector3d.sub
function Vector3d.mul ( v1, fl )
	local vect = Vector3d.new( v1.x*fl, v1.y*fl, v1.z*fl );
	return vect
end
Vector3d.__mul = Vector3d.mul
function Vector3d.div ( v1, fl )
	local vect = Vector3d.new( v1.x/fl, v1.y/fl, v1.z/fl );
	return vect
end
Vector3d.__div = Vector3d.div

function Vector3d.dot ( v1, v2 )
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z)
end
function Vector3d.cross ( v1, v2 )
	local vect = Vector3d.new( v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x );
	return vect
end


function Vector3d:length ( )
	return math.sqrt( self.x*self.x + self.y*self.y + self.z*self.z );
end
function Vector3d:normal ( )
	local len = self:length();
	local vect = Vector3d.new( self.x/len,self.y/len,self.z/len );
	return vect
end

function Vector3d:tostring ( ) --self implicit with ":"
	return ( "(" .. math.floor(self.x) .. "," .. math.floor(self.y) .. "," .. math.floor(self.z) .. ")" );
end
Vector3d.__tostring = Vector3d.tostring


function Vector3d:test ( ) --self implicit with ":"
	print( "test" .. self.x .. self.y .. self.z );
end

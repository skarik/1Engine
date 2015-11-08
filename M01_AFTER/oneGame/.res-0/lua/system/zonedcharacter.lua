
ZonedCharacter = inheritsFrom( CharacterBase );

-- Define constants
ZonedCharacter.Focus = {};
ZonedCharacter.Focus.Wanderer	= 0;
ZonedCharacter.Focus.Companion	= 1;
ZonedCharacter.Focus.TownPerson	= 2;
ZonedCharacter.Focus.Farmer		= 3;
ZonedCharacter.Focus.LuaGeneral = 4;

-- Define routines
function ZonedCharacter.new ( ptr, ai )
	local gm_obj = ZonedCharacter:create()
	
	gm_obj.m_gm_ptr = ptr;
	gm_obj.m_ai = ai;
	
	return gm_obj;
end


--function ZonedCharacter:SetFocus ( 


io.write( "--Finished zonedcharacter.lua\n" );
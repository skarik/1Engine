
CharacterBase = inheritsFrom( GameObject );

-- Define constants


-- Define routines
function CharacterBase.new ( ptr )
	local gm_obj = CharacterBase:create()
	
	gm_obj.m_gm_ptr = ptr;
	gm_obj.m_ai = ai;
	
	return gm_obj;
end

function CharacterBase:GetHealth ()
	local health;
	health = _char_stats_get_health( self.m_gm_ptr )
	return health
end
function CharacterBase:GetHealthMax ()
	local health;
	health = _char_stats_get_health_max( self.m_gm_ptr )
	return health
end

function CharacterBase:GetMana ()
	local mana;
	mana = _char_stats_get_mana( self.m_gm_ptr )
	return mana
end
function CharacterBase:GetManaMax ()
	local mana;
	mana = _char_stats_get_mana_max( self.m_gm_ptr )
	return mana
end

function CharacterBase:GetStamina ()
	local stamina;
	stamina = _char_stats_get_stamina( self.m_gm_ptr )
	return stamina
end
function CharacterBase:GetStaminaMax ()
	local stamina;
	stamina = _char_stats_get_stamina_max( self.m_gm_ptr )
	return stamina
end

io.write( "--Finished character.lua\n" );
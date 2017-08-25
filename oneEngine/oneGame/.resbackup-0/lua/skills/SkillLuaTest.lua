
local anim_type = 9;
local anim_subset = 2;
local cooldown = 3.0;
local level = 0; -- Set by the system

function Update ( deltaTime )
	
	cooldown = math.max( 1.0, 3.0 - (level-1)*0.5 );
	
end

function ItemData ( )
	data = {};
	
	data.info = "Test Spell";
	
	return data;
end

function GetThisLevelCost ( level )
	health = 5.0;
	stamina = 4.0;
	mana = 3.0;
	
	return health, stamina, mana;
end

function SkillInfo ( )
	info = {};
	
	info.name = "Test Spell";
	info.description = "A lua spell used to pattern other lua spells off of it";
	info.icon = ".res/textures/icons/skill_firetoss.png";
	info.passive = false;
	
	return info;
end
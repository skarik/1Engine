


function ItemData ( )
	
	data = {};
	
	data.info = "";
	data.canStack = false;
	data.maxStack = 1;
	data.isSkill = false;
	data.isTradable = false;
	
	data.canDegrade = false;
	data.maxDurability = 100;
	
	data.itemType = 0; --TypeDefault
	data.topType = 14; --ItemMisc
	data.itemRarity = 0; --RarityNone
	
	return data;
	
end
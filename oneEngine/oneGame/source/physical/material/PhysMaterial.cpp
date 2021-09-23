#include "physMaterial.h"
#include "core/debug.h"
#include <array>

namespace material
{
	static std::array<physical::PrPhysMaterial, (int)kPhysMat_MAX> g_PhysMaterials = {

		physical::PrPhysMaterial(kPhysMatDefault, 0.2F, 1.0F,
			nullptr, nullptr,
			nullptr, nullptr,
			"particlesystems/dusttest01", "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatDirt, 0.2F, 1.0F,
			"particlesystems/collide/dirt_hit", nullptr,
			"particlesystems/collide/dirt_collide", nullptr,
			"particlesystems/collide/dirt_hit", "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatGrass, 0.2F, 1.0F,
			"particlesystems/collide/dirt_hit", nullptr,
			"particlesystems/collide/dirt_collide", nullptr,
			nullptr, "Char.FootstepGrass"),

		physical::PrPhysMaterial(kPhysMatRock, 0.2F, 1.0F,
			"particlesystems/collide/stone_hit", nullptr,
			nullptr, nullptr,
			nullptr, "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatMud, 0.2F, 1.5F,
			nullptr, nullptr,
			nullptr, nullptr,
			nullptr, "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatGravel, 0.2F, 1.0F,
			"particlesystems/collide/stone_hit", nullptr,
			nullptr, nullptr,
			nullptr, "Char.FootstepSand"),

		physical::PrPhysMaterial(kPhysMatSand, 0.2F, 0.8F,
			"particlesystems/collide/sand_collide", nullptr,
			"particlesystems/collide/sand_collide", nullptr,
			"particlesystems/collide/sand_collide", "Char.FootstepSand"),

		physical::PrPhysMaterial(kPhysMatCrystal, 0.2F, 1.0F,
			nullptr, nullptr,
			nullptr, nullptr,
			nullptr, "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatWood, 0.2F, 1.0F,
			nullptr, nullptr,
			nullptr, nullptr,
			nullptr, "Char.FootstepWood"),

		physical::PrPhysMaterial(kPhysMatWater, 0.2F, 1.0F,
			nullptr, nullptr,
			nullptr, nullptr,
			nullptr, "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatIce, 0.2F, 0.2F,
			nullptr, nullptr,
			nullptr, nullptr,
			nullptr, "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatSnow, 0.2F, 0.86F,
			"particlesystems/collide/dust_collide", nullptr,
			"particlesystems/collide/dust_collide", nullptr,
			"particlesystems/collide/dust_step2", "Char.FootstepSand"),

		physical::PrPhysMaterial(kPhysMatBlood, 0.2F, 1.0F,
			"particlesystems/blood", nullptr,
			nullptr, nullptr,
			nullptr, "Char.Footstep"),

		physical::PrPhysMaterial(kPhysMatMetal, 0.2F, 0.95F,
			nullptr, nullptr,
			nullptr, nullptr,
			nullptr, "Char.Footstep"),
	};
};

const char* PrGetPhysMaterialName ( const PrPhysMaterialType type )
{
	switch (type)
	{
		case kPhysMatDirt:		return "Dirt";
		case kPhysMatGrass:		return "Grass";
		case kPhysMatRock:		return "Rock";
		case kPhysMatMud:		return "Mud";
		case kPhysMatGravel:	return "Gravel";
		case kPhysMatSand:		return "Sand";
		case kPhysMatCrystal:	return "Crystal";
		case kPhysMatWood:		return "Wood";
		case kPhysMatWater:		return "Water";
		case kPhysMatIce:		return "Ice";
		case kPhysMatSnow:		return "Snow";
		case kPhysMatBlood:		return "Blood";
		case kPhysMatMetal:		return "Metal";
		default:	return "Default/None";
	}
}

const PrPhysMaterialType PrGetPhysMaterialTypeFromName ( const char* name )
{
		 if (strncmp(name, "Dirt", 4))		return kPhysMatDirt;
	else if (strncmp(name, "Grass", 5))		return kPhysMatGrass;
	else if (strncmp(name, "Rock", 4))		return kPhysMatRock;
	else if (strncmp(name, "Mud", 3))		return kPhysMatMud;
	else if (strncmp(name, "Gravel", 6))	return kPhysMatGravel;
	else if (strncmp(name, "Sand", 4))		return kPhysMatSand;
	else if (strncmp(name, "Crystal", 7))	return kPhysMatCrystal;
	else if (strncmp(name, "Wood", 4))		return kPhysMatWood;
	else if (strncmp(name, "Water", 5))		return kPhysMatWater;
	else if (strncmp(name, "Ice", 3))		return kPhysMatIce;
	else if (strncmp(name, "Snow", 4))		return kPhysMatSnow;
	else if (strncmp(name, "Blood", 5))		return kPhysMatBlood;
	else if (strncmp(name, "Metal", 5))		return kPhysMatMetal;
	return kPhysMatDefault;
}

const physical::PrPhysMaterial* PrGetPhysMaterial ( const PrPhysMaterialType type )
{
	ARCORE_ASSERT(material::g_PhysMaterials[(int)type].type == type);
	return &material::g_PhysMaterials[(int)type];
}
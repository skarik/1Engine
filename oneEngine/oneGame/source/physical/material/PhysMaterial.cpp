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

const physical::PrPhysMaterial* PrGetPhysMaterial ( const PrPhysMaterialType type )
{
	ARCORE_ASSERT(material::g_PhysMaterials[(int)type].type == type);
	return &material::g_PhysMaterials[(int)type];
}
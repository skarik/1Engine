
#include <cstring>
#include "physMaterial.h"

// Constructor for default values
physMaterial::physMaterial ( void )
{
	//strcpy( name, "" );
	index = 0;

	//strcpy( pt_hit, "" );
	//strcpy( snd_hit, "" );

	//strcpy( pt_collide, "" );
	//strcpy( snd_collide, "" );

	phys_restitution	= 0.2f;
	phys_friction		= 1.0f;
}

// Destructor not really needed
physMaterial::~physMaterial ( void )
{
	;
}

// Comparison operator
bool physMaterial::operator== ( physMaterial const& right ) const
{
	return (index==right.index);
}

// Physical material space
namespace PhysMats
{
	// Definition of default materials
	physMaterial Default;
	physMaterial Dirt;
	physMaterial Grass;
	physMaterial Rock;
	physMaterial Mud;
	physMaterial Gravel;
	physMaterial Sand;
	physMaterial Crystal;
	physMaterial Wood;
	physMaterial Water;
	physMaterial Ice;
	physMaterial Snow;
	physMaterial Blood;
	physMaterial Metal;

	int _SetDefaultMaterials ( void )
	{
		Default.name = "Default";
		Default.index = 1;
		Default.snd_step = "Char.Footstep";
		Default.pt_step = ".res/particlesystems/dusttest01.pcf";

		Dirt.name = "Dirt";
		Dirt.index = 2;
		Dirt.pt_collide = ".res/particlesystems/collide/dirt_collide.pcf";
		Dirt.pt_hit = ".res/particlesystems/collide/dirt_hit.pcf";
		Dirt.pt_step = ".res/particlesystems/collide/dirt_hit.pcf";
		Dirt.snd_step = "Char.Footstep";

		Grass.name = "Grass";
		Grass.index = 3;
		Grass.pt_hit = Dirt.pt_hit;
		Grass.pt_collide = Dirt.pt_collide;
		Grass.snd_step = "Char.FootstepGrass";

		Rock.name = "Rock";
		Rock.index = 4;
		Rock.pt_hit = ".res/particlesystems/collide/stone_hit.pcf";
		Rock.snd_step = "Char.Footstep";

		Mud.name = "Mud";
		Mud.index = 5;
		Mud.snd_step = "Char.Footstep";

		Gravel.name = "Gravel";
		Gravel.index = 6;
		Gravel.pt_hit = Rock.pt_hit;
		Gravel.snd_step = "Char.FootstepSand";

		Sand.name = "Sand";
		Sand.index = 7;
		Sand.pt_hit = ".res/particlesystems/collide/sand_collide.pcf";
		Sand.pt_collide = ".res/particlesystems/collide/sand_collide.pcf";
		Sand.pt_step = ".res/particlesystems/collide/sand_collide.pcf";
		Sand.snd_step = "Char.FootstepSand";
		Sand.phys_friction = 0.8f;
		
		Crystal.name = "Crystal";
		Crystal.index = 8;
		Crystal.snd_step = "Char.Footstep";

		Wood.name = "Wood";
		Wood.index = 9;
		Wood.pt_hit = Dirt.pt_hit;
		Wood.snd_step = "Char.FootstepWood";

		Water.name = "Water";
		Water.index = 10;

		Ice.name = "Ice";
		Ice.index = 11;
		Ice.phys_friction = 0.2f;
		Ice.snd_step = "Char.Footstep";

		Snow.name = "Snow";
		Snow.index = 12;
		Snow.pt_hit = ".res/particlesystems/collide/dust_collide.pcf";
		Snow.pt_collide = ".res/particlesystems/collide/dust_collide.pcf";
		Snow.pt_step = ".res/particlesystems/collide/dust_step2.pcf";
		Snow.snd_step = "Char.FootstepSand";
		Snow.phys_friction = 0.86f;

		Blood.name = "Blood";
		Blood.index = 13;
		Blood.pt_hit = ".res/particlesystems/blood.pcf";

		Metal.name = "Metal";
		Metal.index = 14;
		Metal.pt_hit = ".res/particlesystems/collide/dust_step2.pcf";
		Metal.phys_friction = 0.94f;

		return 0;
	}

	physMaterial* Get ( const MaterialType type )
	{
		switch ( type )
		{
		case MAT_Dirt:		return &Dirt;
		case MAT_Grass:		return &Grass;
		case MAT_Rock:		return &Rock;
		case MAT_Mud:		return &Mud;
		case MAT_Gravel:	return &Gravel;
		case MAT_Sand:		return &Sand;
		case MAT_Crystal:	return &Crystal;
		case MAT_Wood:		return &Wood;
		case MAT_Water:		return &Water;
		case MAT_Ice:		return &Ice;
		case MAT_Snow:		return &Snow;
		case MAT_Blood:		return &Blood;
		case MAT_Metal:		return &Metal;
		default: return &Default;
		}
	}
};

int _phys_a = PhysMats::_SetDefaultMaterials();
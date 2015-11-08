// physMaterial.h
// This is more like a gameplay material but since it's fairly close to the physics engine, it's called physMaterial
// Right, also, rigidbodies love this guy: can't get enough of that yoloswag
// If glMaterials define how something looks, physMaterials define how something sounds and reacts.
// glMaterials are 

#ifndef _PHYS_MATERIAL_H_
#define _PHYS_MATERIAL_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

class physMaterial //do not inherit from this class
{
public:
	explicit		physMaterial ( void );
					~physMaterial ( void );

	bool			operator== ( physMaterial const & right ) const;

	operator uint32_t ( void ) const {
		return index;
	}

public:	
	arstring<64>	name;		// Material name
	uint32_t		index;		// Type index

	arstring<256> pt_hit;		// Particle + sound for when hit
	arstring<256> snd_hit;
	
	arstring<256> pt_collide;	// Particle + sound for default collisions
	arstring<256> snd_collide;

	arstring<256> pt_step;		// Particle + sound for steps
	arstring<256> snd_step;

	float		phys_restitution;	// Range from 0 to 1 of bounciness multiplier
	float		phys_friction;		// Range from 0 to 1 of friction multiplier

};

namespace PhysMats
{
	/*extern physMaterial Default;
	extern physMaterial Dirt;
	extern physMaterial Grass;
	extern physMaterial Rock;
	extern physMaterial Mud;
	extern physMaterial Gravel;
	extern physMaterial Sand;
	extern physMaterial Crystal;
	extern physMaterial Wood;
	extern physMaterial Water;
	extern physMaterial Ice;
	extern physMaterial Snow;
	extern physMaterial Blood;
	extern physMaterial Metal;*/
	enum MaterialType
	{
		MAT_Default,
		MAT_Dirt,
		MAT_Grass,
		MAT_Rock,
		MAT_Mud,
		MAT_Gravel,
		MAT_Sand,
		MAT_Crystal,
		MAT_Wood,
		MAT_Water,
		MAT_Ice,
		MAT_Snow,
		MAT_Blood,
		MAT_Metal
	};
	ENGINE_API physMaterial* Get ( const MaterialType );
};


#endif//_PHYS_MATERIAL_H_
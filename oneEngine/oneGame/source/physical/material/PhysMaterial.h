//===============================================================================================//
// physical/material/PhysMaterial.h
// 
//	Contains definition for PrPhysMaterial, which holds information for behavior of surfaces when
//	interacted with.
//
//===============================================================================================//
#ifndef PHYSICAL_PHYS_MATERIAL_H_
#define PHYSICAL_PHYS_MATERIAL_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

enum PrPhysMaterialType : int8_t
{
	kPhysMatDefault = 0,
	kPhysMatDirt,
	kPhysMatGrass,
	kPhysMatRock,
	kPhysMatMud,
	kPhysMatGravel,
	kPhysMatSand,
	kPhysMatCrystal,
	kPhysMatWood,
	kPhysMatWater,
	kPhysMatIce,
	kPhysMatSnow,
	kPhysMatBlood,
	kPhysMatMetal,

	kPhysMat_MAX
};

PHYS_API const char*	PrGetPhysMaterialName ( const PrPhysMaterialType type );
PHYS_API const PrPhysMaterialType
						PrGetPhysMaterialTypeFromName ( const char* name );

namespace physical
{
	class PrPhysMaterial final
	{
	public:
		PHYS_API explicit		PrPhysMaterial (
			const PrPhysMaterialType type, 
			const float phys_restitution,
			const float phys_friction,
			const char* pt_hit, 
			const char* snd_hit,
			const char* pt_collide,
			const char* snd_collide,
			const char* pt_step,
			const char* snd_step )
			: type(type)
			, phys_restitution(phys_restitution), phys_friction(phys_friction)
			, pt_hit(pt_hit), snd_hit(snd_hit)
			, pt_collide(pt_collide), snd_collide(snd_collide)
			, pt_step(pt_step), snd_step(snd_step)
		{}
		PHYS_API				~PrPhysMaterial ( void ) {}

		//	operator== : Is this physmat the same as the other one?
		PHYS_API bool			operator== ( PrPhysMaterial const & right ) const
			{ return type == right.type; }

		PHYS_API const char*	GetName ( void ) const
			{ return PrGetPhysMaterialName(type); }

	public:	
		PrPhysMaterialType	type = kPhysMatDefault;

		// Range from 0 to 1 of bounciness multiplier
		float				phys_restitution = 0.2F;
		// Range from 0 to 1 of friction multiplier
		float				phys_friction = 1.0F;

		// Particle + sound for when hit
		arstring<256>		pt_hit;
		arstring<256>		snd_hit;
	
		// Particle + sound for default collisions
		arstring<256>		pt_collide;
		arstring<256>		snd_collide;

		// Particle + sound for steps
		arstring<256>		pt_step;
		arstring<256>		snd_step;
	};
}

PHYS_API const physical::PrPhysMaterial*
						PrGetPhysMaterial ( const PrPhysMaterialType type );


#endif//_PHYS_MATERIAL_H_
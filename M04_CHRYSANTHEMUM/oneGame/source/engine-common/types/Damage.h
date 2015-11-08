//Copyright Implementation 2012 by John Choi

// this is a totally trivial thing to copyright mannng

#ifndef _DAMAGE_H_
#define _DAMAGE_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Vector3d.h"

class CActor;
class physMaterial;

namespace DamageType
{
	enum DamageType : uint64_t
	{
		Void	= 1,
		Physical= 2,	//Physical
		Magical	= 4,	//Magical
		Ice		= 8,
		Fire	= 16,
		//Water	= 32,	// just do drown
		Spirit	= 32,
		Electric= 64,	//Electric
		Poison	= 128,	//Poison
		Ground	= 256,
		Crush	= 512,
		Bullet	= 1024,
		Slash	= 2048,
		Pierce	= 4096,
		Burn	= 8192,
		//Freeze	= 16384,
		Freeze	= 8|32,
		Reflect = 16384,
		Blast	= 32768,
		Club	= 65536,
		Energy	= 131072,
		Drown	= 262144,
		Fall	= 524288,
		Rad		= 1048576,	//Radiation
		Chemical= 2097152,	//Chemical
		Ethereal= 4194304
	};
}

struct Damage
{
	ftype		amount;
	ftype		stagger_chance;
	uint64_t	type;

	Vector3d	source;
	Vector3d	direction;
	CActor*		actor;

	Vector3d	applyDirection;


	// Damage default values
	Damage ( void )
		: amount(0), type(0), stagger_chance(0.05f),
		source(Vector3d()), direction(Vector3d()), applyDirection(Vector3d(0,0,1)),
		actor(NULL)
	{}
};

struct DamageFeedback
{
	physMaterial*	material;
	bool			do_effect;

	DamageFeedback ( void )
		: material(NULL), do_effect(true)
	{
	}
};

#endif
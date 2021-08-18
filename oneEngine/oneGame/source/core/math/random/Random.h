#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Vector2.h"
#include "core/math/Vector3.h"

#include <random>

#ifdef min
	#undef min
#endif
#ifdef max
	#undef max
#endif 

#ifdef random_range
	#undef random_range
	#define random_range(a,b) Random::Range((a),(b))
#endif
#define random_range(a,b) CRandom::Range((a),(b))

class CRandom
{
private:
	//typedef boost::random::mt19937 base_generator_type;
	typedef std::mt19937 base_generator_type;
	base_generator_type m_generator;

public:
	static Real Range ( Real min, Real max );

	static Vector3f PointOnUnitSphere ( void );
	static Vector3f PointInUnitSphere ( void );

	static Vector2f PointOnUnitCircle ( void );
	static Vector2f PointInUnitCircle ( void );

	void Seed ( uint32_t seed )
	{
		m_generator.seed( seed );
	};
	uint32_t Next ( void )
	{
		return m_generator();
	};
	uint32_t Next ( int min, int max )
	{
		//boost::random::uniform_int_distribution<> distribution( min, max );
		std::uniform_int_distribution<> distribution( min,max );
		return distribution(m_generator);
	};
	uint32_t Max ( void )
	{
		return base_generator_type::max();
	};

	bool Chance ( Real test_value );
};

extern CRandom Random;

#endif
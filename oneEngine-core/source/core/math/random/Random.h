
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "core/types/types.h"
#include "core/types/float.h"

/*#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>*/
#include <random>

class Vector2d;
class Vector3d;

#ifdef min
	#undef min
#endif
#ifdef max
	#undef max
#endif 

#ifdef random_range
	#undef random_range
	#define random_range(a,b) ( ((a) + ((Random.Next()/((ftype)Random.Max()))*((b)-(a)))) )
#endif

class CRandom
{
private:
	//typedef boost::random::mt19937 base_generator_type;
	typedef std::mt19937 base_generator_type;
	base_generator_type m_generator;

public:
	static ftype Range ( ftype min, ftype max );

	static Vector3d PointOnUnitSphere ( void );
	static Vector3d PointInUnitSphere ( void );

	static Vector2d PointOnUnitCircle ( void );

	void Seed ( uint32_t seed ) {
		m_generator.seed( seed );
	};
	uint32_t Next ( void ) {
		return m_generator();
	};
	uint32_t Next ( int min, int max ) {
		//boost::random::uniform_int_distribution<> distribution( min, max );
		std::uniform_int_distribution<> distribution( min,max );
		return distribution(m_generator);
	};
	uint32_t Max ( void ) {
		return base_generator_type::max();
	};

	bool Chance ( ftype test_value );
};

extern CRandom Random;

#endif
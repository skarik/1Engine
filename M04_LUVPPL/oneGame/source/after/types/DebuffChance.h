// Class for weighted chances.

#ifndef _DEBUFF_CHANCE_H_
#define _DEBUFF_CHANCE_H_

#include "core/types/float.h"
#include "core/math/random/Random.h"
#include <algorithm>

namespace Debuffs
{

class Chance
{
public:
	explicit Chance ( const ftype nBaseChance )
	{
		base_chance = nBaseChance;
		current_chance = base_chance;
	}

	bool Roll ( void )
	{
		ftype roll = Random.Range( 0,1.0f );
		ftype delta = 0.0f;
		if ( roll < current_chance ) {
			// Roll is good, make current_chance smaller
			// going towards base chance is faster
			// going away from base chance is slower
			delta = 1-(base_chance*2-current_chance);
			if ( current_chance > base_chance ) {
				delta *= 2;
			}
			current_chance -= delta*0.1f;
			current_chance = std::max<ftype>( 0, std::min<ftype>( 1, current_chance ) );
			return true;
		}
		else {
			// Roll is good, make current_chance smaller
			// going towards base chance is faster
			// going away from base chance is slower
			delta = (base_chance*2-current_chance);
			if ( current_chance < base_chance ) {
				delta *= 2;
			}
			current_chance += delta*0.1f;
			current_chance = std::max<ftype>( 0, std::min<ftype>( 1, current_chance ) );
			return false;
		}
	}

	// Assignment operator
	ftype& operator= ( ftype const &rhs )
	{
		base_chance		= rhs;
		doLimiter();
		return base_chance;
	}
	// Assignment operator
	Chance& operator= ( Chance const &rhs )
	{
		base_chance		= rhs.base_chance;
		doLimiter();
		return *this;
	}
	//Addition overload
	Chance operator+ ( ftype const& right ) const
	{
		Chance result ( base_chance );
		result.current_chance = current_chance;

		result.base_chance += right;
		result.current_chance += right;
		result.doLimiter();

		return result;
	}
	//Addition shorthand overload	
	void operator+= ( ftype const& right )
	{
		base_chance += right;
		doLimiter();
	}
	//Subtraction overload
	Chance operator- ( ftype const& right ) const
	{
		Chance result ( base_chance );
		result.current_chance = current_chance;

		result.base_chance -= right;
		result.current_chance -= right;
		result.doLimiter();

		return result;
	}
	//Subtraction shorthand overload
	void operator-= ( ftype const& right )
	{
		base_chance -= right;
		doLimiter();
	}

	// Type casting operator
	operator const ftype() const { return base_chance; }

	// Peek at chance
	ftype GetCurrentChance ( void ) {
		return current_chance;
	}
	void ResetChance ( void ) {
		current_chance = base_chance;
	}

private:
	// Private functions
	void doLimiter ( void )
	{
		base_chance = (((base_chance) < (1.0f)) ? (base_chance) : (1.0f));
		base_chance = (((base_chance) > (0.0f)) ? (base_chance) : (0.0f));
		current_chance = (((current_chance) < (1.0f)) ? (current_chance) : (1.0f));
		current_chance = (((current_chance) > (0.0f)) ? (current_chance) : (0.0f));
	}

private:
	ftype	base_chance;
	ftype	current_chance;
};

};

#endif//_DEBUFF_CHANCE_H_
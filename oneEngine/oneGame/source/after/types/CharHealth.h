// Class for easy representation of Character health.
// Since so many little checks need to be done, it's best to 
// move it to the class.

#ifndef _CHAR_HEALTH_H_
#define _CHAR_HEALTH_H_

#include "core/types/float.h"

class CharHealth
{
public:
	// Constructor
	explicit CharHealth ( ftype f_MaxHealth )
	{
		maxHealth	= f_MaxHealth;
		health		= maxHealth;
		prevHealth	= maxHealth;
	}
	

	// Assignment operator
	ftype& operator= ( ftype const &rhs )
	{
		prevHealth	= health;
		health		= rhs;
		doLimiter();
		return health;
	}
	// Assignment operator
	CharHealth& operator= ( CharHealth const &rhs )
	{
		prevHealth	= health;
		health		= rhs.health;
		doLimiter();
		return *this;
	}
	//Addition overload
	CharHealth operator+ ( ftype const& right ) const
	{
		CharHealth result ( maxHealth );
		result.health = health;
		result.prevHealth = prevHealth;

		result.health += right;
		result.doLimiter();

		return result;
	}
	//Addition shorthand overload	
	void operator+= ( ftype const& right )
	{
		prevHealth = health;
		health += right;
		doLimiter();
	}
	//Subtraction overload
	CharHealth operator- ( ftype const& right ) const
	{
		CharHealth result ( maxHealth );
		result.health = health;
		result.prevHealth = prevHealth;

		result.health -= right;
		result.doLimiter();

		return result;
	}
	//Subtraction shorthand overload
	void operator-= ( ftype const& right )
	{
		prevHealth = health;
		health -= right;
		doLimiter();
	}

	// Type casting operator
	operator const ftype() const { return health; }

	// Set make
	void SetMax ( ftype const& f_NewMax )
	{
		maxHealth	= f_NewMax;
		doLimiter();
	}
	// Get make
	ftype GetMax ( void ) const
	{
		return maxHealth;
	}
private:
	// Private functions
	void doLimiter ( void )
	{
		health = (((health) < (maxHealth)) ? (health) : (maxHealth));
	}
private:
	// Member data
	ftype maxHealth;
	ftype health;
	ftype prevHealth;
};

#endif
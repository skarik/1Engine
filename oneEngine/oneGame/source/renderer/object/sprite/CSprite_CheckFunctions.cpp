
#include "CSprite.h"

// Needs to be changed to work properly with rotations.
bool	CSprite::ContainsPoint ( Vector2d pointToCheck )
{
	if ( pointToCheck.x < position.x )
		return false;
	if ( pointToCheck.y < position.y )
		return false;
	if ( pointToCheck.x > position.x + size.x*scale.x )
		return false;
	if ( pointToCheck.y > position.y + size.y*scale.y )
		return false;
	return true;
}
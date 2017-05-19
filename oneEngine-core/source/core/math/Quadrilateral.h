
#ifndef _QUADRILATERAL_H_
#define _QUADRILATERAL_H_

#include "Vector2d.h"

class Quadrilateral
{
public:
	//Quadrilateral 
	explicit		Quadrilateral ( const Vector2d& a, const Vector2d& b, const Vector2d& c, const Vector2d& d );
	explicit		Quadrilateral ( void );

	Real			Area ( void ) const;

	Vector2d&		operator[] ( const int index );
	const Vector2d& operator[] ( const int index ) const;

	// Cuts this quad into possibly two quads based on the given line equation. Returns the number of quads created by the slice.
	int				Slice ( const Vector2d& ray_pos, const Vector2d& ray_dir, Quadrilateral& out_quad1, Quadrilateral& out_quad2 ) const;

public:
	Vector2d	m_corners [4];
	

};

#endif
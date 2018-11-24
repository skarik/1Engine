
#ifndef _QUADRILATERAL_H_
#define _QUADRILATERAL_H_

#include "Vector2f.h"

class Quadrilateral
{
public:
	//Quadrilateral 
	explicit		Quadrilateral ( const Vector2f& a, const Vector2f& b, const Vector2f& c, const Vector2f& d );
	explicit		Quadrilateral ( void );

	Real			Area ( void ) const;

	Vector2f&		operator[] ( const int index );
	const Vector2f& operator[] ( const int index ) const;

	// Cuts this quad into possibly two quads based on the given line equation. Returns the number of quads created by the slice.
	int				Slice ( const Vector2f& ray_pos, const Vector2f& ray_dir, Quadrilateral& out_quad1, Quadrilateral& out_quad2 ) const;

public:
	Vector2f	m_corners [4];
	

};

#endif
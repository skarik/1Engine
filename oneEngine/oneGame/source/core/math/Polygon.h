
#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "Vector2d.h"

class Polygon
{
public:
	explicit		Polygon ( const int nvertices );
					~Polygon( void );

	// Returns area of polygon if the points define a convex polygon
	Real			GetArea ( void );
	// Returns number of vertices
	int				VertexCount ( void );


	Vector2d&		operator[] ( const int index );
	const Vector2d& operator[] ( const int index ) const;

private:
	Vector2d*	m_vertices;
	int			m_num_verts;
};

#endif
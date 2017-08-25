
#include "Polygon.h"

Polygon::Polygon ( const int nvertices )
	: m_num_verts ( nvertices )
{
	m_vertices = new Vector2d [m_num_verts];

}
Polygon::~Polygon( void )
{
	delete [] m_vertices;
	m_vertices = NULL;
}

Real Polygon::GetArea ( void )
{
	Real totalArea = 0;
	
	for ( int i = 0; i < m_num_verts-1; ++i )
	{
		totalArea += (m_vertices[i].x * m_vertices[i+1].y)-(m_vertices[i].y * m_vertices[i+1].x);
	}
	totalArea += (m_vertices[m_num_verts-1].x * m_vertices[0].y)-(m_vertices[m_num_verts-1].y * m_vertices[0].x);

	return totalArea/2;
}
int Polygon::VertexCount ( void )
{
	return m_num_verts;
}


Vector2d&		Polygon::operator[] ( const int index )
{
	if ( index < 0 ) {
		return m_vertices[0];
	}
	else if ( index < m_num_verts ) {
		return m_vertices[index];
	}
	else {
		return m_vertices[m_num_verts-1];
	}
}
const Vector2d& Polygon::operator[] ( const int index ) const
{
	if ( index < 0 ) {
		return m_vertices[0];
	}
	else if ( index < m_num_verts ) {
		return m_vertices[index];
	}
	else {
		return m_vertices[m_num_verts-1];
	}
}

#include "physStaticCompressedMesh.h"

// == Constructor ==
physStaticCompressedMesh::physStaticCompressedMesh ( void )
{
	bReady	= false;
	m_shape = NULL;
}

// == Destructor ==
physStaticCompressedMesh::~physStaticCompressedMesh ( void )
{
	if ( m_shape != NULL )
	{
		m_shape->removeReference();
		m_shape = NULL;
	}
}

// UpdateCollider (TerrainVertex) : Creates Havok geometry from terrain model
void physStaticCompressedMesh::Initialize ( CTerrainVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count, Real scale )
{
	hkGeometry geometry; 
	geometry.m_triangles.clear();
	geometry.m_vertices.clear();

	geometry.m_vertices.setSize( vertex_count );
	geometry.m_triangles.setSize( triangle_count );

	for ( uint vert = 0; vert < vertex_count; ++vert )
	{
		geometry.m_vertices[vert].setComponent<0>( vertices[vert].x*scale );
		geometry.m_vertices[vert].setComponent<1>( vertices[vert].y*scale );
		geometry.m_vertices[vert].setComponent<2>( vertices[vert].z*scale );
	}
	for ( uint tri = 0; tri < triangle_count; ++tri )
	{
		geometry.m_triangles[tri].m_a = triangles[tri].vert[0];
		geometry.m_triangles[tri].m_b = triangles[tri].vert[1];
		geometry.m_triangles[tri].m_c = triangles[tri].vert[2];
	}
	Initialize( &geometry);
}
// UpdateCollider (ModelVertex) : Creates Havok geometry from normal model
void physStaticCompressedMesh::Initialize ( CModelVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count, Real scale )
{
	hkGeometry geometry; 
	geometry.m_triangles.clear();
	geometry.m_vertices.clear();

	geometry.m_vertices.setSize( vertex_count );
	geometry.m_triangles.setSize( triangle_count );

	for ( uint vert = 0; vert < vertex_count; ++vert )
	{
		geometry.m_vertices[vert].setComponent<0>( vertices[vert].x*scale );
		geometry.m_vertices[vert].setComponent<1>( vertices[vert].y*scale );
		geometry.m_vertices[vert].setComponent<2>( vertices[vert].z*scale );
	}
	for ( uint tri = 0; tri < triangle_count; ++tri )
	{
		geometry.m_triangles[tri].m_a = triangles[tri].vert[0];
		geometry.m_triangles[tri].m_b = triangles[tri].vert[1];
		geometry.m_triangles[tri].m_c = triangles[tri].vert[2];
	}
	Initialize( &geometry );
}

// Init
void physStaticCompressedMesh::Initialize ( const hkGeometry* geometry )
{
	// Free the old data, duh
	hkpDefaultBvCompressedMeshShapeCinfo cInfo( geometry );
	m_shape = new hkpBvCompressedMeshShape( cInfo );
}
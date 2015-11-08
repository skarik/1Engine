

#include "CBoob.h"
#include "CLODBoobMesh.h"
#include "CRenderState.h"

Terrain::terra_t_lod CLODBoobMesh::nullBlock;

CLODBoobMesh::CLODBoobMesh ( void )
	: CBoobMesh()
{
	m_dataSet = NULL;
}

CLODBoobMesh::~CLODBoobMesh ( void )
{
	m_dataSet = NULL;
}


Terrain::terra_t_lod CLODBoobMesh::GetBlockValue ( const char indexx, const char indexy, const char indexz, const EFaceDir dir )
{
	switch ( dir )
	{
	case FRONT:
		if ( indexx == 31 ) {
			return nullBlock;
		}
		return m_dataSet[(indexx+1) + indexy*32 + indexz*1024];

	case BACK:
		if ( indexx == 0 ) {
			return nullBlock;
		}
		return m_dataSet[(indexx-1) + indexy*32 + indexz*1024];

	case LEFT:
		if ( indexy == 31 ) {
			return nullBlock;
		}
		return m_dataSet[indexx + (indexy+1)*32 + indexz*1024];

	case RIGHT:
		if ( indexy == 0 ) {
			return nullBlock;
		}
		return m_dataSet[indexx + (indexy-1)*32 + indexz*1024];
		
	case TOP:
		if ( indexz == 31 ) {
			return nullBlock;
		}
		return m_dataSet[indexx + indexy*32 + (indexz+1)*1024];
		
	case BOTTOM:
		if ( indexz == 0 ) {
			return nullBlock;
		}
		return m_dataSet[indexx + indexy*32 + (indexz-1)*1024];
		
	}
	return nullBlock;
}


void CLODBoobMesh::AddBlockLOD ( const char indexx, const char indexy, const char indexz, const Vector3d& pos, const ftype size )
{
	char i;

	// Create the face block type list
	terra_t_lod terraFaceGrid [7];
	terraFaceGrid[0] = m_dataSet[indexx + indexy*32 + indexz*1024];
	for ( i = 1; i < 7; ++i )
		terraFaceGrid[i] = GetBlockValue( indexx, indexy, indexz, (EFaceDir)(i) );

	unsigned short faceGrid [7];
	for ( i = 0; i < 7; ++i )
		faceGrid[i] = terraFaceGrid[i].loaded && terraFaceGrid[i].solid;

	// Create the cube point list
	Vector3d gridvList[8];
	for ( i = 0; i < 4; ++i )
		gridvList[i] = pos;
	gridvList[1].x += size;
	gridvList[2].x += size;
	gridvList[2].y += size;
	gridvList[3].y += size;
	for ( i = 4; i < 8; ++i )
	{
		gridvList[i] = gridvList[i-4];
		gridvList[i].z += size;
	}

	// Create the mesh
	if ( faceGrid[TOP] ) // Set BOTTOM of the block on TOP
	{
		vertices[vertexCount+0].x = gridvList[4].x;
		vertices[vertexCount+0].y = gridvList[4].y;
		vertices[vertexCount+0].z = gridvList[4].z;

		vertices[vertexCount+1].x = gridvList[5].x;
		vertices[vertexCount+1].y = gridvList[5].y;
		vertices[vertexCount+1].z = gridvList[5].z;

		vertices[vertexCount+2].x = gridvList[6].x;
		vertices[vertexCount+2].y = gridvList[6].y;
		vertices[vertexCount+2].z = gridvList[6].z;

		vertices[vertexCount+3].x = gridvList[7].x;
		vertices[vertexCount+3].y = gridvList[7].y;
		vertices[vertexCount+3].z = gridvList[7].z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = -1;
		}

		// Set UV's
		//SetInFaceUVs( vertices, BOTTOM, faceGrid[TOP] );
		//SetFaceColors( vertices, TOP, faceGrid[TOP], pBoob->data[index16].data[index8].data[indexd].light );
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].u = 0;
			vertices[vertexCount+i].v = 0;
			vertices[vertexCount+i].w = 0;
			vertices[vertexCount+i].r = terraFaceGrid[TOP].r / 256.0f;
			vertices[vertexCount+i].g = terraFaceGrid[TOP].g / 256.0f;
			vertices[vertexCount+i].b = terraFaceGrid[TOP].b / 256.0f;
			vertices[vertexCount+i].a = 1;
		}

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( faceGrid[BOTTOM] ) // Set TOP of the block on BOTTOM
	{
		vertices[vertexCount+0].x = gridvList[0].x;
		vertices[vertexCount+0].y = gridvList[0].y;
		vertices[vertexCount+0].z = gridvList[0].z;

		vertices[vertexCount+1].x = gridvList[3].x;
		vertices[vertexCount+1].y = gridvList[3].y;
		vertices[vertexCount+1].z = gridvList[3].z;

		vertices[vertexCount+2].x = gridvList[2].x;
		vertices[vertexCount+2].y = gridvList[2].y;
		vertices[vertexCount+2].z = gridvList[2].z;

		vertices[vertexCount+3].x = gridvList[1].x;
		vertices[vertexCount+3].y = gridvList[1].y;
		vertices[vertexCount+3].z = gridvList[1].z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = +1;
		}

		// Set UV's
		//SetInFaceUVs( vertices, TOP, faceGrid[BOTTOM] );
		//SetFaceColors( vertices, BOTTOM, faceGrid[BOTTOM], pBoob->data[index16].data[index8].data[indexd].light );
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].u = 0;
			vertices[vertexCount+i].v = 0;
			vertices[vertexCount+i].w = 0;
			vertices[vertexCount+i].r = terraFaceGrid[BOTTOM].r / 256.0f;
			vertices[vertexCount+i].g = terraFaceGrid[BOTTOM].g / 256.0f;
			vertices[vertexCount+i].b = terraFaceGrid[BOTTOM].b / 256.0f;
			vertices[vertexCount+i].a = 1;
		}

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( faceGrid[FRONT] ) // Set BACK of the block on FRONT
	{
		vertices[vertexCount+0].x = gridvList[1].x;
		vertices[vertexCount+0].y = gridvList[1].y;
		vertices[vertexCount+0].z = gridvList[1].z;

		vertices[vertexCount+1].x = gridvList[2].x;
		vertices[vertexCount+1].y = gridvList[2].y;
		vertices[vertexCount+1].z = gridvList[2].z;

		vertices[vertexCount+2].x = gridvList[6].x;
		vertices[vertexCount+2].y = gridvList[6].y;
		vertices[vertexCount+2].z = gridvList[6].z;

		vertices[vertexCount+3].x = gridvList[5].x;
		vertices[vertexCount+3].y = gridvList[5].y;
		vertices[vertexCount+3].z = gridvList[5].z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = -1;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		//SetInFaceUVs( vertices, BACK, faceGrid[FRONT] );
		//SetFaceColors( vertices, FRONT, faceGrid[FRONT], pBoob->data[index16].data[index8].data[indexd].light );
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].u = 0;
			vertices[vertexCount+i].v = 0;
			vertices[vertexCount+i].w = 0;
			vertices[vertexCount+i].r = terraFaceGrid[FRONT].r / 256.0f;
			vertices[vertexCount+i].g = terraFaceGrid[FRONT].g / 256.0f;
			vertices[vertexCount+i].b = terraFaceGrid[FRONT].b / 256.0f;
			vertices[vertexCount+i].a = 1;
		}

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( faceGrid[BACK] ) // Set FRONT of the block on BACK
	{
		vertices[vertexCount+0].x = gridvList[0].x;
		vertices[vertexCount+0].y = gridvList[0].y;
		vertices[vertexCount+0].z = gridvList[0].z;

		vertices[vertexCount+1].x = gridvList[4].x;
		vertices[vertexCount+1].y = gridvList[4].y;
		vertices[vertexCount+1].z = gridvList[4].z;

		vertices[vertexCount+2].x = gridvList[7].x;
		vertices[vertexCount+2].y = gridvList[7].y;
		vertices[vertexCount+2].z = gridvList[7].z;

		vertices[vertexCount+3].x = gridvList[3].x;
		vertices[vertexCount+3].y = gridvList[3].y;
		vertices[vertexCount+3].z = gridvList[3].z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = +1;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		//SetInFaceUVs( vertices, FRONT, faceGrid[BACK] );
		//SetFaceColors( vertices, BACK, faceGrid[BACK], pBoob->data[index16].data[index8].data[indexd].light );
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].u = 0;
			vertices[vertexCount+i].v = 0;
			vertices[vertexCount+i].w = 0;
			vertices[vertexCount+i].r = terraFaceGrid[BACK].r / 256.0f;
			vertices[vertexCount+i].g = terraFaceGrid[BACK].g / 256.0f;
			vertices[vertexCount+i].b = terraFaceGrid[BACK].b / 256.0f;
			vertices[vertexCount+i].a = 1;
		}

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( faceGrid[LEFT] ) // Set RIGHT of the block on LEFT
	{
		vertices[vertexCount+0].x = gridvList[3].x;
		vertices[vertexCount+0].y = gridvList[3].y;
		vertices[vertexCount+0].z = gridvList[3].z;

		vertices[vertexCount+1].x = gridvList[7].x;
		vertices[vertexCount+1].y = gridvList[7].y;
		vertices[vertexCount+1].z = gridvList[7].z;

		vertices[vertexCount+2].x = gridvList[6].x;
		vertices[vertexCount+2].y = gridvList[6].y;
		vertices[vertexCount+2].z = gridvList[6].z;

		vertices[vertexCount+3].x = gridvList[2].x;
		vertices[vertexCount+3].y = gridvList[2].y;
		vertices[vertexCount+3].z = gridvList[2].z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = -1;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		//SetInFaceUVs( vertices, RIGHT, faceGrid[LEFT] );
		//SetFaceColors( vertices, LEFT, faceGrid[LEFT], pBoob->data[index16].data[index8].data[indexd].light );
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].u = 0;
			vertices[vertexCount+i].v = 0;
			vertices[vertexCount+i].w = 0;
			vertices[vertexCount+i].r = terraFaceGrid[LEFT].r / 256.0f;
			vertices[vertexCount+i].g = terraFaceGrid[LEFT].g / 256.0f;
			vertices[vertexCount+i].b = terraFaceGrid[LEFT].b / 256.0f;
			vertices[vertexCount+i].a = 1;
		}

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( faceGrid[RIGHT] ) // Set LEFT of the block on RIGHT
	{
		vertices[vertexCount+0].x = gridvList[0].x;
		vertices[vertexCount+0].y = gridvList[0].y;
		vertices[vertexCount+0].z = gridvList[0].z;

		vertices[vertexCount+1].x = gridvList[1].x;
		vertices[vertexCount+1].y = gridvList[1].y;
		vertices[vertexCount+1].z = gridvList[1].z;

		vertices[vertexCount+2].x = gridvList[5].x;
		vertices[vertexCount+2].y = gridvList[5].y;
		vertices[vertexCount+2].z = gridvList[5].z;

		vertices[vertexCount+3].x = gridvList[4].x;
		vertices[vertexCount+3].y = gridvList[4].y;
		vertices[vertexCount+3].z = gridvList[4].z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = +1;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		//SetInFaceUVs( vertices, LEFT, faceGrid[RIGHT] );
		//SetFaceColors( vertices, RIGHT, faceGrid[RIGHT], pBoob->data[index16].data[index8].data[indexd].light );
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].u = 0;
			vertices[vertexCount+i].v = 0;
			vertices[vertexCount+i].w = 0;
			vertices[vertexCount+i].r = terraFaceGrid[RIGHT].r / 256.0f;
			vertices[vertexCount+i].g = terraFaceGrid[RIGHT].g / 256.0f;
			vertices[vertexCount+i].b = terraFaceGrid[RIGHT].b / 256.0f;
			vertices[vertexCount+i].a = 1;
		}

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
}
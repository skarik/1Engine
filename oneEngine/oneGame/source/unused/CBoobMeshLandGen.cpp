
// ======== CBoobMeshLandGen =======
// Version 7 of the mesh generation algorithm

#include "CBoob.h"
#include "CRenderState.h"
#include "CVoxelTerrain.h"



inline void CBoobMesh::SetFaceColors( CTerrainVertex* vertices, const EFaceDir dir, unsigned short data, unsigned char light )
{
	float lightLevel = light * 0.140625f;
	float reflectLevel = 0.0f;
	float satLevel = 1.0f;
	float invGlowLevel = 1.0f;

	if ( data == EB_MUD ) {
		satLevel = 0.5f;
		reflectLevel = 0.6f;
	}
	else if ( data == EB_CRYSTAL ) {
		reflectLevel = 0.8f;
	}
	else if ( data == EB_XPLO_CRYSTAL ) {
		satLevel = 0.3f;
		reflectLevel = 0.8f;
	}
	else if ( data == EB_ASH ) {
		satLevel = 0.1f;
	}
	else if ( data == EB_SNOW || data == EB_TOP_SNOW ) {
		satLevel = 0.0f;
		reflectLevel = 0.3f;
	}
	else if ( data == EB_DEADSTONE ) {
		reflectLevel = 0.3f;
	}
	else if ( data == EB_CURSED_DEADSTONE ) {
		reflectLevel = 0.3f;
		invGlowLevel = 0.0f;
	}
	else if ( data == EB_ICE ) {
		reflectLevel = 0.9f;
		satLevel = 0.8f;
	}
	else if ( data == EB_HEMATITE ) {
		reflectLevel = 0.6f;
	}

	vertices[vertexCount+0].r = lightLevel;
	vertices[vertexCount+0].g = reflectLevel;
	vertices[vertexCount+0].b = satLevel;
	vertices[vertexCount+0].a = invGlowLevel;

	vertices[vertexCount+1].r = lightLevel;
	vertices[vertexCount+1].g = reflectLevel;
	vertices[vertexCount+1].b = satLevel;
	vertices[vertexCount+1].a = invGlowLevel;

	vertices[vertexCount+2].r = lightLevel;
	vertices[vertexCount+2].g = reflectLevel;
	vertices[vertexCount+2].b = satLevel;
	vertices[vertexCount+2].a = invGlowLevel;

	vertices[vertexCount+3].r = lightLevel;
	vertices[vertexCount+3].g = reflectLevel;
	vertices[vertexCount+3].b = satLevel;
	vertices[vertexCount+3].a = invGlowLevel;
}

inline void CBoobMesh::SetInFaceUVs ( CTerrainVertex* vertices, const EFaceDir dir, unsigned short data )
{
	// Set UV's
	vertices[vertexCount+0].u = 0;
	vertices[vertexCount+0].v = 0;

	vertices[vertexCount+1].u = 0;
	vertices[vertexCount+1].v = 1;

	vertices[vertexCount+2].u = 1;
	vertices[vertexCount+2].v = 1;

	vertices[vertexCount+3].u = 1;
	vertices[vertexCount+3].v = 0;

	float texScale = 1.0f;
	switch ( data ) {
		case EB_RIGDESTONE:
			texScale = 1/8.0f;
		break;

		case EB_DIRT:	case EB_GRASS:	case EB_SAND:
		case EB_CLAY:	case EB_MUD:	case EB_GRAVEL:
		case EB_ROAD_GRAVEL:	case EB_ASH:
		case EB_DEADSTONE:		case EB_CURSED_DEADSTONE:
		case EB_SNOW:			case EB_TOP_SNOW:
		case EB_HEMATITE:
			texScale = 1/4.0f;
		break;
	
		case EB_STONE:	case EB_STONEBRICK:
		case EB_ROOF_0:	case EB_WOOD:
			texScale = 1/2.0f;
		break;
	
		case EB_ROOF_1:
			//texScale = 1/1.5f;
			texScale = 1/2.0f;
		break;

		case EB_CRYSTAL:	case EB_XPLO_CRYSTAL:
		case EB_ICE:
			texScale = 1/4.0f;
		break;
	};

	if (( data == EB_GRASS )&&( dir != TOP )&&( dir != BOTTOM ))
	{
		texScale = 0.5f;
	}

	// Position Based UV coordinates.
	float dummy;
	if (( dir == TOP )||( dir == BOTTOM ))
	{
		vertices[vertexCount+0].u = modf(vertices[vertexCount+0].x*texScale*0.5f,&dummy);
		vertices[vertexCount+0].v = modf(vertices[vertexCount+0].y*texScale*0.5f,&dummy);
	}
	else if (( dir == LEFT )||( dir == RIGHT ))
	{
		vertices[vertexCount+0].u = modf(vertices[vertexCount+0].x*texScale*0.5f,&dummy);
		vertices[vertexCount+0].v = modf(vertices[vertexCount+0].z*texScale*0.5f,&dummy);
	}
	else if (( dir == FRONT )||( dir == BACK ))
	{
		vertices[vertexCount+0].u = modf(vertices[vertexCount+0].y*texScale*0.5f,&dummy);
		vertices[vertexCount+0].v = modf(vertices[vertexCount+0].z*texScale*0.5f,&dummy);
	}
	if ( vertices[vertexCount+0].u < 0 )
		vertices[vertexCount+0].u += 1;
	if ( vertices[vertexCount+0].v < 0 )
		vertices[vertexCount+0].v += 1;

	if (( data == EB_GRASS )&&( dir != TOP )&&( dir != BOTTOM ))
	{
		vertices[vertexCount+0].v += 0.25f;
		if ( vertices[vertexCount+0].v > 0.5f )
		{
			vertices[vertexCount+0].v -= 0.5f;
		}
	}

	if (( dir == TOP )||( dir == FRONT )||( dir == RIGHT ))
	{
		vertices[vertexCount+3].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+3].v = vertices[vertexCount+0].v;

		vertices[vertexCount+2].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;

		vertices[vertexCount+1].u = vertices[vertexCount+0].u;
		vertices[vertexCount+1].v = vertices[vertexCount+0].v+texScale;
	}
	else // Flip the texture
	{
		vertices[vertexCount+1].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+1].v = vertices[vertexCount+0].v;

		vertices[vertexCount+2].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;

		vertices[vertexCount+3].u = vertices[vertexCount+0].u;
		vertices[vertexCount+3].v = vertices[vertexCount+0].v+texScale;
	}

	//if (( dir != TOP )&&( dir != BOTTOM ))
	//{
	//	if (( dir == FRONT )||( dir == RIGHT ))
	//	{
	//		vertices[vertexCount+1].v = vertices[vertexCount+0].v;
	//		vertices[vertexCount+0].v = vertices[vertexCount+0].v;
	//		vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;
	//		vertices[vertexCount+3].v = vertices[vertexCount+0].v+texScale;
	//	}
	//	else
	//	{
	//		vertices[vertexCount+3].v = vertices[vertexCount+0].v;
	//		vertices[vertexCount+2].v = vertices[vertexCount+0].v;
	//		vertices[vertexCount+1].v = vertices[vertexCount+0].v+texScale;
	//		vertices[vertexCount+0].v = vertices[vertexCount+0].v+texScale;
	//	}
	//}

	//atlas is 4x4
	float border = 1/1024.0f;
	for ( unsigned int i = 0; i < 4; i += 1 )
	{
		vertices[vertexCount+i].u *= 0.25f-(border*2);
		vertices[vertexCount+i].v *= 0.25f-(border*2);

		switch ( data )
		{
		case EB_GRASS:
			if ( dir == TOP ) {
				vertices[vertexCount+i].u += 0.0;
			}
			else if ( dir == BOTTOM ) {
				vertices[vertexCount+i].u += 0.50; //Dirt on bottom
			}
			else {
				vertices[vertexCount+i].u += 0.25;
				vertices[vertexCount+i].v += 0.25;
			}
			break;
		case EB_DIRT:
		case EB_MUD:
			vertices[vertexCount+i].u += 0.50;
			break;
		case EB_STONE:
			vertices[vertexCount+i].u += 0.25;
			break;
		case EB_RIGDESTONE:
			vertices[vertexCount+i].v += 0.75;
			break;
		case EB_STONEBRICK:
			vertices[vertexCount+i].u += 0.50;
			vertices[vertexCount+i].v += 0.25;
			break;
		case EB_SAND:
		case EB_ASH:
		case EB_SNOW:
		case EB_TOP_SNOW:
			vertices[vertexCount+i].u += 0.75;
			break;
		case EB_CLAY:
			vertices[vertexCount+i].v += 0.25;
			break;
		case EB_WATER: // DEBUGGGGG TODO
			vertices[vertexCount+i].u += 0.75;
			vertices[vertexCount+i].v += 0.75;
			break;
		case EB_WOOD:
			vertices[vertexCount+i].u += 0.25;
			vertices[vertexCount+i].v += 0.50;
			break;
		case EB_CRYSTAL:
		case EB_XPLO_CRYSTAL:
		case EB_ICE:
			vertices[vertexCount+i].v += 0.50;
			break;
		case EB_GRAVEL:
		case EB_ROAD_GRAVEL:
			vertices[vertexCount+i].u += 0.50;
			vertices[vertexCount+i].v += 0.50;
			break;
		case EB_DEADSTONE:
		case EB_CURSED_DEADSTONE:
			vertices[vertexCount+i].u += 0.75;
			vertices[vertexCount+i].v += 0.50;
			break;
		case EB_ROOF_0:
			vertices[vertexCount+i].u += 0.25;
			vertices[vertexCount+i].v += 0.75;
			break;
		case EB_ROOF_1:
			vertices[vertexCount+i].u += 0.75;
			vertices[vertexCount+i].v += 0.25;
			break;
		case EB_HEMATITE:
			vertices[vertexCount+i].u += 0.50;
			vertices[vertexCount+i].v += 0.75;
			break;
		}

		vertices[vertexCount+i].u += border;
		vertices[vertexCount+i].v += border;
	}


	//if (( data == EB_GRASS )&&( dir != TOP )&&( dir != BOTTOM ))
	//{
	//	for ( char i = 0; i < 4; ++i )
	//	{
	//		vertices[vertexCount+i].v = 0.75f-vertices[vertexCount+i].v;
	//	}
	//}
}


void CBoobMesh::AddBlockMesh( CBoob * pBoob, char const index16, char const index8, short const indexd, Vector3d const& pos, char const res )
{
	static const int bsize = 2;
	char i;

	if ( vertexCount >= 32768 )
	{
		vertexCount = 32768;
		return;
	}

	// Create the face block type list
	terra_t terraFaceGrid [7];
	terraFaceGrid[0] = pBoob->data[index16].data[index8].data[indexd];
	for ( i = 1; i < 7; ++i )
		terraFaceGrid[i] = GetBlockValue( pBoob, index16, index8, (terra_t*)pBoob->data[index16].data[index8].data,indexd,8, (EFaceDir)(i), res );

	unsigned short faceGrid [7];
	for ( i = 0; i < 7; ++i )
		faceGrid[i] = terraFaceGrid[i].block;

	// Create the cube point list
	Vector3d gridvList[8];
	for ( char i = 0; i < 4; ++i )
		gridvList[i] = pos;
	gridvList[1].x += bsize*res;
	gridvList[2].x += bsize*res;
	gridvList[2].y += bsize*res;
	gridvList[3].y += bsize*res;
	for ( i = 4; i < 8; ++i )
	{
		gridvList[i] = gridvList[i-4];
		gridvList[i].z += bsize*res;
	}

	// Create the mesh
	if ( IsOpaque(faceGrid[TOP]) ) // Set BOTTOM of the block on TOP
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
/*
			vertices[vertexCount+0].tx = 0;
			vertices[vertexCount+0].ty = 0;
			vertices[vertexCount+0].tz = 0;*/
		}

		// Set UV's
		SetInFaceUVs( vertices, BOTTOM, faceGrid[TOP] );
		SetFaceColors( vertices, TOP, faceGrid[TOP], pBoob->data[index16].data[index8].data[indexd].light );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( IsOpaque(faceGrid[BOTTOM]) ) // Set TOP of the block on BOTTOM
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
		SetInFaceUVs( vertices, TOP, faceGrid[BOTTOM] );
		SetFaceColors( vertices, BOTTOM, faceGrid[BOTTOM], pBoob->data[index16].data[index8].data[indexd].light );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( IsOpaque(faceGrid[FRONT]) ) // Set BACK of the block on FRONT
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
		SetInFaceUVs( vertices, BACK, faceGrid[FRONT] );
		SetFaceColors( vertices, FRONT, faceGrid[FRONT], pBoob->data[index16].data[index8].data[indexd].light );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( IsOpaque(faceGrid[BACK]) ) // Set FRONT of the block on BACK
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
		SetInFaceUVs( vertices, FRONT, faceGrid[BACK] );
		SetFaceColors( vertices, BACK, faceGrid[BACK], pBoob->data[index16].data[index8].data[indexd].light );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( IsOpaque(faceGrid[LEFT]) ) // Set RIGHT of the block on LEFT
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
		SetInFaceUVs( vertices, RIGHT, faceGrid[LEFT] );
		SetFaceColors( vertices, LEFT, faceGrid[LEFT], pBoob->data[index16].data[index8].data[indexd].light );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	if ( IsOpaque(faceGrid[RIGHT]) ) // Set LEFT of the block on RIGHT
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
		SetInFaceUVs( vertices, LEFT, faceGrid[RIGHT] );
		SetFaceColors( vertices, RIGHT, faceGrid[RIGHT], pBoob->data[index16].data[index8].data[indexd].light );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
}

CBoobMesh::terra_t CBoobMesh::GetBlockValue ( CBoob * pBoob, char const index16, char const index8, terra_t * data, int const i, int const width, EFaceDir const dir, unsigned char ofs )
{
	//static unsigned short checkdata;
	static terra_t checkdata;
	checkdata.raw = 0;

	switch ( dir )
	{
	case FRONT:
		{
			if ( i%width < width-ofs )
			{
				checkdata = data[i+ofs];
			}
			else
			{
				if ( index8%2 != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+1].data[i-(width-ofs)];
				}
				else if ( index16%2 != 1 )
				{
					checkdata = pBoob->data[index16+1].data[index8-1].data[i-(width-ofs)];
				}
				else if ( pBoob->front != NULL )
				{
					checkdata = pBoob->front->data[index16-1].data[index8-1].data[i-(width-ofs)];
				}
				else
				{
					return checkdata;
				}
			}
		}
		break;
	case BACK:
		{
			if ( i%width >= ofs )
			{
				checkdata = data[i-ofs];
			}
			else
			{
				if ( index8%2 != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-1].data[i+(width-ofs)];
				}
				else if ( index16%2 != 0 )
				{
					checkdata = pBoob->data[index16-1].data[index8+1].data[i+(width-ofs)];
				}
				else if ( pBoob->back != NULL )
				{
					checkdata = pBoob->back->data[index16+1].data[index8+1].data[i+(width-ofs)];
				}
				else
				{
					return checkdata;
				}
			}
		}
		break;
	case LEFT:
		{
			if ( ((i/width)%width) < width-ofs )
			{
				checkdata = data[i+(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-ofs))];
				}
				else if ( ((index16/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16+2].data[index8-2].data[i-(width*(width-ofs))];
				}
				else if ( pBoob->left != NULL )
				{
					checkdata = pBoob->left->data[index16-2].data[index8-2].data[i-(width*(width-ofs))];
				}
				else
				{
					return checkdata;
				}
			}
		}
		break;
	case RIGHT:
		{
			if ( ((i/width)%width) >= ofs )
			{
				checkdata = data[i-(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-ofs))];
				}
				else if ( ((index16/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16-2].data[index8+2].data[i+(width*(width-ofs))];
				}
				else if ( pBoob->right != NULL )
				{
					checkdata = pBoob->right->data[index16+2].data[index8+2].data[i+(width*(width-ofs))];
				}
				else
				{
					return checkdata;
				}
			}
		}
		break;
	case TOP:
		{
			if ( ((i/width/width)%width) != width-ofs )
			{
				checkdata = data[i+(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+4].data[i-(width*width*(width-ofs))];
				}
				else if ( ((index16/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16+4].data[index8-4].data[i-(width*width*(width-ofs))];
				}
				else if ( pBoob->top != NULL )
				{
					checkdata = pBoob->top->data[index16-4].data[index8-4].data[i-(width*width*(width-ofs))];
				}
				else
				{
					return checkdata;
				}
			}
		}
		break;
	case BOTTOM:
		{
			if ( ((i/width/width)%width) >= ofs )
			{
				checkdata = data[i-(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-4].data[i+(width*width*(width-ofs))];
				}
				else if ( ((index16/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16-4].data[index8+4].data[i+(width*width*(width-ofs))];
				}
				else if ( pBoob->bottom != NULL )
				{
					checkdata = pBoob->bottom->data[index16+4].data[index8+4].data[i+(width*width*(width-ofs))];
				}
				else
				{
					return checkdata;
				}
			}
		}
		break;
	}
	return checkdata;
}

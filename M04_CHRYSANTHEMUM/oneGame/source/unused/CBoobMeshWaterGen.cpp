// Water Gen

#include "CBoob.h"
#include "CRenderState.h"
#include "CVoxelTerrain.h"

void CBoobMesh::GenerateInCubeWater ( CBoob * pBoob, char const index16, char const index8, int const indexd, Vector3d const& pos, char const res )
{
	static const int bsize = 2;
	char i;

	if ( vertexCountWater >= 32768 )
	{
		vertexCountWater = 32768;
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
		gridvList[i] = pos+Vector3d( 0,0,-0.1f );
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
	if ( IsFluid(faceGrid[TOP]) ) // Set BOTTOM of the block on TOP
	{
		vertices[vertexCountWater+0].x = gridvList[4].x;
		vertices[vertexCountWater+0].y = gridvList[4].y;
		vertices[vertexCountWater+0].z = gridvList[4].z;

		vertices[vertexCountWater+1].x = gridvList[5].x;
		vertices[vertexCountWater+1].y = gridvList[5].y;
		vertices[vertexCountWater+1].z = gridvList[5].z;

		vertices[vertexCountWater+2].x = gridvList[6].x;
		vertices[vertexCountWater+2].y = gridvList[6].y;
		vertices[vertexCountWater+2].z = gridvList[6].z;

		vertices[vertexCountWater+3].x = gridvList[7].x;
		vertices[vertexCountWater+3].y = gridvList[7].y;
		vertices[vertexCountWater+3].z = gridvList[7].z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = 0;
			vertices[vertexCountWater+i].ny = 0;
			vertices[vertexCountWater+i].nz = -1;
		}

		// Set UV's
		SetInFaceWaterUVs( vertices, BOTTOM, faceGrid[TOP] );
		SetWaterFaceColors( vertices, TOP, pBoob->data[index16].data[index8].data[indexd].light * 36 );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	if ( IsFluid(faceGrid[BOTTOM]) ) // Set TOP of the block on BOTTOM
	{
		vertices[vertexCountWater+0].x = gridvList[0].x;
		vertices[vertexCountWater+0].y = gridvList[0].y;
		vertices[vertexCountWater+0].z = gridvList[0].z;

		vertices[vertexCountWater+1].x = gridvList[3].x;
		vertices[vertexCountWater+1].y = gridvList[3].y;
		vertices[vertexCountWater+1].z = gridvList[3].z;

		vertices[vertexCountWater+2].x = gridvList[2].x;
		vertices[vertexCountWater+2].y = gridvList[2].y;
		vertices[vertexCountWater+2].z = gridvList[2].z;

		vertices[vertexCountWater+3].x = gridvList[1].x;
		vertices[vertexCountWater+3].y = gridvList[1].y;
		vertices[vertexCountWater+3].z = gridvList[1].z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = 0;
			vertices[vertexCountWater+i].ny = 0;
			vertices[vertexCountWater+i].nz = +1;
		}

		// Set UV's
		SetInFaceWaterUVs( vertices, TOP, faceGrid[BOTTOM] );
		SetWaterFaceColors( vertices, BOTTOM, pBoob->data[index16].data[index8].data[indexd].light * 36 );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	if ( IsFluid(faceGrid[FRONT]) ) // Set BACK of the block on FRONT
	{
		vertices[vertexCountWater+0].x = gridvList[1].x;
		vertices[vertexCountWater+0].y = gridvList[1].y;
		vertices[vertexCountWater+0].z = gridvList[1].z;

		vertices[vertexCountWater+1].x = gridvList[2].x;
		vertices[vertexCountWater+1].y = gridvList[2].y;
		vertices[vertexCountWater+1].z = gridvList[2].z;

		vertices[vertexCountWater+2].x = gridvList[6].x;
		vertices[vertexCountWater+2].y = gridvList[6].y;
		vertices[vertexCountWater+2].z = gridvList[6].z;

		vertices[vertexCountWater+3].x = gridvList[5].x;
		vertices[vertexCountWater+3].y = gridvList[5].y;
		vertices[vertexCountWater+3].z = gridvList[5].z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = -1;
			vertices[vertexCountWater+i].ny = 0;
			vertices[vertexCountWater+i].nz = 0;
		}

		// Set UV's
		SetInFaceWaterUVs( vertices, BACK, faceGrid[FRONT] );
		SetWaterFaceColors( vertices, FRONT, pBoob->data[index16].data[index8].data[indexd].light * 36 );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	if ( IsFluid(faceGrid[BACK]) ) // Set FRONT of the block on BACK
	{
		vertices[vertexCountWater+0].x = gridvList[0].x;
		vertices[vertexCountWater+0].y = gridvList[0].y;
		vertices[vertexCountWater+0].z = gridvList[0].z;

		vertices[vertexCountWater+1].x = gridvList[4].x;
		vertices[vertexCountWater+1].y = gridvList[4].y;
		vertices[vertexCountWater+1].z = gridvList[4].z;

		vertices[vertexCountWater+2].x = gridvList[7].x;
		vertices[vertexCountWater+2].y = gridvList[7].y;
		vertices[vertexCountWater+2].z = gridvList[7].z;

		vertices[vertexCountWater+3].x = gridvList[3].x;
		vertices[vertexCountWater+3].y = gridvList[3].y;
		vertices[vertexCountWater+3].z = gridvList[3].z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = +1;
			vertices[vertexCountWater+i].ny = 0;
			vertices[vertexCountWater+i].nz = 0;
		}

		// Set UV's
		SetInFaceWaterUVs( vertices, FRONT, faceGrid[BACK] );
		SetWaterFaceColors( vertices, BACK, pBoob->data[index16].data[index8].data[indexd].light * 36 );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	if ( IsFluid(faceGrid[LEFT]) ) // Set RIGHT of the block on LEFT
	{
		vertices[vertexCountWater+0].x = gridvList[3].x;
		vertices[vertexCountWater+0].y = gridvList[3].y;
		vertices[vertexCountWater+0].z = gridvList[3].z;

		vertices[vertexCountWater+1].x = gridvList[7].x;
		vertices[vertexCountWater+1].y = gridvList[7].y;
		vertices[vertexCountWater+1].z = gridvList[7].z;

		vertices[vertexCountWater+2].x = gridvList[6].x;
		vertices[vertexCountWater+2].y = gridvList[6].y;
		vertices[vertexCountWater+2].z = gridvList[6].z;

		vertices[vertexCountWater+3].x = gridvList[2].x;
		vertices[vertexCountWater+3].y = gridvList[2].y;
		vertices[vertexCountWater+3].z = gridvList[2].z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = 0;
			vertices[vertexCountWater+i].ny = -1;
			vertices[vertexCountWater+i].nz = 0;
		}

		// Set UV's
		SetInFaceWaterUVs( vertices, RIGHT, faceGrid[LEFT] );
		SetWaterFaceColors( vertices, LEFT, pBoob->data[index16].data[index8].data[indexd].light * 36 );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	if ( IsFluid(faceGrid[RIGHT]) ) // Set LEFT of the block on RIGHT
	{
		vertices[vertexCountWater+0].x = gridvList[0].x;
		vertices[vertexCountWater+0].y = gridvList[0].y;
		vertices[vertexCountWater+0].z = gridvList[0].z;

		vertices[vertexCountWater+1].x = gridvList[1].x;
		vertices[vertexCountWater+1].y = gridvList[1].y;
		vertices[vertexCountWater+1].z = gridvList[1].z;

		vertices[vertexCountWater+2].x = gridvList[5].x;
		vertices[vertexCountWater+2].y = gridvList[5].y;
		vertices[vertexCountWater+2].z = gridvList[5].z;

		vertices[vertexCountWater+3].x = gridvList[4].x;
		vertices[vertexCountWater+3].y = gridvList[4].y;
		vertices[vertexCountWater+3].z = gridvList[4].z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = 0;
			vertices[vertexCountWater+i].ny = +1;
			vertices[vertexCountWater+i].nz = 0;
		}

		// Set UV's
		SetInFaceWaterUVs( vertices, LEFT, faceGrid[RIGHT] );
		SetWaterFaceColors( vertices, RIGHT, pBoob->data[index16].data[index8].data[indexd].light * 36 );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
}

inline void CBoobMesh::SetWaterFaceColors( CTerrainVertex* vertices, const EFaceDir dir, unsigned char light )
{
	float lightLevel = light/256.0f;
	vertices[vertexCountWater+0].r = lightLevel;
	vertices[vertexCountWater+0].g = lightLevel;
	vertices[vertexCountWater+0].b = lightLevel;
	vertices[vertexCountWater+0].a = 0.9f;

	vertices[vertexCountWater+1].r = lightLevel;
	vertices[vertexCountWater+1].g = lightLevel;
	vertices[vertexCountWater+1].b = lightLevel;
	vertices[vertexCountWater+1].a = 0.9f;

	vertices[vertexCountWater+2].r = lightLevel;
	vertices[vertexCountWater+2].g = lightLevel;
	vertices[vertexCountWater+2].b = lightLevel;
	vertices[vertexCountWater+2].a = 0.9f;

	vertices[vertexCountWater+3].r = lightLevel;
	vertices[vertexCountWater+3].g = lightLevel;
	vertices[vertexCountWater+3].b = lightLevel;
	vertices[vertexCountWater+3].a = 0.9f;
}
inline void CBoobMesh::SetInFaceWaterUVs ( CTerrainVertex* vertices, const EFaceDir dir, unsigned short data )
{
	// Set UV's
	vertices[vertexCountWater+0].u = 0;
	vertices[vertexCountWater+0].v = 0;

	vertices[vertexCountWater+1].u = 0;
	vertices[vertexCountWater+1].v = 1;

	vertices[vertexCountWater+2].u = 1;
	vertices[vertexCountWater+2].v = 1;

	vertices[vertexCountWater+3].u = 1;
	vertices[vertexCountWater+3].v = 0;

	float texScale = 1.0f;
	/*if (( data == EB_DIRT )||( data == EB_GRASS )||( data == EB_SAND ))
	{
		texScale = 1/4.0f;
	}
	else if ( data == EB_STONE )
	{
		texScale = 1/2.0f;
	}*/
	if ( ( data == EB_WATER ) )
	{
		texScale = 1/4.0f;
	}

	// Position Based UV coordinates.
	float dummy;
	if (( dir == TOP )||( dir == BOTTOM ))
	{
		vertices[vertexCountWater+0].u = modf(vertices[vertexCountWater+0].x*texScale*0.5f,&dummy);
		vertices[vertexCountWater+0].v = modf(vertices[vertexCountWater+0].y*texScale*0.5f,&dummy);
	}
	else if (( dir == LEFT )||( dir == RIGHT ))
	{
		vertices[vertexCountWater+0].u = modf(vertices[vertexCountWater+0].x*texScale*0.5f,&dummy);
		vertices[vertexCountWater+0].v = modf(vertices[vertexCountWater+0].z*texScale*0.5f,&dummy);
	}
	else if (( dir == FRONT )||( dir == BACK ))
	{
		vertices[vertexCountWater+0].u = modf(vertices[vertexCountWater+0].y*texScale*0.5f,&dummy);
		vertices[vertexCountWater+0].v = modf(vertices[vertexCountWater+0].z*texScale*0.5f,&dummy);
	}
	if ( vertices[vertexCountWater+0].u < 0 )
		vertices[vertexCountWater+0].u += 1;
	if ( vertices[vertexCountWater+0].v < 0 )
		vertices[vertexCountWater+0].v += 1;

	if (( dir == TOP )||( dir == FRONT )||( dir == RIGHT ))
	{
		vertices[vertexCountWater+3].u = vertices[vertexCountWater+0].u+texScale;
		vertices[vertexCountWater+3].v = vertices[vertexCountWater+0].v;

		vertices[vertexCountWater+2].u = vertices[vertexCountWater+0].u+texScale;
		vertices[vertexCountWater+2].v = vertices[vertexCountWater+0].v+texScale;

		vertices[vertexCountWater+1].u = vertices[vertexCountWater+0].u;
		vertices[vertexCountWater+1].v = vertices[vertexCountWater+0].v+texScale;
	}
	else
	{
		vertices[vertexCountWater+1].u = vertices[vertexCountWater+0].u+texScale;
		vertices[vertexCountWater+1].v = vertices[vertexCountWater+0].v;

		vertices[vertexCountWater+2].u = vertices[vertexCountWater+0].u+texScale;
		vertices[vertexCountWater+2].v = vertices[vertexCountWater+0].v+texScale;

		vertices[vertexCountWater+3].u = vertices[vertexCountWater+0].u;
		vertices[vertexCountWater+3].v = vertices[vertexCountWater+0].v+texScale;
	}

	//atlas is 2x2
	float border = 1/1024.0f;
	for ( unsigned int i = 0; i < 4; i += 1 )
	{
		vertices[vertexCountWater+i].u *= 0.50f-(border*2);
		vertices[vertexCountWater+i].v *= 0.50f-(border*2);

		switch ( data )
		{
		case EB_WATER: // DEBUGGGGG TODO
			vertices[vertexCountWater+i].u += 0.0f;
			vertices[vertexCountWater+i].v += 0.0f;
			break;
		//case EB_LAVA:
		//	vertices[vertexCountWater+i].u += 0.0f;
		//	vertices[vertexCountWater+i].v += 0.5f;
		//	break;
		}

		vertices[vertexCountWater+i].u += border;
		vertices[vertexCountWater+i].v += border;
	}
}
/*
bool CBoobMesh::InSideVisibleWater ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs, Vector3d const& dot, char& return_block_type )
{
	//static unsigned char checkdata = EB_NONE;
	//static Vector3d vDir;
	//vDir = pos - vCameraPos;

	switch ( dir )
	{
	case FRONT:
		//if ( dot.x < 0.0f )
		{
			if ( i%width < width-ofs )
			{
				checkdata = data[i+ofs];
			}
			else
			{
				if ( index8%2 != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+1].data[i-(width-ofs)].data;
				}
				else if ( index16%2 != 1 )
				{
					checkdata = pBoob->data[index16+1].data[index8-1].data[i-(width-ofs)].data;
				}
				else if ( pBoob->front != NULL )
				{
					checkdata = pBoob->front->data[index16-1].data[index8-1].data[i-(width-ofs)].data;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case BACK:
		//if ( dot.x > 0 )
		{
			if ( i%width >= ofs )
			{
				checkdata = data[i-ofs];
			}
			else
			{
				if ( index8%2 != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-1].data[i+(width-ofs)].data;
				}
				else if ( index16%2 != 0 )
				{
					checkdata = pBoob->data[index16-1].data[index8+1].data[i+(width-ofs)].data;
				}
				else if ( pBoob->back != NULL )
				{
					checkdata = pBoob->back->data[index16+1].data[index8+1].data[i+(width-ofs)].data;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case LEFT:
		//if ( dot.y < 0 )
		{
			if ( ((i/width)%width) < width-ofs )
			{
				checkdata = data[i+(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-ofs))].data;
				}
				else if ( ((index16/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16+2].data[index8-2].data[i-(width*(width-ofs))].data;
				}
				else if ( pBoob->left != NULL )
				{
					checkdata = pBoob->left->data[index16-2].data[index8-2].data[i-(width*(width-ofs))].data;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case RIGHT:
		//if ( dot.y > 0 )
		{
			if ( ((i/width)%width) >= ofs )
			{
				checkdata = data[i-(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-ofs))].data;
				}
				else if ( ((index16/2)%2) != 0 )
				{
					checkdata = pBoob->data[index16-2].data[index8+2].data[i+(width*(width-ofs))].data;
				}
				else if ( pBoob->right != NULL )
				{
					checkdata = pBoob->right->data[index16+2].data[index8+2].data[i+(width*(width-ofs))].data;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case TOP:
		//if ( dot.z < 0 )
		{
			if ( ((i/width/width)%width) != width-ofs )
			{
				checkdata = data[i+(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+4].data[i-(width*width*(width-ofs))].data;
				}
				else if ( ((index16/4)%2) != 1 )
				{
					checkdata = pBoob->data[index16+4].data[index8-4].data[i-(width*width*(width-ofs))].data;
				}
				else if ( pBoob->top != NULL )
				{
					checkdata = pBoob->top->data[index16-4].data[index8-4].data[i-(width*width*(width-ofs))].data;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	case BOTTOM:
		//if ( dot.z > 0 )
		{
			if ( ((i/width/width)%width) >= ofs )
			{
				checkdata = data[i-(width*width*ofs)];
			}
			else
			{
				if ( ((index8/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16].data[index8-4].data[i+(width*width*(width-ofs))].data;
				}
				else if ( ((index16/4)%2) != 0 )
				{
					checkdata = pBoob->data[index16-4].data[index8+4].data[i+(width*width*(width-ofs))].data;
				}
				else if ( pBoob->bottom != NULL )
				{
					checkdata = pBoob->bottom->data[index16+4].data[index8+4].data[i+(width*width*(width-ofs))].data;
				}
				else
				{
					return false;
				}
			}
		}
		//else
		//	return false;
		break;
	}
	//return (checkdata==0);
	return_block_type = checkdata;
	//return ((checkdata!=EB_NONE)&&(checkdata!=EB_WATER));
	return (checkdata==EB_WATER);
}
*/

// 
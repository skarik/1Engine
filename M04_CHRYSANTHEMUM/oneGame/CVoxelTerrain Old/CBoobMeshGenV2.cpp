#include "CBoob.h"
#include "CToBeSeen.h"

void CBoobMesh::GenerateInCube( CBoob * pBoob, char const index16, char const index8, int const indexd, Vector3d const& pos, char const res )
{
	static const int bsize = 2;
	unsigned int i = 0;
	//static Vector3d cpos;
	//cpos = pos + Vector3d( bsize*res*0.5,bsize*res*0.5,bsize*res*0.5 );
	static Vector3d dot;
	dot = pos - vCameraPos;

	static CBoobMesh* pMesh;
	pMesh = pBoob->pMesh;

	char result_type;

	if ( vertexCount >= 32768 )
	{
		vertexCount = 32768;
		return;
	}

	// Generate FRONT
	if ( InSideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, FRONT, res, dot, result_type ) )
	{
		vertices[vertexCount+0].x = pos.x+bsize*res;
		vertices[vertexCount+0].y = pos.y;
		vertices[vertexCount+0].z = pos.z;

		vertices[vertexCount+1].x = pos.x+bsize*res;
		vertices[vertexCount+1].y = pos.y+bsize*res;
		vertices[vertexCount+1].z = pos.z;

		vertices[vertexCount+2].x = pos.x+bsize*res;
		vertices[vertexCount+2].y = pos.y+bsize*res;
		vertices[vertexCount+2].z = pos.z+bsize*res;

		vertices[vertexCount+3].x = pos.x+bsize*res;
		vertices[vertexCount+3].y = pos.y;
		vertices[vertexCount+3].z = pos.z+bsize*res;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		// Set normals
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = -1;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		//SetFaceUVs( vertices, FRONT, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceUVs( vertices, BACK, result_type );
		SetFaceColors( vertices, FRONT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate BACK
	if ( InSideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, BACK, res, dot, result_type ) )
	{
		vertices[vertexCount+0].x = pos.x;
		vertices[vertexCount+0].y = pos.y;
		vertices[vertexCount+0].z = pos.z;

		vertices[vertexCount+1].x = pos.x;
		vertices[vertexCount+1].y = pos.y;
		vertices[vertexCount+1].z = pos.z+bsize*res;

		vertices[vertexCount+2].x = pos.x;
		vertices[vertexCount+2].y = pos.y+bsize*res;
		vertices[vertexCount+2].z = pos.z+bsize*res;

		vertices[vertexCount+3].x = pos.x;
		vertices[vertexCount+3].y = pos.y+bsize*res;
		vertices[vertexCount+3].z = pos.z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 1;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		//SetFaceUVs( vertices, BACK, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceUVs( vertices, FRONT, result_type );
		SetFaceColors( vertices, BACK, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate RIGHT
	if ( InSideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, RIGHT, res, dot, result_type ) )
	{
		vertices[vertexCount+0].x = pos.x;
		vertices[vertexCount+0].y = pos.y;
		vertices[vertexCount+0].z = pos.z;

		vertices[vertexCount+1].x = pos.x+bsize*res;
		vertices[vertexCount+1].y = pos.y;
		vertices[vertexCount+1].z = pos.z;

		vertices[vertexCount+2].x = pos.x+bsize*res;
		vertices[vertexCount+2].y = pos.y;
		vertices[vertexCount+2].z = pos.z+bsize*res;

		vertices[vertexCount+3].x = pos.x;
		vertices[vertexCount+3].y = pos.y;
		vertices[vertexCount+3].z = pos.z+bsize*res;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = 1;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		//SetFaceUVs( vertices, RIGHT, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceUVs( vertices, LEFT, result_type );
		SetFaceColors( vertices, RIGHT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate LEFT
	if ( InSideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, LEFT, res, dot, result_type ) )
	{
		vertices[vertexCount+0].x = pos.x;
		vertices[vertexCount+0].y = pos.y+bsize*res;
		vertices[vertexCount+0].z = pos.z;

		vertices[vertexCount+1].x = pos.x;
		vertices[vertexCount+1].y = pos.y+bsize*res;
		vertices[vertexCount+1].z = pos.z+bsize*res;

		vertices[vertexCount+2].x = pos.x+bsize*res;
		vertices[vertexCount+2].y = pos.y+bsize*res;
		vertices[vertexCount+2].z = pos.z+bsize*res;

		vertices[vertexCount+3].x = pos.x+bsize*res;
		vertices[vertexCount+3].y = pos.y+bsize*res;
		vertices[vertexCount+3].z = pos.z;

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
		//SetFaceUVs( vertices, LEFT, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceUVs( vertices, RIGHT, result_type );
		SetFaceColors( vertices, LEFT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate TOP
	if ( InSideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, TOP, res, dot, result_type ) )
	{
		vertices[vertexCount+0].x = pos.x;
		vertices[vertexCount+0].y = pos.y;
		vertices[vertexCount+0].z = pos.z+bsize*res;

		vertices[vertexCount+1].x = pos.x+bsize*res;
		vertices[vertexCount+1].y = pos.y;
		vertices[vertexCount+1].z = pos.z+bsize*res;

		vertices[vertexCount+2].x = pos.x+bsize*res;
		vertices[vertexCount+2].y = pos.y+bsize*res;
		vertices[vertexCount+2].z = pos.z+bsize*res;

		vertices[vertexCount+3].x = pos.x;
		vertices[vertexCount+3].y = pos.y+bsize*res;
		vertices[vertexCount+3].z = pos.z+bsize*res;

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
		//SetFaceUVs( vertices, TOP, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceUVs( vertices, BOTTOM, result_type );
		SetFaceColors( vertices, TOP, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate BOTTOM
	if ( InSideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, BOTTOM, res, dot, result_type ) )
	{
		vertices[vertexCount+0].x = pos.x;
		vertices[vertexCount+0].y = pos.y;
		vertices[vertexCount+0].z = pos.z;

		vertices[vertexCount+1].x = pos.x;
		vertices[vertexCount+1].y = pos.y+bsize*res;
		vertices[vertexCount+1].z = pos.z;

		vertices[vertexCount+2].x = pos.x+bsize*res;
		vertices[vertexCount+2].y = pos.y+bsize*res;
		vertices[vertexCount+2].z = pos.z;

		vertices[vertexCount+3].x = pos.x+bsize*res;
		vertices[vertexCount+3].y = pos.y;
		vertices[vertexCount+3].z = pos.z;

		quads[faceCount].vert[0] = vertexCount+3;
		quads[faceCount].vert[1] = vertexCount+2;
		quads[faceCount].vert[2] = vertexCount+1;
		quads[faceCount].vert[3] = vertexCount+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 1;
		}

		// Set UV's
		//SetFaceUVs( vertices, BOTTOM, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceUVs( vertices, TOP, result_type );
		SetFaceColors( vertices, BOTTOM, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
}

inline void CBoobMesh::SetInFaceUVs ( CTerrainVertex* vertices, const EFaceDir dir, unsigned char data )
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
	if (( data == EB_DIRT )||( data == EB_GRASS )||( data == EB_SAND )||( data == EB_CLAY ))
	{
		texScale = 1/4.0f;
	}
	else if (( data == EB_STONE )||( data == EB_STONEBRICK ))
	{
		texScale = 1/2.0f;
	}
	else if ( data == EB_WOOD )
	{
		//texScale = 1/1.5f;
		texScale = 1/2.0f;
	}

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
	else
	{
		vertices[vertexCount+1].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+1].v = vertices[vertexCount+0].v;

		vertices[vertexCount+2].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;

		vertices[vertexCount+3].u = vertices[vertexCount+0].u;
		vertices[vertexCount+3].v = vertices[vertexCount+0].v+texScale;
	}

	/*if (( dir != TOP )&&( dir != BOTTOM ))
	{
		if (( dir == FRONT )||( dir == RIGHT ))
		{
			vertices[vertexCount+1].v = vertices[vertexCount+0].v;
			vertices[vertexCount+0].v = vertices[vertexCount+0].v;
			vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;
			vertices[vertexCount+3].v = vertices[vertexCount+0].v+texScale;
		}
		else
		{
			vertices[vertexCount+3].v = vertices[vertexCount+0].v;
			vertices[vertexCount+2].v = vertices[vertexCount+0].v;
			vertices[vertexCount+1].v = vertices[vertexCount+0].v+texScale;
			vertices[vertexCount+0].v = vertices[vertexCount+0].v+texScale;
		}
	}*/

	//atlas is 4x4
	float border = 1/1024.0f;
	for ( unsigned int i = 0; i < 4; i += 1 )
	{
		vertices[vertexCount+i].u *= 0.25f-(border*2);
		vertices[vertexCount+i].v *= 0.25f-(border*2);

		switch ( data )
		{
		case EB_GRASS:
			if ( dir == TOP )
			{
				vertices[vertexCount+i].u += 0.0;
				//vertices[vertexCount+i].v += 0.0;
			}
			else if ( dir == BOTTOM )
			{
				vertices[vertexCount+i].u += 0.50; //Dirt on bottom
				//vertices[vertexCount+i].v += 0.0;
			}
			else
			{
				vertices[vertexCount+i].u += 0.25;
				vertices[vertexCount+i].v += 0.25;
			}
			break;
		case EB_DIRT:
			vertices[vertexCount+i].u += 0.50;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case EB_STONE:
			vertices[vertexCount+i].u += 0.25;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case EB_STONEBRICK:
			vertices[vertexCount+i].u += 0.50;
			vertices[vertexCount+i].v += 0.25;
			break;
		case EB_SAND:
			vertices[vertexCount+i].u += 0.75;
			break;
		case EB_CLAY:
			//vertices[vertexCount+i].u += 0.0;
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
		}

		vertices[vertexCount+i].u += border;
		vertices[vertexCount+i].v += border;
	}


	/*if (( data == EB_GRASS )&&( dir != TOP )&&( dir != BOTTOM ))
	{
		for ( char i = 0; i < 4; ++i )
		{
			vertices[vertexCount+i].v = 0.75f-vertices[vertexCount+i].v;
		}
	}*/
}

bool CBoobMesh::InSideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs, Vector3d const& dot, char& return_block_type )
{
	static unsigned char checkdata = EB_DIRT;
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
	return ((checkdata==EB_STONEBRICK )||(checkdata==EB_WOOD ));
}
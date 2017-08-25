
#include "CBoob.h"
#include "CToBeSeen.h"
#include "CVoxelTerrain.h"


bool CBoobMesh::PrepareWaterRegen ( void )
{
	vertices =	new (std::nothrow) CTerrainVertex	[32768];
	if ( vertices == NULL )
	{
		return false;
	}
	quads =		new CModelQuad		[16384];
	if ( quads == NULL )
	{
		delete [] vertices;
		vertices = false;
		return false;
	}

	vertexCountWater = 0;
	faceCountWater	= 0;

	return true;
}


void CBoobMesh::CleanupWaterRegen ( void )
{
	delete [] vertices;
	vertices = NULL;
	delete [] quads;
	quads = NULL;
}

void CBoobMesh::UpdateWaterVBOMesh ( void )
{
	//==Begin OpenGL vertex buffer==

	if ( iWaterVBOverts != 0 )
		glDeleteBuffersARB( 1, &iWaterVBOverts );
	if ( iWaterVBOfaces != 0 )
		glDeleteBuffersARB( 1, &iWaterVBOfaces );
	
	glGenBuffersARB( 1, &iWaterVBOverts );
	glGenBuffersARB( 1, &iWaterVBOfaces );

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, iWaterVBOverts);         // for vertex coordinates
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iWaterVBOfaces); // for face vertex indexes

	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(CTerrainVertex)*vertexCountWater, NULL, GL_STREAM_DRAW_ARB );
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(CModelQuad)*faceCountWater, NULL, GL_STREAM_DRAW_ARB );

	glBufferSubDataARB( GL_ARRAY_BUFFER_ARB, 0,vertexCountWater*sizeof(CTerrainVertex), vertices );
	glBufferSubDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0, faceCountWater*sizeof(CModelQuad), quads );

	/*glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(CTerrainVertex)*vertexCountWater, NULL, GL_DYNAMIC_DRAW_ARB );
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(CModelQuad)*faceCountWater, NULL, GL_DYNAMIC_DRAW_ARB );

	glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );
	glMapBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );

	glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );
	glUnmapBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB );*/

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	//==End OpenGL vertex buffer==
}


void CBoobMesh::GenerateInCubeWater ( CBoob * pBoob, char const index16, char const index8, int const indexd, Vector3d const& pos, char const res )
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

	if ( vertexCountWater >= 32768 )
	{
		vertexCountWater = 32768;
		return;
	}

	// Generate FRONT
	if ( InSideVisibleWater( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, FRONT, res, dot, result_type ) )
	{
		vertices[vertexCountWater+0].x = pos.x+bsize*res;
		vertices[vertexCountWater+0].y = pos.y;
		vertices[vertexCountWater+0].z = pos.z;

		vertices[vertexCountWater+1].x = pos.x+bsize*res;
		vertices[vertexCountWater+1].y = pos.y+bsize*res;
		vertices[vertexCountWater+1].z = pos.z;

		vertices[vertexCountWater+2].x = pos.x+bsize*res;
		vertices[vertexCountWater+2].y = pos.y+bsize*res;
		vertices[vertexCountWater+2].z = pos.z+bsize*res;

		vertices[vertexCountWater+3].x = pos.x+bsize*res;
		vertices[vertexCountWater+3].y = pos.y;
		vertices[vertexCountWater+3].z = pos.z+bsize*res;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		// Set normals
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = -1;
			vertices[vertexCountWater+i].ny = 0;
			vertices[vertexCountWater+i].nz = 0;
		}

		// Set UV's
		//SetFaceUVs( vertices, FRONT, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceWaterUVs( vertices, BACK, result_type );
		SetWaterFaceColors( vertices, FRONT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	// Generate BACK
	if ( InSideVisibleWater( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, BACK, res, dot, result_type ) )
	{
		vertices[vertexCountWater+0].x = pos.x;
		vertices[vertexCountWater+0].y = pos.y;
		vertices[vertexCountWater+0].z = pos.z;

		vertices[vertexCountWater+1].x = pos.x;
		vertices[vertexCountWater+1].y = pos.y;
		vertices[vertexCountWater+1].z = pos.z+bsize*res;

		vertices[vertexCountWater+2].x = pos.x;
		vertices[vertexCountWater+2].y = pos.y+bsize*res;
		vertices[vertexCountWater+2].z = pos.z+bsize*res;

		vertices[vertexCountWater+3].x = pos.x;
		vertices[vertexCountWater+3].y = pos.y+bsize*res;
		vertices[vertexCountWater+3].z = pos.z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = 1;
			vertices[vertexCountWater+i].ny = 0;
			vertices[vertexCountWater+i].nz = 0;
		}

		// Set UV's
		//SetFaceUVs( vertices, BACK, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceWaterUVs( vertices, FRONT, result_type );
		SetWaterFaceColors( vertices, BACK, pBoob->data[index16].data[index8].light[indexd] );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	// Generate RIGHT
	if ( InSideVisibleWater( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, RIGHT, res, dot, result_type ) )
	{
		vertices[vertexCountWater+0].x = pos.x;
		vertices[vertexCountWater+0].y = pos.y;
		vertices[vertexCountWater+0].z = pos.z;

		vertices[vertexCountWater+1].x = pos.x+bsize*res;
		vertices[vertexCountWater+1].y = pos.y;
		vertices[vertexCountWater+1].z = pos.z;

		vertices[vertexCountWater+2].x = pos.x+bsize*res;
		vertices[vertexCountWater+2].y = pos.y;
		vertices[vertexCountWater+2].z = pos.z+bsize*res;

		vertices[vertexCountWater+3].x = pos.x;
		vertices[vertexCountWater+3].y = pos.y;
		vertices[vertexCountWater+3].z = pos.z+bsize*res;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = 0;
			vertices[vertexCountWater+i].ny = 1;
			vertices[vertexCountWater+i].nz = 0;
		}

		// Set UV's
		//SetFaceUVs( vertices, RIGHT, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceWaterUVs( vertices, LEFT, result_type );
		SetWaterFaceColors( vertices, RIGHT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	// Generate LEFT
	if ( InSideVisibleWater( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, LEFT, res, dot, result_type ) )
	{
		vertices[vertexCountWater+0].x = pos.x;
		vertices[vertexCountWater+0].y = pos.y+bsize*res;
		vertices[vertexCountWater+0].z = pos.z;

		vertices[vertexCountWater+1].x = pos.x;
		vertices[vertexCountWater+1].y = pos.y+bsize*res;
		vertices[vertexCountWater+1].z = pos.z+bsize*res;

		vertices[vertexCountWater+2].x = pos.x+bsize*res;
		vertices[vertexCountWater+2].y = pos.y+bsize*res;
		vertices[vertexCountWater+2].z = pos.z+bsize*res;

		vertices[vertexCountWater+3].x = pos.x+bsize*res;
		vertices[vertexCountWater+3].y = pos.y+bsize*res;
		vertices[vertexCountWater+3].z = pos.z;

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
		//SetFaceUVs( vertices, LEFT, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceWaterUVs( vertices, RIGHT, result_type );
		SetWaterFaceColors( vertices, LEFT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	// Generate TOP
	if ( InSideVisibleWater( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, TOP, res, dot, result_type ) )
	{
		vertices[vertexCountWater+0].x = pos.x;
		vertices[vertexCountWater+0].y = pos.y;
		vertices[vertexCountWater+0].z = pos.z+bsize*res;

		vertices[vertexCountWater+1].x = pos.x+bsize*res;
		vertices[vertexCountWater+1].y = pos.y;
		vertices[vertexCountWater+1].z = pos.z+bsize*res;

		vertices[vertexCountWater+2].x = pos.x+bsize*res;
		vertices[vertexCountWater+2].y = pos.y+bsize*res;
		vertices[vertexCountWater+2].z = pos.z+bsize*res;

		vertices[vertexCountWater+3].x = pos.x;
		vertices[vertexCountWater+3].y = pos.y+bsize*res;
		vertices[vertexCountWater+3].z = pos.z+bsize*res;

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
		//SetFaceUVs( vertices, TOP, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceWaterUVs( vertices, BOTTOM, result_type );
		SetWaterFaceColors( vertices, TOP, pBoob->data[index16].data[index8].light[indexd] );

		vertexCountWater += 4;
		if ( vertexCountWater < 4 ) throw xcp_vertex_overflow;
		if ( vertexCountWater > maxVertexCount ) throw xcp_vertex_overflow;
		faceCountWater += 1;
	}
	// Generate BOTTOM
	if ( InSideVisibleWater( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, BOTTOM, res, dot, result_type ) )
	{
		vertices[vertexCountWater+0].x = pos.x;
		vertices[vertexCountWater+0].y = pos.y;
		vertices[vertexCountWater+0].z = pos.z;

		vertices[vertexCountWater+1].x = pos.x;
		vertices[vertexCountWater+1].y = pos.y+bsize*res;
		vertices[vertexCountWater+1].z = pos.z;

		vertices[vertexCountWater+2].x = pos.x+bsize*res;
		vertices[vertexCountWater+2].y = pos.y+bsize*res;
		vertices[vertexCountWater+2].z = pos.z;

		vertices[vertexCountWater+3].x = pos.x+bsize*res;
		vertices[vertexCountWater+3].y = pos.y;
		vertices[vertexCountWater+3].z = pos.z;

		quads[faceCountWater].vert[0] = vertexCountWater+3;
		quads[faceCountWater].vert[1] = vertexCountWater+2;
		quads[faceCountWater].vert[2] = vertexCountWater+1;
		quads[faceCountWater].vert[3] = vertexCountWater+0;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCountWater+i].nx = 0;
			vertices[vertexCountWater+i].ny = 0;
			vertices[vertexCountWater+i].nz = 1;
		}

		// Set UV's
		//SetFaceUVs( vertices, BOTTOM, pBoob->data[index16].data[index8].data[indexd] );
		SetInFaceWaterUVs( vertices, TOP, result_type );
		SetWaterFaceColors( vertices, BOTTOM, pBoob->data[index16].data[index8].light[indexd] );

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
inline void CBoobMesh::SetInFaceWaterUVs ( CTerrainVertex* vertices, const EFaceDir dir, unsigned char data )
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
	if (( data == EB_DIRT )||( data == EB_GRASS )||( data == EB_SAND ))
	{
		texScale = 1/4.0f;
	}
	else if ( data == EB_STONE )
	{
		texScale = 1/2.0f;
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

	//atlas is 4x4
	float border = 1/1024.0f;
	for ( unsigned int i = 0; i < 4; i += 1 )
	{
		vertices[vertexCountWater+i].u *= 0.25f-(border*2);
		vertices[vertexCountWater+i].v *= 0.25f-(border*2);

		switch ( data )
		{
		case EB_GRASS:
			if ( dir == TOP )
			{
				vertices[vertexCountWater+i].u += 0.0;
				//vertices[vertexCount+i].v += 0.0;
			}
			else if ( dir == BOTTOM )
			{
				vertices[vertexCountWater+i].u += 0.50; //Dirt on bottom
				//vertices[vertexCount+i].v += 0.0;
			}
			else
			{
				// TODO: SIDES
				vertices[vertexCountWater+i].u += 0.50;
				//vertices[vertexCount+i].v += 0.0;
			}
			break;
		case EB_DIRT:
			vertices[vertexCountWater+i].u += 0.50;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case EB_STONE:
			vertices[vertexCountWater+i].u += 0.25;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case EB_SAND:
			vertices[vertexCountWater+i].u += 0.75;
			break;
		case EB_WATER: // DEBUGGGGG TODO
			vertices[vertexCountWater+i].u += 0.75;
			vertices[vertexCountWater+i].v += 0.75;
			break;
		}

		vertices[vertexCountWater+i].u += border;
		vertices[vertexCountWater+i].v += border;
	}
}

bool CBoobMesh::InSideVisibleWater ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs, Vector3d const& dot, char& return_block_type )
{
	static unsigned char checkdata = EB_NONE;
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
	return (checkdata==EB_WATER);
}
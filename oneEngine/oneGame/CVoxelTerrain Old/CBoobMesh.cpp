

#include "CBoob.h"
#include "CToBeSeen.h"

// Terrain subMesh Constructor
CBoobMesh::CBoobMesh ( void )
{
	/*vertices =	new CTerrainVertex	[32768];
	quads =		new CModelQuad		[32768];*/

	vertices =	NULL;
	quads =		NULL;
	/*physvertices=NULL;
	phystris = NULL;*/

	vertexCount = 0;
	faceCount	= 0;
	iVBOverts	= 0;
	iVBOfaces	= 0;

	needUpdateOnVBO = false;
	iVBOUpdateState = 0;

	// Create VBO
	/*glGenBuffersARB( 1, &iVBOverts );
	glGenBuffersARB( 1, &iVBOfaces );

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, iVBOverts);         // for vertex coordinates
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iVBOfaces); // for face vertex indexes

	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(CTerrainVertex)*32768, NULL, GL_STREAM_DRAW_ARB );
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(CModelQuad)*16384, NULL, GL_STREAM_DRAW_ARB );
	//iVBOType = GL_STREAM_DRAW_ARB;

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);*/

	vertexCountWater= 0;
	faceCountWater	= 0;
	iWaterVBOverts	= 0;
	iWaterVBOfaces	= 0;

	needUpdateOnWaterVBO = false;

	pShape = NULL;
	pCollision = NULL;

	iMyCollisionRef = unsigned(-1);
}

// Terrain subMesh Destructor
CBoobMesh::~CBoobMesh ( void )
{
	if ( vertices )
		delete [] vertices;
	if ( quads )
		delete [] quads;
	vertices	= NULL;
	quads		= NULL;

	// Free the buffers
	glDeleteBuffersARB( 1, &iVBOverts );
	glDeleteBuffersARB( 1, &iVBOfaces );
	glDeleteBuffersARB( 1, &iWaterVBOverts );
	glDeleteBuffersARB( 1, &iWaterVBOfaces );
	
	// Free the collision
	if ( pCollision != NULL )
		Physics::FreeRigidBody( pCollision );
	if ( iMyCollisionRef != unsigned(-1) )
		m_collisionReferences[iMyCollisionRef].b_inUse = false;

	/*delete [] vertices;
	delete [] quads;*/
	//if (( pShape != NULL )&&( pShape != m_staticCompoundShape ))
	//	Physics::FreeShape( pShape );
}

/*void CBoobMesh::UpdateRegen ( void )
{
	//==Begin OpenGL vertex buffer==

	if ( iVBOverts != 0 )
		glDeleteBuffersARB( 1, &iVBOverts );
	if ( iVBOfaces != 0 )
		glDeleteBuffersARB( 1, &iVBOfaces );
	
	glGenBuffersARB( 1, &iVBOverts );
	glGenBuffersARB( 1, &iVBOfaces );

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, iVBOverts);         // for vertex coordinates
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iVBOfaces); // for face vertex indexes

	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(CTerrainVertex)*vertexCount, NULL, GL_STREAM_DRAW_ARB );
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(CModelQuad)*faceCount, NULL, GL_STREAM_DRAW_ARB );

	// bind with 0, so, switch back to normal pointer operation
	///glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	///glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	///glBindBufferARB(GL_ARRAY_BUFFER_ARB, iVBOverts);         // for vertex coordinates
	///glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iVBOfaces); // for face vertex indexes
	glBufferSubDataARB( GL_ARRAY_BUFFER_ARB, 0,vertexCount*sizeof(CTerrainVertex), vertices );
	glBufferSubDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0, faceCount*sizeof(CModelQuad), quads );

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	//==End OpenGL vertex buffer==

	if (( vertexCount < 4 )||( faceCount < 3 ))
		return;

	//==Begin Physics==
	if ( pShape != NULL )
	{
		Physics::FreeShape( pShape );
		pShape = NULL;
	}
	if ( pShape == NULL )
	{
		// Create new shape
		pShape = Physics::CreateMeshShape( vertices, quads, vertexCount, faceCount ); //TODO NEED MOAR FAST
	}
	else
	{
		
	}
	
	if ( pCollision != NULL )
	{
		pCollision->setMotionType( physMotionType::MOTION_KEYFRAMED );
		pCollision->setShape( pShape );
		pCollision->setMotionType( physMotionType::MOTION_FIXED );
	}
	else
	{
		physRigidBodyInfo info;
		info.m_shape = pShape;									// Set the collision shape to the collider's
		info.m_motionType = physMotionType::MOTION_KEYFRAMED;	// Set the motion to static
		pCollision = Physics::CreateRigidBody( &info, false );	// Create a rigidbody and assign it to the body variable.
		pCollision->setMotionType( physMotionType::MOTION_FIXED );
	}
	//==End Physics==
}*/

C_xcp_vertex_overflow xcp_vertex_overflow;
void CBoobMesh::GenerateCube( CBoob * pBoob, char const index16, char const index8, int const indexd, Vector3d const& pos, char const res )
{
	static const int bsize = 2;
	unsigned int i = 0;
	//static Vector3d cpos;
	//cpos = pos + Vector3d( bsize*res*0.5,bsize*res*0.5,bsize*res*0.5 );
	static Vector3d dot;
	dot = pos - vCameraPos;

	static CBoobMesh* pMesh;
	pMesh = pBoob->pMesh;

	// Generate FRONT
	if ( SideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, FRONT, res, dot ) )
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

		quads[faceCount].vert[0] = vertexCount+0;
		quads[faceCount].vert[1] = vertexCount+1;
		quads[faceCount].vert[2] = vertexCount+2;
		quads[faceCount].vert[3] = vertexCount+3;

		// Set normals
		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 1;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		SetFaceUVs( vertices, FRONT, pBoob->data[index16].data[index8].data[indexd] );
		SetFaceColors( vertices, FRONT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate BACK
	if ( SideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, BACK, res, dot ) )
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

		quads[faceCount].vert[0] = vertexCount+0;
		quads[faceCount].vert[1] = vertexCount+1;
		quads[faceCount].vert[2] = vertexCount+2;
		quads[faceCount].vert[3] = vertexCount+3;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = -1;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		SetFaceUVs( vertices, BACK, pBoob->data[index16].data[index8].data[indexd] );
		SetFaceColors( vertices, BACK, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate RIGHT
	if ( SideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, RIGHT, res, dot ) )
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

		quads[faceCount].vert[0] = vertexCount+0;
		quads[faceCount].vert[1] = vertexCount+1;
		quads[faceCount].vert[2] = vertexCount+2;
		quads[faceCount].vert[3] = vertexCount+3;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = -1;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		SetFaceUVs( vertices, RIGHT, pBoob->data[index16].data[index8].data[indexd] );
		SetFaceColors( vertices, RIGHT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate LEFT
	if ( SideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, LEFT, res, dot ) )
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

		quads[faceCount].vert[0] = vertexCount+0;
		quads[faceCount].vert[1] = vertexCount+1;
		quads[faceCount].vert[2] = vertexCount+2;
		quads[faceCount].vert[3] = vertexCount+3;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = 1;
			vertices[vertexCount+i].nz = 0;
		}

		// Set UV's
		SetFaceUVs( vertices, LEFT, pBoob->data[index16].data[index8].data[indexd] );
		SetFaceColors( vertices, LEFT, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate TOP
	if ( SideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, TOP, res, dot ) )
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

		quads[faceCount].vert[0] = vertexCount+0;
		quads[faceCount].vert[1] = vertexCount+1;
		quads[faceCount].vert[2] = vertexCount+2;
		quads[faceCount].vert[3] = vertexCount+3;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = 1;
		}

		// Set UV's
		SetFaceUVs( vertices, TOP, pBoob->data[index16].data[index8].data[indexd] );
		SetFaceColors( vertices, TOP, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
	// Generate BOTTOM
	if ( SideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, BOTTOM, res, dot ) )
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

		quads[faceCount].vert[0] = vertexCount+0;
		quads[faceCount].vert[1] = vertexCount+1;
		quads[faceCount].vert[2] = vertexCount+2;
		quads[faceCount].vert[3] = vertexCount+3;

		for ( i = 0; i < 4; i += 1 )
		{
			vertices[vertexCount+i].nx = 0;
			vertices[vertexCount+i].ny = 0;
			vertices[vertexCount+i].nz = -1;
		}

		// Set UV's
		SetFaceUVs( vertices, BOTTOM, pBoob->data[index16].data[index8].data[indexd] );
		SetFaceColors( vertices, BOTTOM, pBoob->data[index16].data[index8].light[indexd] );

		vertexCount += 4;
		if ( vertexCount < 4 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
		faceCount += 1;
	}
}
inline void CBoobMesh::SetFaceUVs ( CTerrainVertex* vertices, const EFaceDir dir, unsigned char data )
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
	if (( data == EB_DIRT )||( data == EB_GRASS ))
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

	if (( dir == TOP )||( dir == FRONT )||( dir == RIGHT ))
	{
		vertices[vertexCount+1].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+1].v = vertices[vertexCount+0].v;

		vertices[vertexCount+2].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;

		vertices[vertexCount+3].u = vertices[vertexCount+0].u;
		vertices[vertexCount+3].v = vertices[vertexCount+0].v+texScale;
	}
	else
	{
		vertices[vertexCount+3].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+3].v = vertices[vertexCount+0].v;

		vertices[vertexCount+2].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;

		vertices[vertexCount+1].u = vertices[vertexCount+0].u;
		vertices[vertexCount+1].v = vertices[vertexCount+0].v+texScale;
	}
	/*else
	{

		vertices[vertexCount+0].u = vertices[vertexCount+0].u-texScale;
		vertices[vertexCount+0].v = vertices[vertexCount+0].v-texScale;

		if ( vertices[vertexCount+0].u < 0 )
			vertices[vertexCount+0].u += 1;
		if ( vertices[vertexCount+0].v < 0 )
			vertices[vertexCount+0].v += 1;
		
		vertices[vertexCount+1].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+1].v = vertices[vertexCount+0].v;

		vertices[vertexCount+2].u = vertices[vertexCount+0].u+texScale;
		vertices[vertexCount+2].v = vertices[vertexCount+0].v+texScale;

		vertices[vertexCount+3].u = vertices[vertexCount+0].u;
		vertices[vertexCount+3].v = vertices[vertexCount+0].v+texScale;
	}*/

	/*float dummy;

	vertices[vertexCount+0].u = modf(vertices[vertexCount+0].x*0.25f,&dummy);
	vertices[vertexCount+0].v = modf(vertices[vertexCount+0].y*0.25f,&dummy);

	vertices[vertexCount+1].u = modf((vertices[vertexCount+1].x+2)*0.25f,&dummy);
	vertices[vertexCount+1].v = modf(vertices[vertexCount+1].y*0.25f,&dummy);

	vertices[vertexCount+2].u = modf((vertices[vertexCount+2].x+2)*0.25f,&dummy);
	vertices[vertexCount+2].v = modf((vertices[vertexCount+2].y+2)*0.25f,&dummy);

	vertices[vertexCount+3].u = modf(vertices[vertexCount+3].x*0.25f,&dummy);
	vertices[vertexCount+3].v = modf((vertices[vertexCount+3].y+2)*0.25f,&dummy);*/

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
				// TODO: SIDES
				vertices[vertexCount+i].u += 0.50;
				//vertices[vertexCount+i].v += 0.0;
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
		}

		vertices[vertexCount+i].u += border;
		vertices[vertexCount+i].v += border;
	}
}
inline void CBoobMesh::SetFaceColors( CTerrainVertex* vertices, const EFaceDir dir, unsigned char light )
{
	float lightLevel = light/256.0f;
	vertices[vertexCount+0].r = lightLevel;
	vertices[vertexCount+0].g = lightLevel;
	vertices[vertexCount+0].b = lightLevel;
	vertices[vertexCount+0].a = 1;

	vertices[vertexCount+1].r = lightLevel;
	vertices[vertexCount+1].g = lightLevel;
	vertices[vertexCount+1].b = lightLevel;
	vertices[vertexCount+1].a = 1;

	vertices[vertexCount+2].r = lightLevel;
	vertices[vertexCount+2].g = lightLevel;
	vertices[vertexCount+2].b = lightLevel;
	vertices[vertexCount+2].a = 1;

	vertices[vertexCount+3].r = lightLevel;
	vertices[vertexCount+3].g = lightLevel;
	vertices[vertexCount+3].b = lightLevel;
	vertices[vertexCount+3].a = 1;
}


bool CBoobMesh::SideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs, Vector3d const& dot )
{
	static unsigned char checkdata = 1;
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
	return (checkdata==0);
}

#include "COctreeTerrain.h"
#include "COctreeRenderer.h"
#include "COctreeMesher.h"

#include "Math.h"
#include "MathQEF.h"

#include "CTimeProfiler.h"

using Terrain::_normal_bias;
using Terrain::_depth_bias;

inline Vector3d VertexInterp( Real isolevel, const Vector3d & p1, const Vector3d & p2, Real valp1, Real valp2 )
{
   ftype mu;
   Vector3d p;

   if ( abs(isolevel-valp1) < FTYPE_PRECISION )
      return(p1);
   if ( abs(isolevel-valp2) < FTYPE_PRECISION )
      return(p2);
   if ( abs(valp1-valp2) < FTYPE_PRECISION )
      return(p1);

   mu = Math.Clamp( (isolevel - valp1) / (valp2 - valp1), 0, 1 );
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}

void COctreeMesher::Cube_DC ( Real x, Real y, Real z )
{

}

void COctreeMesher::Vertex_DC ( const CubeIndexer& index )
{
	//Cube *cube = &(cubes[index.index]);
	Cube* cube = &(cubes[(index.x)+(index.y)*index.sizex+(index.z)*index.sizex*index.sizey]);
	Real x = index.position.x;
	Real y = index.position.y;
	Real z = index.position.z;

	// Initialize the Terrain tracker structures
	Terrain::terra_b t_blockSamples[8];
			
	// Create cube point list
	Vector3d t_vectorGrid[8];
	t_vectorGrid[0] = Vector3d( 0,0,0 );
	t_vectorGrid[1] = Vector3d( 1,0,0 );
	t_vectorGrid[2] = Vector3d( 1,1,0 );
	t_vectorGrid[3] = Vector3d( 0,1,0 );
	t_vectorGrid[4] = Vector3d( 0,0,1 );
	t_vectorGrid[5] = Vector3d( 1,0,1 );
	t_vectorGrid[6] = Vector3d( 1,1,1 );
	t_vectorGrid[7] = Vector3d( 0,1,1 );
	for ( char i = 0; i < 8; ++i ) {
		t_vectorGrid[i] = Vector3d( x,y,z ) + (t_vectorGrid[i]*m_step);
	}

	//TimeProfiler.BeginTimeProfile( "sampler0"+std::to_string(index.index) );
	// Sample the terrain information
	for ( int i = 0; i < 8; ++i ) {
		m_terrain->SampleBlock( t_vectorGrid[i], t_blockSamples[i].raw );
	}
	// Grab binary density to generate edges
	bool t_blockBinaryDensity [8];
	for ( int i = 0; i < 8; ++i ) {
		t_blockBinaryDensity[i] = Terrain::Checker::BlocktypeOpaque( t_blockSamples[i].block );
	}

	// Create the cube index
	int cubeindex = 0;
	for ( int i = 0; i < 8; ++i ) {
		// Mark if the block is solid.
		if ( t_blockBinaryDensity[i] ) {
			cubeindex |= (1 << i);
		}
	}

	// Cube is entirely in/out of the surface
	if ( edgeTable[cubeindex] == 0 ) {
		// Give it valid position, as bug in the algorithm creates random verticies
		cube->index = 0;
		cube->position = t_vectorGrid[0];
		// Give it valid blending information.
		cube->blk_type = t_blockSamples[0].block;
		//cube->blk_type[1] = t_blockSamples[7].block;
		cube->blk_weight = 0.01f;
		cube->blk_smooth = 0;
		//cube->blk_weight[1] = 0.01f;
		return;
	}
	cube->index = cubeindex;
	//TimeProfiler.EndPrintTimeProfile( "sampler0"+std::to_string(index.index) );
	//TimeProfiler.BeginTimeProfile( "generate0"+std::to_string(index.index) );
	// Generate blending information
	{
		uchar blk_type [8]		= {0,0,0,0, 0,0,0,0};
		blkflg_t blk_flags[8]	= {0,0,0,0, 0,0,0,0};
		uchar blk_weight [8]	= {0,0,0,0, 0,0,0,0};
		Real blk_ambient [8]	= {0,0,0,0, 0,0,0,0};
		for ( int i = 0; i < 8; ++i )
		{
			if ( t_blockBinaryDensity[i] )
			{
				for ( int t = 0; t < 8; ++t )
				{
					if ( t_blockSamples[i].block == blk_type[t] ) {
						blk_weight[t] += 1;
						break;
					}
					else if ( blk_type[t] == 0 ) {
						blk_type[t]	 = t_blockSamples[i].block;
						blk_flags[t].nutrients = t_blockSamples[i].nutrients;
						blk_flags[t].fluid_type = t_blockSamples[i].fluid_type;
						blk_ambient[t] = t_blockSamples[i].light_r / 15.0;
						blk_weight[t] += 1;
						break;
					}
				}
			}
		}
		uchar max = 0;
		for ( char i = 0; i < 8; ++i ) {
			if ( blk_weight[i] > blk_weight[max] ) {
				max = i;
			}
		}
		cube->blk_type		= blk_type[max] | (blk_flags[max].raw << 8);
		//cube->blk_flags	    = blk_flags[max];
		cube->blk_weight	= blk_weight[max];

		cube->blk_light_r	= blk_ambient[max];
	}

	// Generate edges
	Vector3d vertlist[12];
	Vector4d normlist[12];
	/*static int intersections[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 },
        { 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };*/
	static int intersections[12][2] = {
        { 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 },
		{ 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };
	static int table_axisMajor[12] = {
		X_POS, Y_POS, X_POS, Y_POS, X_POS, Y_POS, X_POS, Y_POS, Z_POS, Z_POS, Z_POS, Z_POS 
	};
	static int table_blockTarget[12] = {
		0, 1, 3, 0,   4, 5, 7, 4,   0, 1, 2, 3
	};

	// 1: Compute intersection points and normals
	uchar smoothCount = 0;
	Vector3d massPoint;
	Vector3d blendNormal;
    int numIntersections = 0;
	for ( char i = 0; i < 12; ++i )
	{
        if ( edgeTable[cubeindex] & (1 << i) )
		{
			// Hilariously, first grab the normal. Each edge has a normal.
			Real direction;
			{
				// Use the direction of the intersection to guess the direction of the normal
				if ( t_blockBinaryDensity[intersections[i][0]] == true && t_blockBinaryDensity[intersections[i][1]] == false ) {
					direction = 1;
				}
				else if ( t_blockBinaryDensity[intersections[i][0]] == false && t_blockBinaryDensity[intersections[i][1]] == true ) {
					direction = -1;
				}
				else {
					throw std::exception( "Invalid case" );
					direction = 0;
				}
				// Now generate the normal
				int axisMajor = table_axisMajor[i];
				int blockTarget = table_blockTarget[i];
				// 6 cases for the normal: 3 axes, 2 directions
				const Real radius = 1.0;
				if ( axisMajor == X_POS ) {
					normlist[i] = Vector4d( 0, _normal_bias(t_blockSamples[blockTarget].normal_x_y),_normal_bias(t_blockSamples[blockTarget].normal_x_z),_depth_bias(t_blockSamples[blockTarget].normal_x_w) );
					normlist[i].x = direction * sqrt( radius - sqr(normlist[i].y) - sqr(normlist[i].z) );
					if (!( normlist[i].x >= -FLT_MAX && normlist[i].x <= FLT_MAX && normlist[i].x==normlist[i].x )) {
						normlist[i].x = 0;
						if ( !BlockHasValidNormals(t_blockSamples[blockTarget]) ) {
							normlist[i] = Vector4d( direction,0,0,0.5f );
						}
					}
				}
				else if ( axisMajor == Y_POS ) {
					normlist[i] = Vector4d(_normal_bias(t_blockSamples[blockTarget].normal_y_x), 0, _normal_bias(t_blockSamples[blockTarget].normal_y_z),_depth_bias(t_blockSamples[blockTarget].normal_y_w) );
					normlist[i].y = direction * sqrt( radius - sqr(normlist[i].x) - sqr(normlist[i].z) );
					if (!( normlist[i].y >= -FLT_MAX && normlist[i].y <= FLT_MAX && normlist[i].y==normlist[i].y )) {
						normlist[i].y = 0;
						if ( !BlockHasValidNormals(t_blockSamples[blockTarget]) ) {
							normlist[i] = Vector4d( 0,direction,0,0.5f );
						}
					}
				}
				else if ( axisMajor == Z_POS ) {
					normlist[i] = Vector4d(_normal_bias(t_blockSamples[blockTarget].normal_z_x),_normal_bias(t_blockSamples[blockTarget].normal_z_y), 0, _depth_bias(t_blockSamples[blockTarget].normal_z_w) );
					normlist[i].z = direction * sqrt( radius - sqr(normlist[i].x) - sqr(normlist[i].y) );
					if (!( normlist[i].z >= -FLT_MAX && normlist[i].z <= FLT_MAX && normlist[i].z==normlist[i].z )) {
						normlist[i].z = 0;
						if ( !BlockHasValidNormals(t_blockSamples[blockTarget]) ) {
							normlist[i] = Vector4d( 0,0,direction,0.5f );
						}
					}
				}
				else {
					throw std::exception( "Invalid case" );
				}
				// Count the block smooth flags
				smoothCount += t_blockSamples[blockTarget].smooth_normal;
			}

			// Calculate the vertex position based on normal offset (more natural way to think about it when working with CSG)
			if ( direction > 0 ) {
				vertlist[i] = t_vectorGrid[intersections[i][0]].lerp( t_vectorGrid[intersections[i][1]], normlist[i].w );
			}
			else {
				vertlist[i] = t_vectorGrid[intersections[i][1]].lerp( t_vectorGrid[intersections[i][0]], normlist[i].w );
			}
			
			// Add to the mix and count the mixes
			massPoint += vertlist[i];
			blendNormal += Vector3d(normlist[i]);
			++numIntersections;
		}
	}
	massPoint /= numIntersections;
	blendNormal /= numIntersections;
	blendNormal.normalize();

	// 2: Compute QEF minimizing point
	ftype avg_amount = 0;
	Vector3d newPoint;
	Vector3d newNormal;
	do
	{
		newPoint = Vector3d();
		newNormal= Vector3d();

		double matrix[12][3];
		double vector[12];
		int rows = 0;
		for ( char i = 0; i < 12; ++i )
		{
			if ( edgeTable[cubeindex] & (1 << i) )
			{
				Vector3d normal = normlist[i];
				const ftype t_ref = 0.86;
				normal = normal.lerp( blendNormal, (((normal.dot(blendNormal)-t_ref)/(1-t_ref))*2) + avg_amount );

				matrix[rows][0] = normal.x;
				matrix[rows][1] = normal.y;
				matrix[rows][2] = normal.z;

				Vector3d p = ( vertlist[i] - massPoint ) * Math.Clamp( 1.5f - avg_amount - (smoothCount/8.0f), 0.0f, 1.0f );
				vector[rows] = normal.dot(p);

				++rows;
				newNormal += normal;   
			}
		}
		Maths::QEF::evaluate(matrix, vector, rows, &newPoint);
		// Increase blending point
		avg_amount += 0.2f;
	}
	while ( ( avg_amount < 1.1f ) && ( newPoint.sqrMagnitude() > sqr(m_step)*3.0f ) );
	// Point is out of range even after blend. Bring it back.
	/*if ( newPoint.sqrMagnitude() > sqr(m_step)*3.0f ) {
		newPoint = newPoint.normal() * sqrt(sqr(m_step)*3.0f);
	}*/
	cube->position = newPoint + massPoint;
	cube->normal = newNormal.normal();
	cube->blk_smooth = smoothCount;

	/*Vector3d pos, norm;
	int vals = 0;
	for ( char i = 0; i < 12; ++i) {
        if ( edgeTable[cubeindex] & (1 << i) ) {
			pos += vertlist[i];
			norm += normlist[i];
			vals++;
		}
	}
	cube->position = pos/vals;
	cube->normal = norm/vals;*/

	//TimeProfiler.EndPrintTimeProfile( "generate0"+std::to_string(index.index) );
	
	// Generate skirts for cubes
	if ( m_pushdown ) {
		cube->position -= cube->normal * m_step * 0.7f;
		cube->blk_smooth += 3;
	}
}

/*
void COctreeMesher::Vertex_DC_BORDER ( const CubeIndexer& index, const uint border )
{
	//Cube *cube = &(cubes[index.index]);
	Cube* cube = &(cubes[(index.x)+(index.y)*index.sizex+(index.z)*index.sizex*index.sizey]);
	Real x = index.position.x;
	Real y = index.position.y;
	Real z = index.position.z;

	// Initialize the Terrain tracker structures
	Terrain::terra_b t_blockSamples[8];
			
	// Create cube point list
	Vector3d t_vectorGrid[8];
	t_vectorGrid[0] = Vector3d( 0,0,0 );
	t_vectorGrid[1] = Vector3d( 1,0,0 );
	t_vectorGrid[2] = Vector3d( 1,1,0 );
	t_vectorGrid[3] = Vector3d( 0,1,0 );
	t_vectorGrid[4] = Vector3d( 0,0,1 );
	t_vectorGrid[5] = Vector3d( 1,0,1 );
	t_vectorGrid[6] = Vector3d( 1,1,1 );
	t_vectorGrid[7] = Vector3d( 0,1,1 );
	if ( border & 0x01 ) { // When odd, offset forward to next sample position.
		for ( char i = 0; i < 8; ++i ) {
			if ( t_vectorGrid[i].x > 0.5f ) continue;
			//t_vectorGrid[i].x = (border&0x40) ? 1.0f : 0.0f;
			t_vectorGrid[i].y = (border&0x80) ? 1.0f : 0.0f;
			t_vectorGrid[i].z = (border&0x100) ? 1.0f : 0.0f;
		}
	}
	if ( border & 0x02 ) {
		for ( char i = 0; i < 8; ++i ) {
			if ( t_vectorGrid[i].y > 0.5f ) continue;
			t_vectorGrid[i].x = (border&0x40) ? 1.0f : 0.0f;
			//t_vectorGrid[i].y = (border&0x80) ? 1.0f : 0.0f;
			t_vectorGrid[i].z = (border&0x100) ? 1.0f : 0.0f;
		}
	}
	if ( border & 0x04 ) {
		for ( char i = 0; i < 8; ++i ) {
			if ( t_vectorGrid[i].z > 0.5f ) continue;
			t_vectorGrid[i].x = (border&0x40) ? 1.0f : 0.0f;
			t_vectorGrid[i].y = (border&0x80) ? 1.0f : 0.0f;
			//t_vectorGrid[i].z = (border&0x100) ? 1.0f : 0.0f;
		}
	}
	if ( border & 0x08 ) {
		for ( char i = 0; i < 8; ++i ) {
			if ( t_vectorGrid[i].x < 0.5f ) continue;
			t_vectorGrid[i].y = (border&0x80) ? 1.0f : 0.0f;
			t_vectorGrid[i].z = (border&0x100) ? 1.0f : 0.0f;
		}
	}
	if ( border & 0x10 ) {
		for ( char i = 0; i < 8; ++i ) {
			if ( t_vectorGrid[i].y < 0.5f ) continue;
			t_vectorGrid[i].x = (border&0x40) ? 1.0f : 0.0f;
			t_vectorGrid[i].z = (border&0x100) ? 1.0f : 0.0f;
		}
	}
	if ( border & 0x20 ) {
		for ( char i = 0; i < 8; ++i ) {
			if ( t_vectorGrid[i].z < 0.5f ) continue;
			t_vectorGrid[i].x = (border&0x40) ? 1.0f : 0.0f;
			t_vectorGrid[i].y = (border&0x80) ? 1.0f : 0.0f;
		}
	}
	for ( char i = 0; i < 8; ++i ) {
		t_vectorGrid[i] = Vector3d( x,y,z ) + (t_vectorGrid[i]*m_step);
	}

	// Sample the terrain information
	for ( int i = 0; i < 8; ++i ) {
		m_terrain->SampleBlock( t_vectorGrid[i], t_blockSamples[i].raw );
	}
	// Grab binary density to generate edges
	bool t_blockBinaryDensity [8];
	for ( int i = 0; i < 8; ++i ) {
		t_blockBinaryDensity[i] = m_renderer->BlocktypeOpaque( t_blockSamples[i].block );
	}

	// Create the cube index
	int cubeindex = 0;
	for ( int i = 0; i < 8; ++i ) {
		// Mark if the block is solid.
		if ( t_blockBinaryDensity[i] ) {
			cubeindex |= (1 << i);
		}
	}

	// Cube is entirely in/out of the surface
	if ( edgeTable[cubeindex] == 0 ) {
		// Give it valid position, as bug in the algorithm creates random verticies
		cube->index = 0;
		cube->position = t_vectorGrid[0];
		// Give it valid blending information.
		cube->blk_type = t_blockSamples[0].block;
		cube->blk_weight = 0.01f;
		return;
	}
	cube->index = cubeindex;
	// Generate blending information
	{
		uchar blk_type [8]		= {0,0,0,0, 0,0,0,0};
		blkflg_t blk_flags[8]	= {0,0,0,0, 0,0,0,0};
		uchar blk_weight [8]	= {0,0,0,0, 0,0,0,0};
		for ( int i = 0; i < 8; ++i )
		{
			if ( t_blockBinaryDensity[i] )
			{
				for ( int t = 0; t < 8; ++t )
				{
					if ( t_blockSamples[i].block == blk_type[t] ) {
						blk_weight[t] += 1;
						break;
					}
					else if ( blk_type[t] == 0 ) {
						blk_type[t]	 = t_blockSamples[i].block;
						blk_flags[t].nutrients = t_blockSamples[i].nutrients;
						blk_flags[t].fluid_type = t_blockSamples[i].fluid_type;
						blk_weight[t] += 1;
						break;
					}
				}
			}
		}
		uchar max = 0;
		for ( char i = 0; i < 8; ++i ) {
			if ( blk_weight[i] > blk_weight[max] ) {
				max = i;
			}
		}
		cube->blk_type		= blk_type[max] | (blk_flags[max].raw << 8);
		cube->blk_weight	= blk_weight[max];
	}

	// Generate edges
	Vector3d vertlist[12];
	Vector4d normlist[12];
	static int intersections[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 },
        { 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };
	static int table_axisMajor[12] = {
		X_POS, Y_POS, X_POS, Y_POS, X_POS, Y_POS, X_POS, Y_POS, Z_POS, Z_POS, Z_POS, Z_POS 
	};
	static int table_blockTarget[12] = {
		0, 1, 3, 0,   4, 5, 7, 4,   0, 1, 2, 3
	};

	// 1: Compute intersection points and normals
	Vector3d massPoint;
	Vector3d blendNormal;
    int numIntersections = 0;
	for ( char i = 0; i < 12; ++i )
	{
		if ( edgeTable[cubeindex] & (1 << i) ) {
			// Hilariously, first grab the normal. Each edge has a normal.
			Real direction;
			{
				// Use the direction of the intersection to guess the direction of the normal
				if ( t_blockBinaryDensity[intersections[i][0]] == true && t_blockBinaryDensity[intersections[i][1]] == false ) {
					direction = 1;
				}
				else if ( t_blockBinaryDensity[intersections[i][0]] == false && t_blockBinaryDensity[intersections[i][1]] == true ) {
					direction = -1;
				}
				else {
					throw std::exception( "Invalid case" );
					direction = 0;
				}
				// Now generate the normal
				uchar axisMajor = table_axisMajor[i];
				uchar blockTarget = table_blockTarget[i];
				if ( !BlockHasValidNormals(t_blockSamples[blockTarget]) ) {
					//throw std::exception();
				}
				const Real radius = 1.0;
				if ( axisMajor == X_POS ) {
					normlist[i] = Vector4d( 0, _normal_bias(t_blockSamples[blockTarget].normal_x_y),_normal_bias(t_blockSamples[blockTarget].normal_x_z),_depth_bias(t_blockSamples[blockTarget].normal_x_w) );
					normlist[i].x = direction * sqrt( radius - sqr(normlist[i].y) - sqr(normlist[i].z) );
					if (!( normlist[i].x >= -FLT_MAX && normlist[i].x <= FLT_MAX && normlist[i].x==normlist[i].x )) {
						normlist[i].x = 0;
						if ( !BlockHasValidNormals(t_blockSamples[blockTarget]) ) {
							normlist[i] = Vector4d( direction,0,0,0.5f );
						}
					}
				}
				else if ( axisMajor == Y_POS ) {
					normlist[i] = Vector4d(_normal_bias(t_blockSamples[blockTarget].normal_y_x), 0, _normal_bias(t_blockSamples[blockTarget].normal_y_z),_depth_bias(t_blockSamples[blockTarget].normal_y_w) );
					normlist[i].y = direction * sqrt( radius - sqr(normlist[i].x) - sqr(normlist[i].z) );
					if (!( normlist[i].y >= -FLT_MAX && normlist[i].y <= FLT_MAX && normlist[i].y==normlist[i].y )) {
						normlist[i].y = 0;
						if ( !BlockHasValidNormals(t_blockSamples[blockTarget]) ) {
							normlist[i] = Vector4d( 0,direction,0,0.5f );
						}
					}
				}
				else if ( axisMajor == Z_POS ) {
					normlist[i] = Vector4d(_normal_bias(t_blockSamples[blockTarget].normal_z_x),_normal_bias(t_blockSamples[blockTarget].normal_z_y), 0, _depth_bias(t_blockSamples[blockTarget].normal_z_w) );
					normlist[i].z = direction * sqrt( radius - sqr(normlist[i].x) - sqr(normlist[i].y) );
					if (!( normlist[i].z >= -FLT_MAX && normlist[i].z <= FLT_MAX && normlist[i].z==normlist[i].z )) {
						normlist[i].z = 0;
						if ( !BlockHasValidNormals(t_blockSamples[blockTarget]) ) {
							normlist[i] = Vector4d( 0,0,direction,0.5f );
						}
					}
				}
			}

			// Calculate the vertex position based on either density, or the parametric density
			if ( direction > 0 ) {
				vertlist[i] = t_vectorGrid[intersections[i][0]].lerp( t_vectorGrid[intersections[i][1]], normlist[i].w );
			}
			else {
				vertlist[i] = t_vectorGrid[intersections[i][1]].lerp( t_vectorGrid[intersections[i][0]], normlist[i].w );
			}

			massPoint += vertlist[i];
			blendNormal += normlist[i];
			++numIntersections;
		}
	}
	massPoint /= numIntersections;
	blendNormal /= numIntersections;
	blendNormal.normalize();

	// 2: Compute QEF minimizing point
    Vector3d newNormal;
	double matrix[12][3];
    double vector[12];
    int rows = 0;
    for ( char i = 0; i < 12; ++i) {
        if ( edgeTable[cubeindex] & (1 << i) )
		{
			Vector3d normal = normlist[i];

			const ftype t_ref = 0.86;
			normal = normal.lerp( blendNormal, ((normal.dot(blendNormal)-t_ref)/(1-t_ref))*2 );

			matrix[rows][0] = normal.x;
			matrix[rows][1] = normal.y;
			matrix[rows][2] = normal.z;

			Vector3d p = vertlist[i] - massPoint;
			vector[rows] = normal.dot(p);

			++rows;
			newNormal += normal;   
		}
	}
	Vector3d newPoint;
	Maths::QEF::evaluate(matrix, vector, rows, &newPoint);
	if ( newPoint.sqrMagnitude() > sqr(m_step)*3 ) {
		newPoint = newPoint.normal() * sqrt(sqr(m_step)*3);
	}
	cube->position = newPoint + massPoint;
	cube->normal = newNormal.normal();
}*/

				/*switch ( i ) {
					case 0: normlist[i] = Vector4d( direction, t_blockSamples[0].normal_x_y,t_blockSamples[0].normal_x_z,t_blockSamples[0].normal_x_w );	break;
					case 1: normlist[i] = Vector4d(t_blockSamples[1].normal_y_x, direction, t_blockSamples[1].normal_y_z,t_blockSamples[1].normal_y_w );	break;
					case 2: normlist[i] = Vector4d( direction, t_blockSamples[3].normal_x_y,t_blockSamples[3].normal_x_z,t_blockSamples[3].normal_x_w );	break;
					case 3: normlist[i] = Vector4d(t_blockSamples[0].normal_y_x, direction, t_blockSamples[0].normal_y_z,t_blockSamples[0].normal_y_w );	break;
					case 4: normlist[i] = Vector4d( direction, t_blockSamples[4].normal_x_y,t_blockSamples[4].normal_x_z,t_blockSamples[4].normal_x_w );	break;
					case 5: normlist[i] = Vector4d(t_blockSamples[5].normal_y_x, direction, t_blockSamples[5].normal_y_z,t_blockSamples[5].normal_y_w );	break;
					case 6: normlist[i] = Vector4d( direction, t_blockSamples[7].normal_x_y,t_blockSamples[7].normal_x_z,t_blockSamples[7].normal_x_w );	break;
					case 7: normlist[i] = Vector4d(t_blockSamples[4].normal_y_x, direction, t_blockSamples[4].normal_y_z,t_blockSamples[4].normal_y_w );	break;
					case 8: normlist[i] = Vector4d(t_blockSamples[0].normal_z_x,t_blockSamples[0].normal_z_y, direction, t_blockSamples[0].normal_z_w );	break;
					case 9: normlist[i] = Vector4d(t_blockSamples[1].normal_z_x,t_blockSamples[1].normal_z_y, direction, t_blockSamples[1].normal_z_w );	break;
					case 10:normlist[i] = Vector4d(t_blockSamples[2].normal_z_x,t_blockSamples[2].normal_z_y, direction, t_blockSamples[2].normal_z_w );	break;
					case 11:normlist[i] = Vector4d(t_blockSamples[3].normal_z_x,t_blockSamples[3].normal_z_y, direction, t_blockSamples[3].normal_z_w );	break;
				}*/

void COctreeMesher::Triangulate_DC ( const CubeIndexer& index )
{
#define CUBE_PTR(xi,yi,zi) &(cubes[(index.x+xi)+(index.y+yi)*index.sizex+(index.z+zi)*index.sizex*index.sizey])
	/*struct _CUBE_SAMPLER
	{
		Cube* cubes;
		const CubeIndexer& index;
		bool* m_split_border;
		bool newchoice ( int tx, int ty, int tz )
		{
			if ( cubes[tx+ty*index.sizex+tz*index.sizex*index.sizey].index != 0 ) {
				return true;
			}
			return false;
		}
		Cube* operator() ( int xi, int yi, int zi ) {
			int tx = index.x+xi;
			int ty = index.y+yi;
			int tz = index.z+zi;
			return &(cubes[tx+ty*index.sizex+tz*index.sizex*index.sizey]);
		}
	} CUBE_PTR = {cubes,index,m_split_border};*/

	Cube *tCubes[4];
	tCubes[0] = CUBE_PTR(0,0,0);

    int cube0_edgeInfo = edgeTable[tCubes[0]->index];
    int flip_if_nonzero = 0;
	// Check the 3 edge cases
	for (int i = 0; i < 3; ++i)
	{
        if ( i == 0 && cube0_edgeInfo & (1 << 10) ) {
            tCubes[1] = CUBE_PTR(1,0,0);
            tCubes[2] = CUBE_PTR(1,1,0);
            tCubes[3] = CUBE_PTR(0,1,0);
            flip_if_nonzero = (tCubes[0]->index & (1 << 6));
        }
		else if ( i == 1 && cube0_edgeInfo & (1 << 6) ) {
            tCubes[1] = CUBE_PTR(0,0,1);
            tCubes[2] = CUBE_PTR(0,1,1);
            tCubes[3] = CUBE_PTR(0,1,0);
            flip_if_nonzero = (tCubes[0]->index & (1 << 7));
        }
		else if ( i == 2 && cube0_edgeInfo & (1 << 5) ) {
            tCubes[1] = CUBE_PTR(1,0,0);
            tCubes[2] = CUBE_PTR(1,0,1);
            tCubes[3] = CUBE_PTR(0,0,1);
            flip_if_nonzero = (tCubes[0]->index & (1 << 5));
        }
		else {
            continue;
		}

		Cube* form [3];
		form[0] = tCubes[0];

        for (int j = 1; j < 3; ++j)
		{
            int ja, jb;
            if (flip_if_nonzero) {
                ja = j + 0;
                jb = j + 1;
            } else {
                ja = j + 1;
                jb = j + 0;
            }

			form[1] = tCubes[ja];
			form[2] = tCubes[jb];

			Real	blk_weight	[6] = {0,0,0,0,0,0};
			uchar	blk_type	[6] = {0,0,0,0,0,0};

			// set positions
			for ( char v = 0; v < 3; ++ v ) {
				vertices[vertexCount+v].x = form[v]->position.x;
				vertices[vertexCount+v].y = form[v]->position.y;
				vertices[vertexCount+v].z = form[v]->position.z;
			}
			// calculate normals
			Vector3d hardNormal = (form[0]->position-form[1]->position).cross(form[2]->position-form[0]->position).normal();
			for ( char v = 0; v < 3; ++ v )
			{
				if ( (form[v]->blk_smooth >= 2) || fabs(hardNormal.dot(form[v]->normal)) < 0.70 )
				{
					vertices[vertexCount+v].nx = form[v]->normal.x;
					vertices[vertexCount+v].ny = form[v]->normal.y;
					vertices[vertexCount+v].nz = form[v]->normal.z;
				}
				else
				{
					vertices[vertexCount+v].nx = hardNormal.x;
					vertices[vertexCount+v].ny = hardNormal.y;
					vertices[vertexCount+v].nz = hardNormal.z;
				}
			}
			// Push down for skirts
			/*if ( m_pushdown )
			{
				for ( char v = 0; v < 3; ++ v ) {
					vertices[vertexCount+v].x -= vertices[vertexCount+v].nx;
					vertices[vertexCount+v].y -= vertices[vertexCount+v].ny;
					vertices[vertexCount+v].z -= vertices[vertexCount+v].nz;
				}
			}*/

			// set the rest of the vertex properties
			for ( char v = 0; v < 3; ++ v ) {
				vertices[vertexCount+v].r = 1 - form[v]->blk_light_r; // ambient color
				vertices[vertexCount+v].g = BlockGetSpecular(form[v]->blk_type); // specular lighting
				vertices[vertexCount+v].b = BlockGetSaturation(form[v]->blk_type); // saturation
				vertices[vertexCount+v].a = BlockGetGlow(form[v]->blk_type); // glow amount
				// l4 used for change brightness
				vertices[vertexCount+v].l4 = BlockGetBrightness(form[v]->blk_type);
				// m4 used for change hue
				vertices[vertexCount+v].m4 = BlockGetHue(form[v]->blk_type);

				//vertices[vertexCount+v].u = 0.25f; // scaling
				//vertices[vertexCount+v].l4 = 0.25f; // scaling

				/*vertices[vertexCount+v].v = BlockGetTexW(blk_type[maxBlocks[0]]); // block 0 type
				vertices[vertexCount+v].l1 = blk_weight[maxBlocks[0]]; // block 0 weight
				vertices[vertexCount+v].w = BlockGetTexW(blk_type[maxBlocks[1]]); // block 1 type
				vertices[vertexCount+v].l2 = blk_weight[maxBlocks[1]]; // block 1 weight*/
				vertices[vertexCount+v].u = BlockGetTexW(form[0]->blk_type);
				vertices[vertexCount+v].l1 = (v==0) ? (form[0]->blk_weight) : 0;
				vertices[vertexCount+v].m1 = BlockGetTexScale(form[0]->blk_type);
				vertices[vertexCount+v].v = BlockGetTexW(form[1]->blk_type);
				vertices[vertexCount+v].l2 = (v==1) ? (form[1]->blk_weight) : 0;
				vertices[vertexCount+v].m2 = BlockGetTexScale(form[1]->blk_type);
				vertices[vertexCount+v].w = BlockGetTexW(form[2]->blk_type);
				vertices[vertexCount+v].l3 = (v==2) ? (form[2]->blk_weight) : 0;
				vertices[vertexCount+v].m3 = BlockGetTexScale(form[2]->blk_type);
				//vertices[vertexCount+v].l4 = 0;
			}
			triangles[triangleCount].vert[0] = vertexCount+2;
			triangles[triangleCount].vert[1] = vertexCount+1;
			triangles[triangleCount].vert[2] = vertexCount+0;

			vertexCount += 3;
			triangleCount += 1;
        }
	}

#undef CUBE_PTR
}